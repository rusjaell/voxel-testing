#ifndef CHUNK_H
#define CHUNK_H

#include "ChunkMesh.h"
#include "ChunkSegment.h"

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
