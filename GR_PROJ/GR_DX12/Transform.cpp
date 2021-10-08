#include "Transform.h"
#include "Scene.h"
#include "GameObject.h"

#include <glm/gtx/quaternion.hpp>


Transform::Transform(GameObject* go, GameObject* parent, glm::vec3 position, glm::quat rotation, glm::vec3 scale)
	:m_Position(position), m_Rotation(rotation), m_Scale(scale),
	m_Parent(parent != nullptr ? parent->transform() : nullptr)
{
	assert(go != nullptr);
	//父类赋值
	Component::m_pTransform = this;
	Component::m_pGameObject = go;

	if (parent == nullptr)
	{
		Scene::Default->AddGameObjectToSceneRoot(go);
	}
}

Transform::~Transform()
{
	m_Children.clear();
}

InterfaceID Transform::GetIID()
{
	return IID_TRANSFORM;
}

glm::vec3 Transform::GetPosition() const
{
	return m_Position;
}

void Transform::SetPosition(float x, float y, float z)
{
	m_Position = glm::vec3(x, y, z);
}

void Transform::SetPosition(glm::vec3 position)
{
	m_Position = position;
}

glm::quat Transform::GetRotation() const
{
	return m_Rotation;
}

void Transform::SetRotation(glm::quat rotation)
{
	m_Rotation = rotation;
}

glm::vec3 Transform::GetScale() const
{
	return m_Scale;
}

void Transform::SetScale(float x, float y, float z)
{
	m_Scale = glm::vec3(x, y, z);
}

void Transform::SetScale(glm::vec3 scale)
{
	m_Scale = scale;
}

Transform* Transform::Parent() const
{
	return m_Parent;
}

void Transform::SetParent(Transform* parent)
{
	if (this->m_Parent == parent) return;
	if(this->m_Parent == nullptr)
	{
		Scene::Default->RemoveGameObjectFromScene(this->m_pGameObject);
	}
	
	if(parent == nullptr)
	{
		Scene::Default->AddGameObjectToSceneRoot(this->m_pGameObject);
	}
	m_Parent = parent;
}

Transform* Transform::GetChild(int index) const
{
	if (index >= m_Children.size() || index < 0) throw std::exception("get child index > children size() ");
	return m_Children[index];
}

int Transform::GetChildCount() const
{
	return m_Children.size();
}

void Transform::AddChild(Transform* other)
{
	m_Children.emplace_back(other);
}


glm::mat4 Transform::GetLocalToWorldMatrix() const
{
	auto S = glm::scale(glm::mat4(1), m_Scale);
	auto R = glm::toMat4(m_Rotation);
	auto T = glm::translate(glm::mat4(1), m_Position);

	glm::mat4 TRS = S * R * T;
	if (m_Parent == nullptr)
		return m_Parent->GetLocalToWorldMatrix() * TRS;
	else
		return TRS;
}

glm::mat4 Transform::GetWorldToLocalMatrix() const
{
	return glm::inverse(GetLocalToWorldMatrix());
}















