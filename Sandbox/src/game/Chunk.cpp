#include "pch.h"
#include "Chunk.h"
#include "World.h"

#include "threading/CommandQueue.h"

Chunk::Chunk(World* world, const glm::ivec2& pos)
    : _world(world), _x(pos.x), _z(pos.y), _generated(false), _dirty(false), _building(false), _updateMesh(false)
{
    _mesh = new ChunkMesh();
    for (int i = 0; i < 16; i++) {
        _chunkSegments[i] = new ChunkSegment();
    }
}

Chunk::~Chunk()
{
    for (int i = 0; i < 16; i++) {
        delete _chunkSegments[i];
    }
    delete _mesh;
}

ChunkSegment* Chunk::GetChunkSegment(int index)
{
    if (index < 0 || index >= 16) {
        return nullptr;
    }
    return _chunkSegments[index];

}

void Chunk::MarkNeighboursDirty()
{
    static std::vector<glm::ivec2> offsets = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };

    for (const glm::ivec2& offset : offsets) {
        std::shared_ptr<Chunk> neighbour = _world->GetChunk(_x + offset.x, _z + offset.y);

        if (neighbour == nullptr || !neighbour->_generated || neighbour->_building) {
            continue;
        }

        neighbour->_dirty = true;
    }
}

void Chunk::Render()
{
    _mesh->Render();
}

glm::ivec2 Chunk::pos() const
{
    return { _x, _z };
}

ChunkMesh* Chunk::mesh() const
{
    return _mesh;
}

bool Chunk::generated() const
{
    return _generated;
}

bool Chunk::dirty() const
{
    return _dirty;
}

void Chunk::SetGenerated(bool generated)
{
    _generated = generated;
}

void Chunk::SetBuilding(bool building)
{
    _building = building;
}

void Chunk::MakeDirty(bool dirty)
{
    _dirty = dirty;
}

bool Chunk::building() const
{
    return _building;
}