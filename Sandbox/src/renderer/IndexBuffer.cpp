#include "pch.h"
#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(GLsizeiptr count)
    : _count(count)
{
    GL_CALL(glGenBuffers(1, &_ibo));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo));
    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW));
}

IndexBuffer::IndexBuffer(const GLuint* indices, GLsizeiptr count)
    : _count(count)
{
    GL_CALL(glGenBuffers(1, &_ibo));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo));
    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), indices, GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer()
{
    GL_CALL(glDeleteBuffers(1, &_ibo));
}

void IndexBuffer::Bind() const
{
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo));
}

void IndexBuffer::Unbind() const
{
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

}

GLsizei IndexBuffer::count() const
{
    return _count; 
}

void IndexBuffer::SetData(GLintptr offset, GLsizeiptr count, const void* data)
{
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo));

    if (count > _count) {
        GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), data, GL_DYNAMIC_DRAW));
        _count = count;
        return;
    }

    _count = count;
    GL_CALL(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, count * sizeof(GLuint), data));
}