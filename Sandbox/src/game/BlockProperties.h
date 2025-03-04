#ifndef BLOCK_PROPERTIES_H
#define BLOCK_PROPERTIES_H

enum BlockTypeEnum : unsigned char
{
	Air = 0,
	Grass = 1,
	Dirt = 2,
	Stone = 3
};

enum class BlockFace
{
	Top = 0,
	Bottom,
	Front,
	Back,
	Left,
	Right
};

class BlockProperties 
{
public:
	BlockProperties(BlockTypeEnum type, const glm::vec4& color);

	const BlockTypeEnum type() const;
	const glm::vec4& color() const;

private:
	BlockTypeEnum _type;
	glm::vec4 _color;
};

#endif