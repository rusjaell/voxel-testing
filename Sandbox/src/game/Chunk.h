#ifndef CHUNK_H
#define CHUNK_H

#include "BlockType.h"
#include "renderer/VertexArray.h"
#include "renderer/VertexBuffer.h"
#include "renderer/IndexBuffer.h"

#include <vector>
#include <unordered_map>
#include <mutex>
#include <array>

constexpr int CHUNK_WIDTH = 16;
constexpr int CHUNK_HEIGHT = 16;
constexpr int CHUNK_DEPTH = 16;
constexpr int CHUNK_SIZE = CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH;

class ChunkSegment 
{
public:
    void SetBlockType(int x, int y, int z, BlockTypeEnum blockType) {
        int blockIndex = GetBlockIndex(x, y, z);
        int paletteIndex = GetOrAddPalette(blockType);
        _blocks[blockIndex] = paletteIndex;
        if (blockType != BlockTypeEnum::Air) {
            _isFullyAir = false;
        }
    }

    BlockTypeEnum GetBlockType(int x, int y, int z) {
        int blockIndex = GetBlockIndex(x, y, z);
        if (blockIndex == -1) {
            return BlockTypeEnum::Air;
        }
        int paletteIndex = _blocks[blockIndex];
        return _palette[paletteIndex];
    }

    bool isFullyAir() const {
        return _isFullyAir;
    }

private: 
    int GetBlockIndex(int x, int y, int z) {
        if (x < 0 || x >= CHUNK_WIDTH || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_DEPTH) {
            return -1;
        }
        return x + y * CHUNK_WIDTH + z * CHUNK_WIDTH * CHUNK_HEIGHT;
    }

    int GetOrAddPalette(BlockTypeEnum blockType) 
    {
        std::vector<BlockTypeEnum>::iterator it = std::find(_palette.begin(), _palette.end(), blockType);
        if (it != _palette.end()) {
            return std::distance(_palette.begin(), it);
        }
        _palette.push_back(blockType);
        return _palette.size() - 1;
    }

    std::vector<BlockTypeEnum> _palette = { BlockTypeEnum::Air };
    std::array<unsigned char, CHUNK_SIZE> _blocks = { 0 };
    bool _isFullyAir = true;
};

struct VertexData
{
    glm::vec3 position;
    glm::vec4 color;
    glm::vec2 uv;
    glm::vec3 normal;
};

class ChunkMesh 
{
public:
    ChunkMesh() 
        : _updateMesh(false)
    {
        VertexBufferLayout layout;
        layout.Push(3, GL_FLOAT); // Position
        layout.Push(4, GL_FLOAT); // Color
        layout.Push(2, GL_FLOAT); // UVs
        layout.Push(3, GL_FLOAT); // Normals

        _vao = std::make_shared<VertexArray>();
        _vbo = std::make_shared<VertexBuffer>(0);
        _ibo = std::make_shared<IndexBuffer>(0);

        _vao->SetVertexBuffer(_vbo.get(), layout);
        _vao->SetIndexBuffer(_ibo.get());
    }

