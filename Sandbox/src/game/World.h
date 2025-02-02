#ifndef WORLD_H
#define WORLD_H

#include "Chunk.h"
#include "Camera.h"
#include "renderer/Shader.h"
#include "renderer/Texture.h"
#include "threading/ThreadPool.h"

class ChunkBuilder;
class World
{
public:
	World();
	~World();

	void OnKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
	void OnMouseMoveEvent(float xpos, float ypos);
	void OnResizeEvent(int width, int height);

	void Update(double dt);
	void Render(double dt);

	std::shared_ptr<Chunk> GetChunk(int chunkX, int chunkZ);

	BlockTypeEnum GetBlockType(int worldX, int worldY, int worldZ);

	void LoadChunksAroundPlayer();
	void UnloadFarChunks();

private:
	std::shared_mutex _chunksMutex;
	std::unordered_map<glm::ivec2, std::shared_ptr<Chunk>> _chunks;

	Camera* _camera;
	Shader* _shader;
	Texture* _texture;

	const int renderDistance = 7;

	ChunkBuilder* _chunkBuilder;

public:
	static glm::ivec2 ChunkToWorld(int x, int z)
	{
		int worldX = x * CHUNK_WIDTH;
		int worldZ = z * CHUNK_DEPTH;
		return { worldX, worldZ };
	}
	
	static glm::ivec2 WorldToChunk(int x, int z) 
	{
		int chunkX = (x < 0) ? (x - (CHUNK_WIDTH - 1)) / CHUNK_WIDTH : x / CHUNK_WIDTH;
		int chunkZ = (z < 0) ? (z - (CHUNK_DEPTH - 1)) / CHUNK_DEPTH : z / CHUNK_DEPTH;
		return { chunkX, chunkZ };
	}

	static glm::ivec3 WorldToLocal(int x, int y, int z)
	{
		int localX = (x < 0) ? (x % CHUNK_WIDTH + CHUNK_WIDTH) % CHUNK_WIDTH : x % CHUNK_WIDTH;
		int localY = y % CHUNK_HEIGHT;
		int localZ = (z < 0) ? (z % CHUNK_DEPTH + CHUNK_DEPTH) % CHUNK_DEPTH : z % CHUNK_DEPTH;
		return { localX, localY, localZ };
	}
};

#endif // !WORLD_H
