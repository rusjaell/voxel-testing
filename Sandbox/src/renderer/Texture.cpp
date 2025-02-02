#include "pch.h"
#include "Texture.h"

Texture::Texture(const std::string& filePath)
	: id_(0), width_(0), height_(0), channels_(0)
{
	stbi_set_flip_vertically_on_load(1);

	stbi_uc* data = stbi_load(filePath.c_str(), &width_, &height_, &channels_, 0);
	if (data == NULL)
	{
		std::cout << "Failed to load texture creating 1, 1 default " << filePath.c_str() << "\n";

		width_ = 1;
		height_ = 1;

		internalformat_ = GL_RGBA8;
		dataFormat_ = GL_RGBA;

		glCreateTextures(GL_TEXTURE_2D, 1, &id_);
		glTextureStorage2D(id_, 1, internalformat_, width_, height_);

		glTextureParameteri(id_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(id_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(id_, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(id_, GL_TEXTURE_WRAP_T, GL_REPEAT);

		unsigned int data = 0xFFFFFFFF;
		glTextureSubImage2D(id_, 0, 0, 0, width_, height_, GL_RGBA, GL_UNSIGNED_BYTE, &data);

		return;
	}

	GLenum internalFormat = 0, dataFormat = 0;
	if (channels_ == 4)
	{
		internalFormat = GL_RGBA8;
		dataFormat = GL_RGBA;
	}
	else if (channels_ == 3)
	{
		internalFormat = GL_RGB8;
		dataFormat = GL_RGB;
	}

	internalformat_ = internalFormat;
	dataFormat_ = dataFormat;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glCreateTextures(GL_TEXTURE_2D, 1, &id_);
	glTextureStorage2D(id_, 1, internalFormat, width_, height_);

	glTextureParameteri(id_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(id_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTextureParameteri(id_, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(id_, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTextureSubImage2D(id_, 0, 0, 0, width_, height_, dataFormat, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);
}

Texture::Texture(unsigned int width, unsigned int height)
	: id_(0), width_(width), height_(height), channels_(0)
{
	internalformat_ = GL_RGBA8;
	dataFormat_ = GL_RGBA;

	glCreateTextures(GL_TEXTURE_2D, 1, &id_);
	glTextureStorage2D(id_, 1, internalformat_, width_, height_);

	glTextureParameteri(id_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(id_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTextureParameteri(id_, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(id_, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

Texture::~Texture()
{
	glDeleteTextures(1, &id_);
}

void Texture::Bind(unsigned int slot) const
{
	glBindTextureUnit(slot, id_);
}

void Texture::Unbind(unsigned int slot) const
{
	glBindTextureUnit(slot, 0);
}

void Texture::SetDataRGBA(void* data, unsigned int size)
{
	assert(size == width_ * height_ * 4);// , "Data must be entire texture!");
	glTextureSubImage2D(id_, 0, 0, 0, width_, height_, GL_RGBA, GL_UNSIGNED_BYTE, data);
}

unsigned int Texture::id() const {
	return id_;
}

unsigned int Texture::width() const {
	return width_;
}

unsigned int Texture::height() const {
	return height_;
}

bool Texture::operator==(const Texture& other)
{
	return id_ == other.id_;
}