#pragma once
#include "Material.h"

static InterfaceID IID_DEFAULT_MATERIAL = { 4,4,1,{0,0,0,0,0,0,0,0} };

class DefaultMaterial : public Material
{
public:
	IMPLEMENT_QUERY_INTERFACE_INPLACE(IID_DEFAULT_MATERIAL, Material)
public:
	DefaultMaterial();

	void BindResources() override;
	
	static InterfaceID GetIID() { return IID_DEFAULT_MATERIAL; }
};

