#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include "Component.h"

class GameObject;

static const InterfaceID IID_TRANSFORM = { 2,1,0,{0,0,0,0,0,0,0,0} };

class Transform : public Component
{
public:
	IMPLEMENT_QUERY_INTERFACE_INPLACE(IID_TRANSFORM, Component)
public:
	Transform(GameObject* go, GameObject* parent = nullptr, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::quat rotation = glm::identity<glm::quat>(),
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f));

	~Transform() override;
	
	static InterfaceID GetIID();
	
	glm::mat4 GetLocalToWorldMatrix() const;
	glm::mat4 GetWorldToLocalMatrix() const;

	glm::vec3 GetPosition() const;
	void SetPosition(float x, float y, float z);
	void SetPosition(glm::vec3);

	glm::quat GetRotation() const;
	void SetRotation(glm::quat rotation);

	glm::vec3 GetScale() const;
	void SetScale(float x, float y, float z);
	void SetScale(glm::vec3);

	Transform* Parent() const;
	void SetParent(Transform* parent);

	Transform* GetChild(int index) const;
	int GetChildCount() const;
	void AddChild(Transform* other);

private:
	glm::vec3 m_Position;
	glm::quat m_Rotation;
	glm::vec3 m_Scale;

	Transform* m_Parent;
	std::vector<SharedPtr<Transform>> m_Children;
};

