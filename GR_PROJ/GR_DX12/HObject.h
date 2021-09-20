#pragma once
#include "InterfaceID.h"
#include "SharedPtr.h"

static const InterfaceID IID_OBJECT = { 0x00000000,0x0000,0x0000 ,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00} };

#define IMPLEMENT_QUERY_INTERFACE_BODY(IID_CURRENT,ParentClass) \
	{																			 \
		if (object == nullptr) return;												 \
		if (iid == IID_CURRENT)														 \
		{																			 \
			*object = this;															 \
			this->AddRef();															 \
		}																			 \
		else																		 \
		{																			 \
			ParentClass::QueryInterface(iid, object);								 \
		}																			 \
	}

#define IMPLEMENT_QUERY_INTERFACE_INPLACE(IID_CURRENT,ParentClass)  \
	 virtual void QueryInterface(const InterfaceID& iid, HObject** object) override  \
	 IMPLEMENT_QUERY_INTERFACE_BODY(IID_CURRENT,ParentClass)

#define IMPLEMENT_QUERY_INTERFACE(IID_CURRENT,ClassName,ParentClass)		 \
	 void ClassName::QueryInterface(const InterfaceID& iid, HObject** object)		 \
	 IMPLEMENT_QUERY_INTERFACE_BODY(IID_CURRENT,ParentClass)

#define IMPLEMENT_DEFAULT_CONSTRUCTS(ClassName)					\
	ClassName() = default;								\
	~ClassName() = default;								\
														\
	ClassName(const ClassName&) = delete;				\
	ClassName(ClassName&&) = delete;					\
	ClassName& operator=(const ClassName&) = delete;	\
	ClassName& operator=(ClassName&&) = delete;			

class HObject
{
public:
	HObject() :m_RefCount(1) {}
	~HObject() = default;

	HObject(const HObject&) = delete;
	HObject(HObject&&) = delete;
	HObject& operator=(const HObject&) = delete;
	HObject& operator=(HObject&&) = delete;

	virtual void QueryInterface(const InterfaceID& iid, HObject** object);
	
	static InterfaceID GetIID();

protected:
	template<typename T>
	friend class SharedPtr;
	
	void AddRef();
	void Release();
	int  GetRefCount();

	int m_RefCount;
};



