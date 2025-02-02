#ifndef CHUNK_SEGMENT_H
#define CHUNK_SEGMENT_H

#include "BlockProperties.h"

class ChunkSegment
{
public:
    void SetBlockType(int x, int y, int z, BlockTypeEnum blockType);

    BlockTypeEnum GetBlockType(int x, int y, int z);

    bool IsFullyAir() const;

private:
    int GetBlockIndex(int x, int y, int z);
    int GetOrAddPalette(BlockTypeEnum blockType);

    std::vector<BlockTypeEnum> _palette = { BlockTypeEnum::Air };
    std::array<unsigned char, CHUNK_SIZE_3> _blocks = { 0 };

    bool _isFullyAir = true;
};


#endif