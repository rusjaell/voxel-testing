#include "pch.h"
#include "BlockProperties.h"

BlockProperties::BlockProperties(BlockTypeEnum type, const glm::vec4& color)
	: _type(type), _color(color)
{
}

const BlockTypeEnum BlockProperties::type() const
{
	return _type;
}

const glm::vec4& BlockProperties::color() const
{
	return _color;
}