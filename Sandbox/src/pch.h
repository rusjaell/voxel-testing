#ifndef PCH_H
#define PCH_H

#include <random> 
#include <string>
#include <vector>
#include <array>
#include <chrono>
#include <unordered_map>
#include <algorithm>
#include <queue>
#include <thread>
#include <functional>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <future>
#include <iostream>
#include <atomic>
#include <cassert>
#include <fstream>
#include <sstream>

constexpr int CHUNK_SIZE = 16;
constexpr int CHUNK_SIZE_2 = CHUNK_SIZE * CHUNK_SIZE;
constexpr int CHUNK_SIZE_3 = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;

constexpr int CHUNK_SIZE_PADDED = CHUNK_SIZE + 2;
constexpr int CHUNK_SIZE_PADDED_2 = CHUNK_SIZE_PADDED * CHUNK_SIZE_PADDED;
constexpr int CHUNK_SIZE_PADDED_3 = CHUNK_SIZE_PADDED * CHUNK_SIZE_PADDED * CHUNK_SIZE_PADDED;

// STBI
#include <stb/stb_image.h>

// OpenGL

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLM/gtc/noise.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMEBNTAL
#include <glm/gtx/hash.hpp>

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