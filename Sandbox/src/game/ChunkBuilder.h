#ifndef CHUNK_BUILDER_H
#define CHUNK_BUILDER_H

#include <queue>
#include <GLM/ext/vector_int2.hpp>
#include <memory>
#include <unordered_map>

#include "World.h"

class ChunkBuilder 
{
public:
	ChunkBuilder(World* world);
	~ChunkBuilder() = default;

	void PushBuildChunk(const std::shared_ptr<Chunk>& chunk);
	void PushRebuildChunkMesh(const std::shared_ptr<Chunk>& chunk);

	void Process();

	void BuildChunk(const std::shared_ptr<Chunk>& chunk);
	void RebuildMesh(const std::shared_ptr<Chunk>& chunk);

private:
	World* _world;
	ThreadPool _threadPool;
	
	std::mutex _lock;
	std::queue<std::shared_ptr<Chunk>> _pendingData;
	std::queue<std::shared_ptr<Chunk>> _pendingMesh;

	std::unordered_map<glm::ivec2, std::future<void>> _tasks;

	std::atomic<int> _activeBuilderTasks { 0 };
	std::atomic<int> _activeMeshTasks{ 0 };
};

constexpr int MAX_BUILDER_TASKS = 32;
constexpr int MAX_MESH_TASKS = 16;

#endif