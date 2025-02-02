#include "pch.h"
#include "ChunkBuilder.h"

BlockProperties* air = new BlockProperties(BlockTypeEnum::Air, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
BlockProperties* grass = new BlockProperties(BlockTypeEnum::Grass, glm::vec4(0.5f, 0.8f, 0.3f, 1.0f));
BlockProperties* dirt = new BlockProperties(BlockTypeEnum::Dirt, glm::vec4(0.6f, 0.3f, 0.1f, 1.0f));
BlockProperties* stone = new BlockProperties(BlockTypeEnum::Stone, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

static const BlockProperties* BlockPropertiesFromType(BlockTypeEnum blockType)
{
    switch (blockType) {
    case BlockTypeEnum::Air: return air;
    case BlockTypeEnum::Grass: return grass;
    case BlockTypeEnum::Dirt: return dirt;
    case BlockTypeEnum::Stone: return stone;
    }
    return nullptr;
}

ChunkBuilder::ChunkBuilder(World* world)
	: _world(world)
{
}

void ChunkBuilder::PushBuildChunk(const std::shared_ptr<Chunk>& chunk)
{
	std::scoped_lock lock(_lock);
	_pendingData.push(chunk);
}

void ChunkBuilder::PushRebuildChunkMesh(const std::shared_ptr<Chunk>& chunk)
{
	std::scoped_lock lock(_lock);
	_pendingMesh.push(chunk);
}

void ChunkBuilder::BuildChunk(const std::shared_ptr<Chunk>& chunk)
{
    const glm::ivec2& pos = chunk->pos();

    const float baseFrequency = 0.005f;   // Lower frequency for smoother noise
    const float amplitude = 0.5f;         // Base amplitude for height
    const int surfaceHeight = 64;         // Default surface height
    const int octaves = 8;                // Fewer octaves for smoother terrain
    const float persistence = 0.4f;       // Lower persistence to make terrain smoother
    const int maxTerrainHeight = 256.0f;     // Max height for the terrain (mountain peaks)

    int cx = (pos.x * CHUNK_WIDTH);
    int cz = (pos.y * CHUNK_DEPTH);

    // Function to generate multi-octave Perlin noise
    auto generateNoise = [octaves, persistence, baseFrequency](float x, float z) -> float {
        float noiseValue = 0.0f;
        float frequency = baseFrequency;
        float amplitudeModifier = 1.0f;

        for (int i = 0; i < octaves; i++) {
            noiseValue += glm::perlin(glm::vec2(x * frequency, z * frequency)) * amplitudeModifier;
            frequency *= 2.0f;  // Increase frequency for finer details
            amplitudeModifier *= persistence;  // Decrease amplitude for finer details
        }

        return noiseValue;
    };

    for (int x = 0; x < CHUNK_WIDTH; x++) {
        for (int z = 0; z < CHUNK_DEPTH; z++) {

            int worldX = cx + x;
            int worldZ = cz + z;

            float noiseValue = generateNoise(worldX, worldZ);

            float normalizedNoise = (noiseValue + 1.0f) * 0.5f;
            float remappedHeight = normalizedNoise * maxTerrainHeight;

            // Apply smoothing or biasing if necessary (e.g., clamping to the chunk height)
            int mappedHeight = (int)std::clamp(remappedHeight, 0.0f, 255.0f);  // Clamp height within bounds

            for (int sectionIndex = 0; sectionIndex < 16; sectionIndex++) 
            {
                ChunkSegment* section = chunk->GetChunkSegment(sectionIndex);

                for (int y = 0; y < CHUNK_HEIGHT; y++) 
                {
                    int worldY = (sectionIndex * CHUNK_HEIGHT) + y;
                    
                    // air is default if not mapped so no need to map it

                    if (worldY == mappedHeight) {
                        section->SetBlockType(x, y, z, BlockTypeEnum::Grass);  // Grass blocks at the surface height
                    }
                    else if (worldY < mappedHeight && worldY > mappedHeight - 4) {
                        section->SetBlockType(x, y, z, BlockTypeEnum::Dirt);   // Dirt blocks just below the grass
                    }
                    else if (worldY <= mappedHeight - 4) {
                        section->SetBlockType(x, y, z, BlockTypeEnum::Stone);  // Stone blocks below the dirt
                    }
                }
            }
        }
    }

    chunk->MakeDirty(true);
    //chunk->MarkNeighboursDirty(); // todo fix its broken? mabye ignore neihgbours
    chunk->SetGenerated(true);

    {
        std::scoped_lock lock(_lock);
        _activeBuilderTasks--;
    }
}

void ChunkBuilder::RebuildMesh(const std::shared_ptr<Chunk>& chunk)
{
    ChunkMesh* mesh = chunk->mesh();

    mesh->SetUpdateMesh(false);
    chunk->SetBuilding(true);

    const glm::ivec2& pos = chunk->pos();

    auto checkNeighbour = [this, pos](int x, int y, int z)
    { 
        const glm::ivec2 chunkPos = _world->WorldToChunk(x, z);

        const std::shared_ptr<Chunk>& chunk = _world->GetChunk(chunkPos.x, chunkPos.y);
        if (chunk == nullptr) {
            return false;
        }

        const glm::ivec3 local = _world->WorldToLocal(x, y, z);

        int segmentIndex = y / 16;

        ChunkSegment* segment = chunk->GetChunkSegment(segmentIndex);
        BlockTypeEnum blockType = segment->GetBlockType(local.x, local.y, local.z);
        return blockType == BlockTypeEnum::Air;
    };

    for (int i = 0; i < 16; i++) {
        ChunkSegment* segment = chunk->GetChunkSegment(i);
        if (segment->IsFullyAir()) {
            continue;
        }

        for (int x = 0; x < 16; x++) {
            for (int z = 0; z < 16; z++) {
                for (int y = 0; y < 16; y++) {

                    BlockTypeEnum blockType = segment->GetBlockType(x, y, z);
                    if (blockType == BlockTypeEnum::Air) {
                        continue;
                    }

                    const BlockProperties* blockProperties = BlockPropertiesFromType(blockType);
                    if (blockProperties == nullptr) { // Uh-oh, no block properties?
                        continue;
                    }

                    float worldX = (pos.x * CHUNK_HEIGHT) + x;
                    float worldY = (i * CHUNK_HEIGHT) + y;
                    float worldZ = (pos.y * CHUNK_HEIGHT) + z;

                    bool isFrontEdge = z == 0;
                    bool isBackEdge = z == CHUNK_DEPTH - 1;

                    bool isLeftEdge = x == 0;
                    bool isRightEdge = x == CHUNK_WIDTH - 1;

                    bool showTopFace = segment->GetBlockType(x, y + 1, z) == BlockTypeEnum::Air;
                    bool showBottomFace = segment->GetBlockType(x, y - 1, z) == BlockTypeEnum::Air;

                    bool showFrontFace = segment->GetBlockType(x, y, z + 1) == BlockTypeEnum::Air;
                    bool showBackFace = segment->GetBlockType(x, y, z - 1) == BlockTypeEnum::Air;

                    bool showLeftFace = segment->GetBlockType(x - 1, y, z) == BlockTypeEnum::Air;
                    bool showRightFace = segment->GetBlockType(x + 1, y, z) == BlockTypeEnum::Air;

                    //ChunkSegment* neighborSegment = chunk->GetChunkSegment(i + 1);
                    //if (neighborSegment == nullptr || neighborSegment->GetBlockType(x, 0, z) != BlockTypeEnum::Air) {
                    //    showTopFace = false;
                    //}

                    //neighborSegment = chunk->GetChunkSegment(i - 1);
                    //if (neighborSegment == nullptr || neighborSegment->GetBlockType(x, 15, z) != BlockTypeEnum::Air) {
                    //    showBottomFace = false;
                    //}

                    // todo figure out how to cull unneeded faces
                    //if (isLeftEdge)
                    //{
                    //    const std::shared_ptr<Chunk>& leftChunk = _world->GetChunk(pos.x - 1, pos.y);
                    //    if (leftChunk != nullptr) {

                    //        ChunkSegment* relativeChunkSegment = leftChunk->GetChunkSegment(i);
                    //        if (relativeChunkSegment == nullptr) {
                    //            __debugbreak(); // huh how?
                    //        }

                    //        showLeftFace = relativeChunkSegment->GetBlockType(15, y, z) == BlockTypeEnum::Air;
                    //    }
                    //}

                    //if (isRightEdge)
                    //{
                    //    const std::shared_ptr<Chunk>& rightChunk = _world->GetChunk(pos.x + 1, pos.y);
                    //    if (rightChunk != nullptr) {

                    //        ChunkSegment* relativeChunkSegment = rightChunk->GetChunkSegment(i);
                    //        if (relativeChunkSegment == nullptr) {
                    //            __debugbreak(); // huh how?
                    //        }

                    //        showRightFace = relativeChunkSegment->GetBlockType(0, y, z) == BlockTypeEnum::Air;
                    //    }
                    //}

                    //if (isFrontEdge)
                    //{
                    //    const std::shared_ptr<Chunk>& frontChunk = _world->GetChunk(pos.x, pos.y + 1);
                    //    if (frontChunk != nullptr) {

                    //        ChunkSegment* relativeChunkSegment = frontChunk->GetChunkSegment(i);
                    //        if (relativeChunkSegment == nullptr) {
                    //            __debugbreak(); 
                    //        }
                    //        showFrontFace = relativeChunkSegment->GetBlockType(x, y, 0) == BlockTypeEnum::Air;
                    //    }
                    //}

                    //if (isBackEdge)
                    //{
                    //    const std::shared_ptr<Chunk>& backChunk = _world->GetChunk(pos.x, pos.y - 1);
                    //    if (backChunk != nullptr) {

                    //        ChunkSegment* relativeChunkSegment = backChunk->GetChunkSegment(i);
                    //        if (relativeChunkSegment == nullptr) {
                    //            __debugbreak();
                    //        }
                    //        showBackFace = relativeChunkSegment->GetBlockType(x, y, 15) == BlockTypeEnum::Air;
                    //    }
                    //}

                    if (showTopFace) {
                        mesh->AddFace(BlockFace::Top, blockProperties, x, worldY, z);
                    }

                    if (showBottomFace) {
                        mesh->AddFace(BlockFace::Bottom, blockProperties, x, worldY, z);
                    }

                    if (showFrontFace) {
                        mesh->AddFace(BlockFace::Front, blockProperties, x, worldY, z);
                    }

                    if (showBackFace) {
                        mesh->AddFace(BlockFace::Back, blockProperties, x, worldY, z);
                    }

                    if (showLeftFace) {
                        mesh->AddFace(BlockFace::Left, blockProperties, x, worldY, z);
                    }

                    if (showRightFace) {
                        mesh->AddFace(BlockFace::Right, blockProperties, x, worldY, z);
                    }
                }
            }
        }
    }

    mesh->SetUpdateMesh(true);
    chunk->SetBuilding(false);

    {
        std::scoped_lock lock(_lock);
        _activeMeshTasks--;
    }
}

void ChunkBuilder::Process()
{
    std::shared_ptr<Chunk> dataToBuild;
    std::shared_ptr<Chunk> chunkToRebuild;

    {
        std::scoped_lock lock(_lock);

        while (!_pendingData.empty() && _activeBuilderTasks < MAX_BUILDER_TASKS) {
            dataToBuild = _pendingData.front();
            _pendingData.pop();
            _activeBuilderTasks++;

            _threadPool.Enqueue([this, dataToBuild]() {
                BuildChunk(dataToBuild);
            });
        }

        while (!_pendingMesh.empty() && _activeMeshTasks < MAX_MESH_TASKS) {
            chunkToRebuild = _pendingMesh.front();
            _pendingMesh.pop();
            _activeMeshTasks++;

            _threadPool.Enqueue([this, chunkToRebuild]() {
                RebuildMesh(chunkToRebuild);
            });
        }
    }

    if (_activeBuilderTasks > 0 || _activeMeshTasks > 0) {
        std::cout << "Data: " << _activeBuilderTasks << " Meshes: " << _activeMeshTasks << '\n';
    }
}
