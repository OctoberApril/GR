#include "ThirdCamera.h"
#include "Scene.h"
#include "Transform.h"


ThirdCamera::ThirdCamera(CameraMode mode) :
					m_Left(-100), m_Right(100), m_Top(100), m_Bottom(-100), m_Near(-100), m_Far(100),
					m_Fov(60 * 3.1415927f / 180), m_Aspect(1.0f), m_NearPlane(0.01f), m_FarPlane(1000.0f),m_CameraMode(mode)
{
	Scene::Default->SetSceneCamera(this);
}

void ThirdCamera::SetOrthoCamera(float _left, float _right, float _top, float _bottom, float _near, float _far)
{
	this->m_Left = _left;
	this->m_Right = _right;
	this->m_Top = _top;
	this->m_Bottom = m_Bottom;
	this->m_Near = _near;
	this->m_Far = _far;
}

void ThirdCamera::SetPerspectiveCamera(float fov, float aspect, float nearPlane, float farPlane)
{
	this->m_Fov = fov;
	this->m_Aspect = aspect;
	this->m_NearPlane = nearPlane;
	this->m_FarPlane = farPlane;
}

glm::mat4 ThirdCamera::GetViewMatrix() const
{
	return this->m_pTransform->GetWorldToLocalMatrix();
}

glm::mat4 ThirdCamera::GetPerspectiveMatrix() const
{
	return m_CameraMode == CameraMode_Perspective ? glm::perspectiveLH_ZO(m_Fov, m_Aspect, m_NearPlane, m_FarPlane) :
		glm::orthoLH_ZO(m_Left, m_Right, m_Bottom, m_Top, m_Near, m_Far);		
}

void ThirdCamera::SetCameraMode(CameraMode mode)
{
	m_CameraMode = mode;
}





