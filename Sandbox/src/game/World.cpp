#include "World.h"
#include <random> 
#include <iostream>
#include <threading/CommandQueue.h>
#include <atomic>
#include "ChunkBuilder.h"

World::World()
{
    _shader = new Shader("resources/shader.vert", "resources/shader.frag");
    _texture = new Texture("resources/textures/test.png");

	constexpr float fov = 60.0f;
	constexpr float aspectRatio = 1280.0f / 720.0f;

	_camera = new Camera(fov, aspectRatio, 0.1f, 100000.0f);
    _camera->SetPosition(glm::vec3(0, 0, 0));
    
    _chunkBuilder = new ChunkBuilder(this);
}

World::~World()
{
	delete _camera;
    delete _shader;
    delete _chunkBuilder;
}

void World::OnKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_W) {
        _camera->keys[GLFW_KEY_W] = action != GLFW_RELEASE;
    }

    if (key == GLFW_KEY_S) {
        _camera->keys[GLFW_KEY_S] = action != GLFW_RELEASE;
    }

    if (key == GLFW_KEY_A) {
        _camera->keys[GLFW_KEY_A] = action != GLFW_RELEASE;
    }

    if (key == GLFW_KEY_D) {
        _camera->keys[GLFW_KEY_D] = action != GLFW_RELEASE;
    }

    if (key == GLFW_KEY_SPACE) {
        _camera->keys[GLFW_KEY_SPACE] = action != GLFW_RELEASE;
    }

    if (key == GLFW_KEY_LEFT_SHIFT) {
        _camera->keys[GLFW_KEY_LEFT_SHIFT] = action != GLFW_RELEASE;
    }

    if (key == GLFW_KEY_F1) {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
    }

    if (key == GLFW_KEY_F2) {
        glDisable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
    }
}

void World::OnMouseMoveEvent(float xpos, float ypos)
{
    static float lastX = 1280.0f / 2.0f;
    static float lastY = 720.0f / 2.0f;

    float xOffset = xpos - lastX;
    float yOffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    _camera->ProcessMouseMovement(xOffset, yOffset);
}

void World::OnResizeEvent(int width, int height)
{
	_camera->SetAspectRatio(width / (double)height);
}

void World::Update(double dt)
{
    _camera->ProcessKeyboardInput(dt);

    LoadChunksAroundPlayer();
    UnloadFarChunks();

    _chunkBuilder->Process();
}

void World::Render(double dt)
{
    _shader->Use();
    _shader->SetUniform1i("u_Texture", 0);
    _shader->SetUniformMatrix4f("u_View", _camera->viewMatrix());
    _shader->SetUniformMatrix4f("u_Projection", _camera->projectionMatrix());
    _shader->SetUniform3f("u_LightDirection", { -1.0f, -1.0f, 0.25f});

    _texture->Bind();

    std::shared_lock lock(_chunksMutex);
    for (auto& [key, chunk] : _chunks) 
    {
        if (!chunk->generated()) {
            continue;
        }

        if (chunk->dirty())
        {
            chunk->MakeDirty(false);
            _chunkBuilder->PushRebuildChunkMesh(chunk);
            continue;
        }

        const glm::ivec2 pos = chunk->pos();
        glm::vec3 chunkPos = { pos.x * CHUNK_WIDTH, 0.0f, pos.y * CHUNK_DEPTH };

        glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), chunkPos);

        _shader->SetUniformMatrix4f("u_Model", translationMatrix);

        chunk->Render();
    }
}

std::shared_ptr<Chunk> World::GetChunk(int chunkX, int chunkZ)
{
    const glm::ivec2 chunkPos = { chunkX, chunkZ };

    std::shared_lock lock(_chunksMutex);

    auto it = _chunks.find(chunkPos);
    if (it != _chunks.end()) {
        return it->second;
    }
    return nullptr;
}

BlockTypeEnum World::GetBlockType(int worldX, int worldY, int worldZ)
{
    const glm::ivec2 chunkPos = WorldToChunk(worldX, worldZ);

    const std::shared_ptr<Chunk>& chunk = GetChunk(chunkPos.x, chunkPos.y);
    if (chunk == nullptr) {
        return BlockTypeEnum::Air;
    }

    const glm::ivec3 local = WorldToLocal(worldX, worldY, worldZ);

    int segmentIndex = worldY / 16;

    ChunkSegment* segment = chunk->GetChunkSegment(segmentIndex);
    return segment->GetBlockType(local.x, local.y, local.z);
}

void World::LoadChunksAroundPlayer()
{
    glm::ivec2 playerChunkPos = WorldToChunk((int)_camera->GetPosition().x, (int)_camera->GetPosition().z);

    for (int x = -renderDistance; x <= renderDistance; ++x) {
        for (int z = -renderDistance; z <= renderDistance; ++z) {
            glm::ivec2 chunkPos = playerChunkPos + glm::ivec2(x, z);

            const std::shared_ptr<Chunk>& chunk = GetChunk(chunkPos.x, chunkPos.y);
            if (chunk != nullptr) {
                continue;
            }

            const std::shared_ptr<Chunk>& newChunk = std::make_shared<Chunk>(this, chunkPos);
            {
                std::unique_lock lock(_chunksMutex);
                _chunks.insert(std::make_pair(chunkPos, newChunk));
            }

            _chunkBuilder->PushBuildChunk(newChunk);
        }
    }
}

void World::UnloadFarChunks()
{
    return;

    std::unique_lock lock(_chunksMutex);

    glm::ivec2 playerChunkPos = WorldToChunk((int)_camera->GetPosition().x, (int)_camera->GetPosition().z);

    for (auto it = _chunks.begin(); it != _chunks.end();)
    {
        const std::shared_ptr<Chunk>& chunk = it->second;

        if (chunk->dirty() || chunk->building() || !chunk->generated()) {
            ++it;
            continue;
        }

        glm::ivec2 chunkPos = chunk->pos();
        float distance = glm::distance(glm::vec2(playerChunkPos), glm::vec2(chunkPos));

        if (distance < renderDistance + 4) {
            ++it;
            continue;
        }

        it = _chunks.erase(it);
    }
}
