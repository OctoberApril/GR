#pragma once
#include "HObject.h"

static const InterfaceID IID_GAMEOBJECT = { 1,0,0,{0,0,0,0,0,0,0,0} };

class GameObject : public HObject
{
public:
	IMPLEMENT_QUERY_INTERFACE_INPLACE(IID_GAMEOBJECT, HObject)
	IMPLEMENT_DEFAULT_CONSTRUCTS(GameObject)
public:


	
private:
	 
};

