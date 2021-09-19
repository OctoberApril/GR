#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Component.h"

static const InterfaceID IID_TRANSFORM = { 2,1,0,{0,0,0,0,0,0,0,0} };

class Transform : public Component
{
public:
	IMPLEMENT_QUERY_INTERFACE_INPLACE(IID_TRANSFORM, Component)
public:
	Transform(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::quat rotation = glm::identity<glm::quat>(),
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f))
		:m_Position(position), m_Rotation(rotation), m_Scale(scale)
	{}

	~Transform() = default;

	Transform(const Transform&) = delete;
	Transform(Transform&&) = delete;
	Transform& operator=(const Transform&) = delete;
	Transform& operator=(Transform&&) = delete;

	glm::mat4 GetLocalToWorldMatrix() const;
	glm::mat4 GetWorldToLocalMatrix() const;

	void SetPosition(float x, float y, float z);
	void SetPosition(glm::vec3);
	
	void SetRotation(glm::quat rotation);

	void SetScale(float x, float y, float z);
	void SetScale(glm::vec3);

	Transform& Parent();
	void SetParent(const Transform& parent);

	Transform& GetChild(int num);
	int GetChildCount() const;

	//删除Component 使用GameObject中的方法删除

private:
	glm::vec3 m_Position;
	glm::quat m_Rotation;
	glm::vec3 m_Scale;
};

