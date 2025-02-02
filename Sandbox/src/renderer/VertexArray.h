#ifndef VERTEX_ARRAY
#define VERTEX_ARRAY

#include "OpenGL.h"

class VertexBuffer;
class VertexBufferLayout;
class IndexBuffer;
class VertexArray
{
public:
    VertexArray();
    ~VertexArray();

    void Bind() const;
    void Unbind() const;

    void SetVertexBuffer(VertexBuffer* vbo, const VertexBufferLayout& layout);
    void SetIndexBuffer(IndexBuffer* ibo);

    void DrawIndexed();

    const IndexBuffer* indexBuffer() const;
    const VertexBuffer* vertexBuffer() const;

private:
    GLuint _vao;
    VertexBuffer* _vbo;
    IndexBuffer* _ibo;
};

#endif