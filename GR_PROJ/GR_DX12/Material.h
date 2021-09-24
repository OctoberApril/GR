#pragma once
#include "HObject.h"


static const InterfaceID IID_MATERIAL = { 4,4,0,{0,0,0,0,0,0,0,0} };

class Material : public HObject
{
public:
	IMPLEMENT_QUERY_INTERFACE_INPLACE(IID_MATERIAL,HObject)


	
};

