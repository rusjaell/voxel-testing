#include "pch.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

VertexArray::VertexArray()
{
    GL_CALL(glGenVertexArrays(1, &_vao));
    GL_CALL(glBindVertexArray(_vao));
}

VertexArray::~VertexArray()
{
    GL_CALL(glDeleteVertexArrays(1, &_vao));
}

void VertexArray::Bind() const
{
    GL_CALL(glBindVertexArray(_vao));
}

void VertexArray::Unbind() const
{
    GL_CALL(glBindVertexArray(0));
}

void VertexArray::SetVertexBuffer(VertexBuffer* vbo, const VertexBufferLayout& layout)
{
    Bind();
    vbo->Bind();

    GLsizei offset = 0;
    GLuint index = 0;
    for (const VertexBufferElement& element : layout.elements())
    {
        GL_CALL(glVertexAttribPointer(index, element.size, element.type, element.normalized, layout.stride(), (const void*)offset));
        GL_CALL(glEnableVertexAttribArray(index));

        offset += element.size * sizeof(element.type);
        index++;
    }

    _vbo = vbo;
}

void VertexArray::SetIndexBuffer(IndexBuffer* ibo)
{
    Bind();
    ibo->Bind();

    _ibo = ibo;
}

void VertexArray::DrawIndexed()
{
    Bind();
    int count = _ibo->count();
    GL_CALL(glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr));
}

const IndexBuffer* VertexArray::indexBuffer() const
{
    return _ibo;
}

const VertexBuffer* VertexArray::vertexBuffer() const
{
    return _vbo;
}