    void AddFace(BlockFace face, const BlockType* blockType, int x, int y, int z)
    {
        const glm::vec4& color = blockType->color();

        glm::vec3 normal;

        switch (face)
        {
        case BlockFace::Top:
        {
            normal = { 0.0f, 1.0f, 0.0f };

            vertices.push_back({ {x + 1, y + 1, z}, color, {1.0f, 0.0f}, normal });
            vertices.push_back({ {x, y + 1, z}, color, {0.0f, 0.0f}, normal });
            vertices.push_back({ {x, y + 1, z + 1}, color, {0.0f, 1.0f}, normal });
            vertices.push_back({ {x + 1, y + 1, z + 1}, color, {1.0f, 1.0f}, normal });

            indices.push_back(currentVertexIndex + 0);
            indices.push_back(currentVertexIndex + 1);
            indices.push_back(currentVertexIndex + 2);
            indices.push_back(currentVertexIndex + 0);
            indices.push_back(currentVertexIndex + 2);
            indices.push_back(currentVertexIndex + 3);
            break;
        }
        case BlockFace::Bottom: {
            normal = { 0.0f, -1.0f, 0.0f };

            vertices.push_back({ {x, y, z}, color, {0.0f, 1.0f}, normal });
            vertices.push_back({ {x + 1, y, z}, color, {1.0f, 1.0f}, normal });
            vertices.push_back({ {x + 1, y, z + 1}, color, {1.0f, 0.0f}, normal });
            vertices.push_back({ {x, y, z + 1}, color, {0.0f, 0.0f}, normal });

            indices.push_back(currentVertexIndex + 0);
            indices.push_back(currentVertexIndex + 1);
            indices.push_back(currentVertexIndex + 3);
            indices.push_back(currentVertexIndex + 1);
            indices.push_back(currentVertexIndex + 2);
            indices.push_back(currentVertexIndex + 3);
            break;
        }

        case BlockFace::Front: {
            normal = { 0.0f, 0.0f, 1.0f };

            vertices.push_back({ {x, y, z + 1}, color, {0.0f, 0.0f}, normal });
            vertices.push_back({ {x + 1, y, z + 1}, color, {1.0f, 0.0f}, normal });
            vertices.push_back({ {x, y + 1, z + 1}, color, {0.0f, 1.0f}, normal });
            vertices.push_back({ {x + 1, y + 1, z + 1}, color, {1.0f, 1.0f}, normal });

            indices.push_back(currentVertexIndex + 0);
            indices.push_back(currentVertexIndex + 1);
            indices.push_back(currentVertexIndex + 2);
            indices.push_back(currentVertexIndex + 1);
            indices.push_back(currentVertexIndex + 3);
            indices.push_back(currentVertexIndex + 2);
            break;
        }

        case BlockFace::Back: {
            normal = { 0.0f, 0.0f, -1.0f };

            vertices.push_back({ {x, y, z}, color, {1.0f, 0.0f}, normal });
            vertices.push_back({ {x + 1, y, z}, color, {0.0f, 0.0f}, normal });
            vertices.push_back({ {x, y + 1, z}, color, {1.0f, 1.0f}, normal });
            vertices.push_back({ {x + 1, y + 1, z}, color, {0.0f, 1.0f}, normal });

            indices.push_back(currentVertexIndex + 0);
            indices.push_back(currentVertexIndex + 2);
            indices.push_back(currentVertexIndex + 1);
            indices.push_back(currentVertexIndex + 1);
            indices.push_back(currentVertexIndex + 2);
            indices.push_back(currentVertexIndex + 3);
            break;
        }


        case BlockFace::Left: {
            normal = { -1.0f, 0.0f, 0.0f };

            vertices.push_back({ {x, y, z}, color, {0.0f, 0.0f}, normal });
            vertices.push_back({ {x, y, z + 1}, color, {1.0f, 0.0f}, normal });
            vertices.push_back({ {x, y + 1, z}, color, {0.0f, 1.0f}, normal });
            vertices.push_back({ {x, y + 1, z + 1}, color, {1.0f, 1.0f}, normal });

            indices.push_back(currentVertexIndex + 0);
            indices.push_back(currentVertexIndex + 1);
            indices.push_back(currentVertexIndex + 2);
            indices.push_back(currentVertexIndex + 1);
            indices.push_back(currentVertexIndex + 3);
            indices.push_back(currentVertexIndex + 2);
            break;
        }

        case BlockFace::Right: {
            normal = { 1.0f, 0.0f, 0.0f };

            vertices.push_back({ {x + 1, y, z}, color, {1.0f, 0.0f}, normal });
            vertices.push_back({ {x + 1, y, z + 1}, color, {0.0f, 0.0f}, normal });
            vertices.push_back({ {x + 1, y + 1, z}, color, {1.0f, 1.0f}, normal });
            vertices.push_back({ {x + 1, y + 1, z + 1}, color, {0.0f, 1.0f}, normal });

            indices.push_back(currentVertexIndex + 0);
            indices.push_back(currentVertexIndex + 2);
            indices.push_back(currentVertexIndex + 1);
            indices.push_back(currentVertexIndex + 1);
            indices.push_back(currentVertexIndex + 2);
            indices.push_back(currentVertexIndex + 3);
            break;
        }
        }

        currentVertexIndex += 4;
    }

    void SetUpdateMesh(bool updateMesh) 
    {
        std::lock_guard lock(_meshMutex);
        _updateMesh = updateMesh;
    }

    void Render()
    {
        {
            std::lock_guard lock(_meshMutex);
            if (_updateMesh) {
                _vao->Bind();
                _vbo->SetData(0, vertices.size() * sizeof(VertexData), vertices.data());
                _ibo->SetData(0, indices.size(), indices.data());

                vertices.clear();
                indices.clear();
                currentVertexIndex = 0;

                _updateMesh = false;
            }
        }

        _vao->DrawIndexed();
    }

private:
    std::mutex _meshMutex;

    std::shared_ptr<VertexArray> _vao;
    std::shared_ptr<VertexBuffer> _vbo;
    std::shared_ptr<IndexBuffer> _ibo;

    std::vector<VertexData> vertices;
    std::vector<unsigned int> indices;
    int currentVertexIndex = 0;

    bool _updateMesh;
};

class World;
class Chunk
{
public:
    Chunk(World* world, const glm::ivec2& pos);
    ~Chunk();

    ChunkSegment* GetChunkSegment(int index);

    void MarkNeighboursDirty();
    void Render();

    glm::ivec2 pos() const;
    ChunkMesh* mesh() const;

    bool generated() const;
    bool building() const;
    bool dirty() const;

    void SetGenerated(bool generated);
    void SetBuilding(bool building);
    void MakeDirty(bool dirty);

private:
    ChunkMesh* _mesh;

    int _x;
    int _z;

    ChunkSegment* _chunkSegments[16];

    bool _generated;
    bool _dirty;
    bool _building;
    bool _updateMesh;

    World* _world;
};

#endif // !CHUNK_H
