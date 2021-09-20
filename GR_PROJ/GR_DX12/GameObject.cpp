#include "GameObject.h"


GameObject::GameObject() :m_Transform(new Transform())
{

}

SharedPtr<Transform> GameObject::transform()
{
	return m_Transform;
}

void GameObject::Destroy(SharedPtr<Component> cpt)
{
	if (cpt == nullptr) return;
}

