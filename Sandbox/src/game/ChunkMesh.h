#ifndef CHUNK_MESH_H
#define CHUNK_MESH_H

#include "BlockProperties.h"
#include "renderer/VertexArray.h"
#include "renderer/VertexBuffer.h"
#include "renderer/IndexBuffer.h"

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
    ChunkMesh();

    void AddFace(BlockFace face, const BlockProperties* blockType, int x, int y, int z);
    void SetUpdateMesh(bool updateMesh);
    void Render();

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

#endif	