#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>

class Scene;

class RendererSystem
{
public:
	RendererSystem();
	RendererSystem(const RendererSystem&) = delete;
	RendererSystem(RendererSystem&&) = delete;

	RendererSystem& operator=(const RendererSystem&) = delete;
	RendererSystem& operator=(RendererSystem&&) = delete;

	void Initialize();
	void Render();

private:

	template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	ID3D12Device* m_Device;
	ID3D12CommandQueue* m_CommandQueue;
	IDXGISwapChain1* m_Swapchain;
	
	ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_CommandListBegin;
	ComPtr<ID3D12GraphicsCommandList> m_CommandListEnd;

	
	
	ComPtr<ID3D12Fence> m_Fence;
	HANDLE m_SyncEvent;
};

