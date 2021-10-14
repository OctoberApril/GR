#pragma once
#include "Material.h"

static InterfaceID IID_DEFAULT_MATERIAL = { 4,4,1,{0,0,0,0,0,0,0,0} };

class DefaultMaterial : public Material
{
public:
	IMPLEMENT_QUERY_INTERFACE_INPLACE(IID_DEFAULT_MATERIAL, Material)
public:
	DefaultMaterial() :Material(L"./Shader/gpass_vs.hlsl", L"./Shader/gpass_ps.hlsl") {}

	void BindResources() override;
	
	static InterfaceID GetIID() { return IID_DEFAULT_MATERIAL; }
};

