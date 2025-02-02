#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <glad/glad.h>

class Texture
{
public:
	Texture(const std::string& filePath);
	Texture(unsigned int width, unsigned int height);
	~Texture();

	void Bind(unsigned int slot = 0) const;
	void Unbind(unsigned int slot = 0) const;

	void SetDataRGBA(void* data, unsigned int size);

	unsigned int id() const;
	unsigned int width() const;
	unsigned int height() const;

	bool operator==(const Texture& other);

private:
	unsigned int id_;

	int width_;
	int height_;
	int channels_;
	GLenum internalformat_;
	GLenum dataFormat_;
};

#endif // !TEXTURE_H
