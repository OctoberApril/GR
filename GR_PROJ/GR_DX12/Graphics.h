#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <dxgi1_6.h>
#include <memory>


class DX12Graphics
{
	template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	DX12Graphics(int w, int h);
	~DX12Graphics();
	DX12Graphics(const DX12Graphics&) = delete;
	DX12Graphics(DX12Graphics&&) = delete;

	DX12Graphics& operator=(const DX12Graphics&) = delete;
	DX12Graphics& operator=(DX12Graphics&&) = delete;

	bool Initialize();
	void Update();
	void LateUpdate();

	ID3D12Device* GetDevice() const;
	ID3D12CommandQueue* GetCommandQueue() const;
	IDXGISwapChain1* GetSwapchain() const;
	int GetGraphicsWidth() const;
	int GetGraphicsHeight() const;
	
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRtvDescriptor() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetDsvDescriptor() const;
	ComPtr<ID3D12Resource> GetCurrentRtvResource() const;

	static DX12Graphics* Instance;

private:

	ComPtr<ID3D12Device3> m_Device;
	ComPtr<ID3D12CommandQueue> m_GraphicsCommandQueue;
	ComPtr<IDXGISwapChain1> m_SwapChain;
	ComPtr<ID3D12DescriptorHeap> m_RtvDescriptorHeap;
	ComPtr<ID3D12DescriptorHeap> m_DsvDescriptorHeap;
	ComPtr<ID3D12Resource> m_DsvResource;

	int m_GraphicsWidth;
	int m_GraphicsHeight;

	uint64_t m_GlobalFrame;
};
