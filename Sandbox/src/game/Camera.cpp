#include "pch.h"
#include "Camera.h"

Camera::Camera(float fov, float aspectRatio, float nearPlane, float farPlane)
    : _fov(fov), _aspectRatio(aspectRatio), _nearPlane(nearPlane), _farPlane(farPlane)
{
    UpdateCameraVectors();
    UpdateViewMatrix();
    UpdateProjectionMatrix();
}

Camera::~Camera() {
}

void Camera::ProcessKeyboardInput(float deltaTime)
{
    float velocity = _speed * deltaTime;

    if (keys[GLFW_KEY_W])
        _pos += _forward * velocity;

    if (keys[GLFW_KEY_S])
        _pos -= _forward * velocity;

    if (keys[GLFW_KEY_A])
        _pos -= _right * velocity;

    if (keys[GLFW_KEY_D])
        _pos += _right * velocity;

    if (keys[GLFW_KEY_SPACE])
        _pos += glm::vec3(0, 1, 0) * velocity;

    if (keys[GLFW_KEY_LEFT_SHIFT])
        _pos -= glm::vec3(0, 1, 0) * velocity;

    UpdateViewMatrix();
}

void Camera::ProcessMouseMovement(float xOffset, float yOffset)
{
    xOffset *= _sensitivity;
    yOffset *= _sensitivity;

    _yaw += xOffset;
    _pitch += yOffset;

    if (_pitch > 89.0f)
        _pitch = 89.0f;
    if (_pitch < -89.0f)
        _pitch = -89.0f;

    UpdateCameraVectors();
}

void Camera::SetPosition(const glm::vec3& pos)
{
    _pos = pos;
    UpdateViewMatrix();
}

void Camera::SetFov(float fov)
{
    _fov = fov;
    UpdateProjectionMatrix();
}

void Camera::SetAspectRatio(float aspectRatio)
{
    _aspectRatio = aspectRatio;
    UpdateProjectionMatrix();
}

glm::mat4 Camera::viewMatrix() const {
    return _viewMatrix;
}

glm::mat4 Camera::projectionMatrix() const {
    return _projectionMatrix;
}

glm::vec3 Camera::GetPosition() const {
    return _pos;
}

glm::vec3 Camera::GetDirection() const {
    return _forward;
}

float Camera::fov() const {
    return _fov;
}

float Camera::nearPlane() const {
    return _nearPlane;
}

float Camera::farPlane() const {
    return _farPlane;
}

float Camera::aspectRatio() const {
    return _aspectRatio;
}


void Camera::UpdateViewMatrix()
{
    _viewMatrix = glm::lookAt(_pos, _pos + _forward, _up);
}

void Camera::UpdateProjectionMatrix()
{
    _projectionMatrix = glm::perspective(glm::radians(_fov), _aspectRatio, _nearPlane, _farPlane);
}

void Camera::UpdateCameraVectors()
{
    constexpr glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    float yaw = glm::radians(_yaw);
    float pitch = glm::radians(_pitch);

    // Calculate the front vector (the direction the camera is looking)
    _forward.x = cos(yaw) * cos(pitch);
    _forward.y = sin(pitch);
    _forward.z = sin(yaw) * cos(pitch);
    _forward = glm::normalize(_forward);

    // Recalculate the right and up vectors
    _right = glm::normalize(glm::cross(_forward, worldUp)); // Right vector (X-axis)
    _up = glm::normalize(glm::cross(_right, _forward));     // Up vector (Y-axis)
}