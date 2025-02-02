#include "game/Chunk.h"

#include "renderer/VertexArray.h"
#include "renderer/VertexBuffer.h"
#include "renderer/IndexBuffer.h"
#include <renderer/Shader.h>

#include "game/Camera.h"

#include "game/World.h"
#include <iostream>

#include "threading/CommandQueue.h"

int main()
{
    try 
    {
        glfwSetErrorCallback([](int error, const char* description)
        {
             fprintf(stderr, "[GLFW Error] %s\n", description);
        });

        if (!glfwInit()) {
            return -1;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        GLFWwindow* window = glfwCreateWindow(1280, 720, "Sandbox", NULL, NULL);
        if (window == nullptr)
        {
            glfwTerminate();
            return -2;
        }

        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            glfwTerminate();
            return -3;
        }

        static World* world = new World();

        glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            world->OnKeyEvent(window, key, scancode, action, mods);
        });

        glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos)
        {
            world->OnMouseMoveEvent(xpos, ypos);
        });

        glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, int width, int height)
        {
            GL_CALL(glViewport(0, 0, width, height));
        });

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        GL_CALL(glEnable(GL_DEBUG_OUTPUT));
        GL_CALL(glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
        {
            if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) {
                //fprintf(stderr, "[OpenGL Debug] Notification Source: %u, Type: %u, ID: %u, Severity: %u, Message: %s\n", source, type, id, severity, message);
                return;
            }

            fprintf(stderr, "[OpenGL Debug] Source: %u, Type: %u, ID: %u, Severity: %u, Message: %s\n", source, type, id, severity, message);
        }, nullptr));

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  // Use source alpha for blending

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);

        bool vsync = false;

        int maximumFPS = 0;

        double fixedTimeStep = 1.0 / 60.0;
        double maxRenderTimeStep = maximumFPS == 0 ? 0 : 1.0 / maximumFPS;

        double lastFrame = 0.0;
        double timeAccumulator = 0.0;
        double renderAccumulator = 0.0f;

        int updates = 0;
        int frames = 0;
        double timer = 0.0;

        glfwSwapInterval(vsync ? 1 : 0);

        while (!glfwWindowShouldClose(window))
        {
            double currentTime = glfwGetTime();
            double deltaTime = currentTime - lastFrame;
            lastFrame = currentTime;

            timer += deltaTime;
            timeAccumulator += deltaTime;
            renderAccumulator += deltaTime;

            CommandQueue::Process();

            //world->Update(deltaTime);
            while (timeAccumulator >= fixedTimeStep)
            {
                timeAccumulator -= fixedTimeStep;
                updates++;

                //world->FixedUpdate(fixedTimeStep);
                world->Update(fixedTimeStep);
            }

            float alpha = timeAccumulator / fixedTimeStep;

            if (vsync || renderAccumulator >= maxRenderTimeStep)
            {
                renderAccumulator = 0.0;
                frames++;

                GL_CALL(glClearColor(0.392f, 0.584f, 0.929f, 1.0f));
                GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

                world->Render(alpha);

                glfwSwapBuffers(window);
            }

            glfwPollEvents();

            if (timer >= 1.0)
            {
                std::cout << "UPS: " << updates << " | FPS: " << frames << " | ALPHA: " << alpha << std::endl;
                updates = 0;
                frames = 0; 
                timer = 0.0;
            }
        }

        delete world;

        glfwDestroyWindow(window);
        glfwTerminate();
    }
    catch (const std::exception& e) 
    {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }
    return 0;
}