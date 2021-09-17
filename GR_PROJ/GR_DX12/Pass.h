#pragma once
#include <d3d12.h>
#include <wrl/client.h>

class Pass
{
protected:
	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr <T>;

public:
	Pass() = default;
	Pass(const Pass&) = delete;
	virtual ~Pass() = default;
	Pass& operator=(const Pass&) = delete;

	virtual void Initialize(ComPtr<ID3D12Device>) = 0;
	virtual void Draw(ComPtr<IDXGISwapChain> _swapChain, ComPtr<ID3D12Device> _device, ComPtr<ID3D12CommandQueue> _cmdQueue, ComPtr<ID3D12DescriptorHeap> _rtvDescriptorHeap) = 0;

protected:	

	ComPtr<ID3D12PipelineState> m_PipelineState;

	ComPtr<ID3DBlob> m_VSBlob;
	ComPtr<ID3DBlob> m_PSBlob;
};

