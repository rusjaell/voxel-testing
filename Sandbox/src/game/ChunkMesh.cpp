#include "pch.h"
#include "ChunkMesh.h"

ChunkMesh::ChunkMesh()
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

void ChunkMesh::AddFace(BlockFace face, const BlockProperties* blockType, int x, int y, int z)
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

void ChunkMesh::SetUpdateMesh(bool updateMesh)
{
    std::lock_guard lock(_meshMutex);
    _updateMesh = updateMesh;
}

void ChunkMesh::Render()
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