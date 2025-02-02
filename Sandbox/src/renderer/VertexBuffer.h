#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

struct VertexBufferElement
{
    GLint size;
    GLenum type;
    GLboolean normalized;
};

class VertexBufferLayout 
{
public:
    VertexBufferLayout();

    const std::vector<VertexBufferElement>& elements() const;
    GLsizei stride() const;

    void Push(GLint size, GLenum type, GLboolean normalized = false);

private:
    std::vector<VertexBufferElement> _elements; 
    GLsizei _stride;  
};

class VertexBuffer
{
public:
    VertexBuffer(GLsizeiptr size);
    VertexBuffer(const void* data, GLuint size);
    ~VertexBuffer();

    void Bind() const;
    void Unbind() const;

    void SetData(GLintptr offset, GLsizeiptr size, const void* data);

private:
    GLuint _vbo; 
    GLsizeiptr _size;
};

#endif