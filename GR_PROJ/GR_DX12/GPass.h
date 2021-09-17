#pragma once
#include "Pass.h"

class GPass : public Pass
{
public:
	void Initialize(ComPtr<ID3D12Device> _device) override;
	void Draw(ComPtr<IDXGISwapChain> _swapChain, ComPtr<ID3D12Device> _device, ComPtr<ID3D12CommandQueue> _cmdQueue, ComPtr<ID3D12DescriptorHeap> _rtvDescriptorHeap) override;

private:

	void InitializeInputData(ComPtr<ID3D12Device> _device);
	void CreateCBs(ComPtr<ID3D12Device> _device);
	void CreateRTVs(ComPtr<ID3D12Device> _device);
	void CreateDSV(ComPtr<ID3D12Device> _device);


	D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
	D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;
	ComPtr<ID3D12Resource> m_UploadResourcesPage;

	ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_CommandList;

	ComPtr<ID3D12DescriptorHeap> m_RtvDescriptorHeap;
	ComPtr<ID3D12Resource> m_RtvResources[3];

	ComPtr<ID3D12DescriptorHeap> m_DsvDescriptorHeap;
	ComPtr<ID3D12Resource> m_DsvResource;

	ComPtr<ID3D12RootSignature> m_RootSignature;

	DXGI_FORMAT m_RtvFormats[3] = { DXGI_FORMAT_R8G8B8A8_UNORM,DXGI_FORMAT_R8G8B8A8_UNORM,DXGI_FORMAT_R8G8B8A8_UNORM };

	ComPtr<ID3D12Fence> m_Fence;
	HANDLE m_SyncEvent;
};

