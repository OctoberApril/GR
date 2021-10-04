#pragma once
#include <wrl.h>
#include "HObject.h"


static const InterfaceID IID_MATERIAL = { 4,4,0,{0,0,0,0,0,0,0,0} };

class HPass;

class Material : public HObject
{
public:
	IMPLEMENT_QUERY_INTERFACE_INPLACE(IID_MATERIAL, HObject)
public:
	Material() = default;
	template<typename T>  using ComPtr = Microsoft::WRL::ComPtr<T>;


private:
	HPass* m_pHPass;
};

