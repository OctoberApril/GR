#pragma once
#include "InterfaceID.h"
#include "SharedPtr.h"
#include "Noncopy.h"

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




class HObject : public Noncopy
{
public:
	HObject() :m_RefCount(0) {}
	virtual ~HObject();


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



