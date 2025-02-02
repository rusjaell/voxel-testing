#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

class Camera
{
public:
    Camera(float fov, float aspectRatio, float nearPlane, float farPlane);
    ~Camera();

    void ProcessKeyboardInput(float deltaTime);
    void ProcessMouseMovement(float xOffset, float yOffset);

    void SetPosition(const glm::vec3& pos);
    void SetFov(float fov);
    void SetAspectRatio(float aspectRatio);

    glm::mat4 viewMatrix() const {
        return _viewMatrix;
    }

    glm::mat4 projectionMatrix() const {
        return _projectionMatrix;
    }

    glm::vec3 GetPosition() const {
        return _pos;
    }

    glm::vec3 GetDirection() const {
        return _forward;
    }

    float fov() const {
        return _fov;
    }

    float nearPlane() const {
        return _nearPlane;
    }

    float farPlane() const {
        return _farPlane;
    }

    float aspectRatio() const {
        return _aspectRatio;
    }

    void UpdateViewMatrix();
    void UpdateProjectionMatrix();
    void UpdateCameraVectors();

private:
    glm::vec3 _pos = { 0, 0, 0 };
    glm::vec3 _forward = { 0, 0, -1 };
    glm::vec3 _up = { 0, 1, 0 };
    glm::vec3 _right = { 1, 0, 0 };

    float _nearPlane = 0.1f;
    float _farPlane = 300.0f;

    float _yaw = -90.0f;
    float _pitch = 0.0f;
    float _speed = 125.0f;
    float _sensitivity = 0.1f;
    float _fov = 60.0f;
    float _aspectRatio = 1.0f;

    glm::mat4 _viewMatrix;
    glm::mat4 _projectionMatrix;

public:
    bool keys[GLFW_KEY_LAST] = { false };
};


#endif // !CAMERA_H
 