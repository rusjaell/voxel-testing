#ifndef INDEX_BUFFER
#define INDEX_BUFFER

#include "OpenGL.h"

class IndexBuffer
{
public:
    IndexBuffer(GLsizeiptr count);
    IndexBuffer(const GLuint* indices, GLsizeiptr count);
    ~IndexBuffer();

    void Bind() const;
    void Unbind() const;

    GLsizei count() const;

    void SetData(GLintptr offset, GLsizeiptr count, const void* data);

private:
    GLuint _ibo;
    GLsizei _count;
};

#endif