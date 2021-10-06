#include "Camera.h"
#include "Input.h"
#include <iostream>


extern int g_iWidth;
extern int g_iHeight;

void Camera::LookAtLH(glm::vec3 eye, glm::vec3 center, glm::vec3 up)
{
	m_Position = eye;
	glm::vec3 forwardDir = center - eye;
	m_Rotation = glm::quatLookAtLH(glm::normalize(forwardDir), up);
}

void Camera::PerspectiveLH(float fov, float aspect, float nearPlane, float farPlane)
{
	m_Fov = fov;
	m_Aspect = aspect;
	m_NearPlane = nearPlane;
	m_FarPlane = farPlane;
}

void Camera::OrthoLH(float left, float right, float bottom, float top, float _near, float _far)
{
	this->left = left; this->right = right;
	this->top = top; this->bottom = bottom;
	this->_near = _near; this->_far = _far;
}

glm::mat4 Camera::GetViewMatrix() const
{
	return glm::inverse(GetLocal2WorldMatrix());
}

glm::mat4 Camera::GetPerspectiveMatrix(bool useOrtho) const
{
	glm::mat4 ProjMat;
	if (useOrtho)
		ProjMat = glm::orthoLH_ZO(left, right, bottom, top, _near, _far);
	else
		ProjMat = glm::perspectiveLH_ZO(m_Fov, m_Aspect, m_NearPlane, m_FarPlane);

	return ProjMat;
}

glm::mat4 Camera::GetLocal2WorldMatrix() const
{
	glm::mat4 local2world = glm::toMat4(m_Rotation);
	local2world[3].x = m_Position.x;
	local2world[3].y = m_Position.y;
	local2world[3].z = m_Position.z;

	std::cout << m_Position.x << "," << m_Position.y << "," << m_Position.z << std::endl;

	return local2world;
}

void Camera::Update()
{
	{
		static float2 lastMousePosition = Input::Instance->GetMousePosition();

		if (Input::Instance->GetMouseButton(0))
		{
			//对鼠标的移动 做绕y或者x旋转			
			float2 currentMousePosition = Input::Instance->GetMousePosition();
			float2 deltaMove = currentMousePosition - lastMousePosition;
			lastMousePosition = currentMousePosition;

			float xTheta = deltaMove.x / (g_iWidth * 1.0f) * 3.1415927f * 0.5f;
			float yTheta = deltaMove.y / (g_iHeight * 1.0f) * 3.1415927f * 0.5f;
				
			m_Rotation = glm::quat(glm::vec3(yTheta, xTheta, 0.0f)) * m_Rotation;			
			std::cout << m_Rotation.x << "		" << m_Rotation.y << "		" << m_Rotation.z <<"		"<< m_Rotation.w << std::endl;
		}
		else
		{
			lastMousePosition = Input::Instance->GetMousePosition();
		}
	}

	if (Input::Instance->GetKey(KeyCode::W))
	{
		m_Position = GetLocal2WorldMatrix() * glm::vec4(-.0f, 0, .5f, 1);
		//m_Position = (m_Position * (glm::mat3)GetViewMatrix() + glm::vec3(-.0f, 0, .5f)) * (glm::mat3)GetLocal2WorldMatrix();
		/*glm::vec3 localOffset = glm::vec4(0, 0, .5f, 1) * GetLocal2WorldMatrix();
		m_Position += localOffset;*/

		auto m = glm::vec4(m_Position.x, m_Position.y, m_Position.z, 1) * GetViewMatrix();
		std::cout << "(" << m.x << "," << m.y << "," << m.z << ")" << std::endl;
		std::cout << std::endl;
	}

	if (Input::Instance->GetKey(KeyCode::S))
	{
		m_Position = GetLocal2WorldMatrix() * glm::vec4(-.0f, 0, -.5f, 1);
		//m_Position = (m_Position * (glm::mat3)GetViewMatrix() + glm::vec3(-.0f, 0, -.5f)) * (glm::mat3)GetLocal2WorldMatrix();
		//glm::vec3 localOffset = glm::vec4(0, 0, -.5f, 1) * GetLocal2WorldMatrix();
		//m_Position += localOffset;

		auto m = glm::vec4(m_Position.x, m_Position.y, m_Position.z, 1) * GetViewMatrix();
		std::cout << "(" << m.x << "," << m.y << "," << m.z << ")" << std::endl;
		std::cout << std::endl;
	}

	if (Input::Instance->GetKey(KeyCode::A))
	{
		m_Position = GetLocal2WorldMatrix() * glm::vec4(-1.5f, 0, 0, 1);
		//m_Position = (m_Position * (glm::mat3)GetViewMatrix() + glm::vec3(-.5f, 0, 0)) * (glm::mat3)GetLocal2WorldMatrix();
		//glm::vec3 localOffset = glm::vec4(-.5f, 0, 0, 1) * GetLocal2WorldMatrix();
		//m_Position += localOffset;

		auto m = glm::vec4(m_Position.x, m_Position.y, m_Position.z, 1) * GetViewMatrix();
		std::cout << "(" << m.x << "," << m.y << "," << m.z << ")" << std::endl;
		std::cout << std::endl;
	}

	if (Input::Instance->GetKey(KeyCode::D))
	{
		m_Position = GetLocal2WorldMatrix() * glm::vec4(1.5f, 0, 0, 1);
		//m_Position = (m_Position * (glm::mat3)GetViewMatrix() + glm::vec3(.5f, 0, 0)) * (glm::mat3)GetLocal2WorldMatrix();
		/*glm::vec3 localOffset = glm::vec4(.5f, 0, 0, 1) * GetLocal2WorldMatrix();
		m_Position += localOffset;*/

		auto m = glm::vec4(m_Position.x, m_Position.y, m_Position.z, 1) * GetViewMatrix();
		std::cout << "(" << m.x << "," << m.y << "," << m.z << ")" << std::endl;
		std::cout << std::endl;
	}
}

