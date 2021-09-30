#pragma once
#include "GPass.h"
#include <d3d12.h>
#include <wrl.h>
#include <dxgi1_6.h>
#include <memory>


class DX12Graphics
{
	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	DX12Graphics();
	~DX12Graphics();
	DX12Graphics(const DX12Graphics&) = delete;
	DX12Graphics(DX12Graphics&&) = delete;

	DX12Graphics& operator=(const DX12Graphics&) = delete;
	DX12Graphics& operator=(DX12Graphics&&) = delete;

	bool Initialize();

	void Update();

	ComPtr<ID3D12DescriptorHeap> GetRtvDescriptorHeap() { return m_DescriptorHeap; }

	ID3D12Device* GetDevice() const;

	static DX12Graphics* Instance;

private:
		
	ComPtr<ID3D12CommandQueue> m_GraphicsCommandQueue;

	ComPtr<ID3D12CommandAllocator> m_GraphicsCommandAllocator;

	ComPtr<ID3D12Device3> m_Device;

	ComPtr<ID3D12DescriptorHeap> m_DescriptorHeap;

	ComPtr<IDXGISwapChain1> m_SwapChain;	

	std::shared_ptr<GPass> m_Gpass;
};
