#pragma once
#include <wrl.h>
#include <d3d12.h>


class Scene;

class RendererSystem
{
public:
	RendererSystem();
	RendererSystem(const RendererSystem&) = delete;
	RendererSystem(RendererSystem&&) = delete;

	RendererSystem& operator=(const RendererSystem&) = delete;
	RendererSystem& operator=(RendererSystem&&) = delete;

	void Render();

private:

	template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	ID3D12Device* m_Device;

	ComPtr<ID3D12Fence> m_Fence;
	HANDLE m_SyncEvent;
};

