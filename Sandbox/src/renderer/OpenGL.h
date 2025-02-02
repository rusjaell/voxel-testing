#ifndef OPENGL_H
#define OPENGL_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>

#if defined(_DEBUG) || defined(DEBUG) || !defined(NDEBUG)


static const char* GLErrorToString(GLenum error)
{
    switch (error)
    {
        case GL_NO_ERROR: return "GL_NO_ERROR";
        case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
        case GL_STACK_OVERFLOW: return "GL_STACK_OVERFLOW";
        case GL_STACK_UNDERFLOW: return "GL_STACK_UNDERFLOW";
        case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
        case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
        case GL_CONTEXT_LOST: return "GL_CONTEXT_LOST";
    }
    return "Unknown OpenGL error";
}

#define GL_CALL(x) \
        do { \
            x; \
            GLenum err; \
            while ((err = glGetError()) != GL_NO_ERROR) { \
                fprintf(stderr, "[OpenGL Error] (%s-%d): %s in %s:%d\n", GLErrorToString(err), err, #x, __FILE__, __LINE__); \
                __debugbreak(); \
            } \
        } while (0)
#else
#define GL_CALL(x) x
#endif

#endif