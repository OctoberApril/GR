#pragma once
#include <vector>
#include "Transform.h"

static const InterfaceID IID_GAMEOBJECT = { 1,0,0,{0,0,0,0,0,0,0,0} };

class GameObject : public HObject
{
public:
	IMPLEMENT_QUERY_INTERFACE_INPLACE(IID_GAMEOBJECT, HObject)
public:
	GameObject();
	explicit GameObject(GameObject* parent);
	~GameObject() override;

	Transform* transform() const;
	void AddComponent(Component* cmp);

	template<typename T> T* GetComponent();
	template<typename T> std::vector<T*> GetComponentsInChildren();
	template<typename T> T* AddComponent();

	static void Destroy(SharedPtr<Component> cpt);
	static InterfaceID GetIID();

private:

	std::vector<SharedPtr<Component>> m_Comps;
	Transform* m_pTransform;
};


template <typename T>
T* GameObject::GetComponent()
{
	if (T::GetIID() == Transform::GetIID()) return (T*)m_pTransform;
	
	SharedPtr<T> result;
	for (int i = 0; i < m_Comps.size(); i++)
	{
		m_Comps[i]->QueryInterface(T::GetIID(), reinterpret_cast<HObject**>(result.GetAddress()));
		if (result != nullptr)
			break;
	}

	return result.Get();
};

template<typename T>
std::vector<T*> GameObject::GetComponentsInChildren()
{
	std::vector<T*> result;

	SharedPtr<T> cmp;
	for (int i = 0; i < m_Comps.size(); i++)
	{
		m_Comps[i]->QueryInterface(T::GetIID(), reinterpret_cast<HObject**>(cmp.GetAddress()));
		if (cmp != nullptr)
		{
			result.emplace_back(cmp.Get());
		}
	}

	for (int i = 0; i < m_pTransform->GetChildCount(); i++)
	{
		auto m = m_pTransform->GetChild(i)->m_pGameObject->GetComponentsInChildren<T>();
		result.insert(result.end(), m.begin(), m.end());
	}

	return result;
};

template <typename T>
T* GameObject::AddComponent()
{
	SharedPtr<T> newObj(new T());
	newObj->m_pGameObject = this;
	newObj->m_pTransform = this->m_pTransform;
	
	m_Comps.push_back(newObj);
	return newObj.Get();
};



