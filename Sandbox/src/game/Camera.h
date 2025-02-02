#ifndef CAMERA_H
#define CAMERA_H

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

    glm::mat4 viewMatrix() const;
    glm::mat4 projectionMatrix() const;

    glm::vec3 GetPosition() const;
    glm::vec3 GetDirection() const;

    float fov() const;
    float nearPlane() const;
    float farPlane() const;
    float aspectRatio() const;

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
 