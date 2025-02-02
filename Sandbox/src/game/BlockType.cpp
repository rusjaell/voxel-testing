#include "BlockType.h"

BlockType::BlockType(BlockTypeEnum type, const glm::vec4& color)
	: _type(type), _color(color)
{
}

const BlockTypeEnum BlockType::type() const
{
	return _type;
}

const glm::vec4& BlockType::color() const
{
	return _color;
}