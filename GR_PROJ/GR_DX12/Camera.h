#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

class Camera
{
public:
	Camera() = default;
	Camera(const Camera&) = delete;
	Camera(Camera&&) = delete;
	Camera& operator=(const Camera&) = delete;

	void LookAtLH(glm::vec3 eye, glm::vec3 center, glm::vec3 up);
	void PerspectiveLH(float fov, float aspect, float nearPlane,float farPlane);
	void OrthoLH(float left, float right, float bottom, float top, float _near, float _far);

	glm::mat4 GetViewMatrix() const;
	glm::mat4 GetPerspectiveMatrix(bool useOrtho = false) const;
	
	void Update();

private:

	glm::mat4 GetLocal2WorldMatrix() const;

	glm::vec3 m_Position;
	glm::quat m_Rotation;

	float m_Fov;
	float m_Aspect;
	float m_NearPlane;
	float m_FarPlane;

	float left, right, top, bottom, _near, _far;
};

