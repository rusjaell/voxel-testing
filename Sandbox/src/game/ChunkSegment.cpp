#include "pch.h"
#include "ChunkSegment.h"

void ChunkSegment::SetBlockType(int x, int y, int z, BlockTypeEnum blockType) {
    int blockIndex = GetBlockIndex(x, y, z);
    int paletteIndex = GetOrAddPalette(blockType);
    _blocks[blockIndex] = paletteIndex;
    if (blockType != BlockTypeEnum::Air) {
        _isFullyAir = false;
    }
}

BlockTypeEnum ChunkSegment::GetBlockType(int x, int y, int z) {
    int blockIndex = GetBlockIndex(x, y, z);
    if (blockIndex == -1) {
        return BlockTypeEnum::Air;
    }
    int paletteIndex = _blocks[blockIndex];
    return _palette[paletteIndex];
}

bool ChunkSegment::IsFullyAir() const {
    return _isFullyAir;
}

int ChunkSegment::GetBlockIndex(int x, int y, int z) {
    if (x < 0 || x >= CHUNK_WIDTH || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_DEPTH) {
        return -1;
    }
    return x + y * CHUNK_WIDTH + z * CHUNK_WIDTH * CHUNK_HEIGHT;
}

int ChunkSegment::GetOrAddPalette(BlockTypeEnum blockType)
{
    std::vector<BlockTypeEnum>::iterator it = std::find(_palette.begin(), _palette.end(), blockType);
    if (it != _palette.end()) {
        return std::distance(_palette.begin(), it);
    }
    _palette.push_back(blockType);
    return _palette.size() - 1;
}