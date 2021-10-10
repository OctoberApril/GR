#pragma once
#include <glm/fwd.hpp>

#include "Component.h"

static const InterfaceID IID_THIRDCAMERA = { 2,2,0,{0,0,0,0,0,0,0,0} };

enum CameraMode
{
	CameraMode_Perspective,
	CameraMode_Ortho
};

class ThirdCamera : public Component
{
public:
	IMPLEMENT_QUERY_INTERFACE_INPLACE(IID_THIRDCAMERA, Component)
public:
	ThirdCamera(CameraMode mode = CameraMode_Perspective);

	void SetOrthoCamera(float _left, float _right, float _top, float _bottom, float _near, float _far);
	void SetPerspectiveCamera(float fov, float aspect, float nearPlane, float farPlane);

	glm::mat4 GetViewMatrix()const;
	glm::mat4 GetPerspectiveMatrix() const;
	void SetCameraMode(CameraMode mode);
	

private:
	
	//Perspective
	float m_Fov, m_Aspect, m_NearPlane, m_FarPlane;

	//Ortho
	float m_Left, m_Right, m_Top, m_Bottom, m_Near, m_Far;

	CameraMode m_CameraMode;
};

