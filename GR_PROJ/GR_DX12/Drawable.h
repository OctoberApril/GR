#pragma once
#include "Component.h"
#include "wrl.h"
#include <d3d12.h>

static const InterfaceID IID_DRAWABLE = { 4,0,0,{0,0,0,0,0,0,0,0} };


class Drawable : public Component
{
public:
	IMPLEMENT_QUERY_INTERFACE_INPLACE(IID_DRAWABLE, Component)
public:
	static InterfaceID GetIID() { return IID_DRAWABLE; }

	template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	
	virtual ComPtr<ID3D12CommandList> Draw() = 0;
};

