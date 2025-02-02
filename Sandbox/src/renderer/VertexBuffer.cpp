#include "VertexBuffer.h"
#include <iostream>

VertexBufferLayout::VertexBufferLayout()
    : _stride(0)
{
}

GLsizei VertexBufferLayout::stride() const {
    return _stride;
}

const std::vector<VertexBufferElement>& VertexBufferLayout::elements() const {
    return _elements;
}

void VertexBufferLayout::Push(GLint size, GLenum type, GLboolean normalized)
{
    GLuint typeSize = 0;

    switch (type) {
    case GL_FLOAT:                typeSize = sizeof(GLfloat); break;
    case GL_UNSIGNED_INT:         typeSize = sizeof(GLuint); break;
    case GL_UNSIGNED_BYTE:        typeSize = sizeof(GLubyte); break;
    case GL_SHORT:                typeSize = sizeof(GLshort); break;
    case GL_HALF_FLOAT:           typeSize = sizeof(GLhalf); break;
    case GL_INT_2_10_10_10_REV: 
        typeSize = sizeof(GLuint);
        size = 1;
        break;
    default:
        std::cerr << "Unsupported type!" << std::endl;
        return;
    }

    _elements.emplace_back(size, type, normalized);
    _stride += size * typeSize;
}

VertexBuffer::VertexBuffer(GLsizeiptr size)
    : _size(size)
{
    GL_CALL(glGenBuffers(1, &_vbo));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, _vbo));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW));
}

VertexBuffer::VertexBuffer(const void* data, GLuint size)
    : _size(size)
{
    GL_CALL(glGenBuffers(1, &_vbo));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, _vbo));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

void VertexBuffer::SetData(GLintptr offset, GLsizeiptr size, const void* data)
{
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, _vbo));

    if (size > _size) {
        GL_CALL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
        _size = size;
        return;
    }
    _size = size;

    GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, offset, size, data));
}

VertexBuffer::~VertexBuffer()
{
    GL_CALL(glDeleteBuffers(1, &_vbo));
}

void VertexBuffer::Bind() const
{
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, _vbo));
}

void VertexBuffer::Unbind() const
{
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
