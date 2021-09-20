#pragma once
#include "HObject.h"
#include "Transform.h"

static const InterfaceID IID_GAMEOBJECT = { 1,0,0,{0,0,0,0,0,0,0,0} };

class GameObject : public HObject
{
public:
	IMPLEMENT_QUERY_INTERFACE_INPLACE(IID_GAMEOBJECT, HObject)
public:
	GameObject();
	~GameObject() = default;
	GameObject(const GameObject&) = delete;
	GameObject(GameObject&&) = delete;
	GameObject& operator=(const GameObject&) = delete;
	GameObject& operator=(GameObject&&) = delete;

	SharedPtr<Transform> transform();

	template<typename T>
	SharedPtr<T> GetComponent();
	
	static void Destroy(SharedPtr<Component> cpt);

private:

	std::vector<Component> m_Comps;
	SharedPtr<Transform> m_Transform;
};


template <typename T>
SharedPtr<T> GameObject::GetComponent()
{
	SharedPtr<T> ret;
	for(int i = 0;i < m_Comps.size();i++)
	{
		m_Comps[i].QueryInterface(T::GetIID(), ret.GetAddress());
		if (ret != nullptr)
			break;
	}

	return ret;
}


