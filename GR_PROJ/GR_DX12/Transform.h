#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

#include "Component.h"
#include "Scene.h"

static const InterfaceID IID_TRANSFORM = { 2,1,0,{0,0,0,0,0,0,0,0} };

class Transform : public Component
{
public:
	IMPLEMENT_QUERY_INTERFACE_INPLACE(IID_TRANSFORM, Component)
public:
	Transform(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::quat rotation = glm::identity<glm::quat>(),
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f),GameObject* go = nullptr,Transform* parent = Scene::Default->GetRootTransform())
		:m_Position(position), m_Rotation(rotation), m_Scale(scale),m_Parent(parent)
	{
		//父类赋值
		m_pTransform = this;
		m_pGameObject = go;
	}

	~Transform() override = default;
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
	//删除Component 使用GameObject中的方法删除

private:
	glm::vec3 m_Position;
	glm::quat m_Rotation;
	glm::vec3 m_Scale;

	Transform* m_Parent;
	std::vector<SharedPtr<Transform>> m_Children;
};

