#include "Graphics.h"

#include <iostream>
#include <cassert>
#include "Helper.h"


extern int g_iWidth;
extern int g_iHeight;

DX12Graphics* DX12Graphics::Instance = nullptr;

DX12Graphics::DX12Graphics()
{
	assert(Instance == nullptr);
	Instance = this;
}

DX12Graphics::~DX12Graphics()
{
	Instance = nullptr;
}


bool DX12Graphics::Initialize()
{
	try
	{
#if _DEBUG

		ComPtr<ID3D12Debug> debugInterface;
		ThrowIfFailed(D3D12GetDebugInterface(__uuidof(ID3D12Debug), reinterpret_cast<void**>(debugInterface.GetAddressOf())));
		debugInterface->EnableDebugLayer();

#endif
		ComPtr<IDXGIFactory4> dxgiFactory4;
		ThrowIfFailed(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(dxgiFactory4.GetAddressOf())));

		ComPtr<IDXGIAdapter1> dxgiAdapter4;
		ComPtr<IDXGIAdapter1> cacheAdapter1;
		SIZE_T videoMemory = 0;
		for (int i = 0; dxgiFactory4->EnumAdapters1(i, cacheAdapter1.GetAddressOf()) != DXGI_ERROR_NOT_FOUND; i++)
		{
			DXGI_ADAPTER_DESC1 desc1;
			cacheAdapter1->GetDesc1(&desc1);

			if (!(desc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) && SUCCEEDED(D3D12CreateDevice(cacheAdapter1.Get(), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), nullptr)))
			{
				if (desc1.DedicatedVideoMemory > videoMemory)
				{
					videoMemory = desc1.DedicatedVideoMemory;
					cacheAdapter1->QueryInterface(IID_PPV_ARGS(dxgiAdapter4.GetAddressOf()));
				}
			}
		}

		ThrowIfFailed(D3D12CreateDevice(dxgiAdapter4.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(m_Device.GetAddressOf())));

		D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
		commandQueueDesc.NodeMask = 0;
		commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		ThrowIfFailed(m_Device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(m_GraphicsCommandQueue.GetAddressOf())));

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.Height = g_iHeight;
		swapChainDesc.Width = g_iWidth;
		swapChainDesc.Scaling = DXGI_SCALING::DXGI_SCALING_NONE;
		swapChainDesc.Stereo = FALSE;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

		//DX12 Create SwapChain Need CommandQueue But DX11 Need D3D11Device		
		ThrowIfFailed(dxgiFactory4->CreateSwapChainForHwnd(m_GraphicsCommandQueue.Get(), GetActiveWindow(), &swapChainDesc, nullptr, nullptr, m_SwapChain.GetAddressOf()));

		//Create RTV DescriptorView
		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
		descriptorHeapDesc.NodeMask = 0;
		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		descriptorHeapDesc.NumDescriptors = 2;
		ThrowIfFailed(m_Device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(m_DescriptorHeap.GetAddressOf())));

		//RTV
		for (int i = 0; i < swapChainDesc.BufferCount; i++)
		{
			ComPtr<ID3D12Resource> backBuffer;
			m_SwapChain->GetBuffer(i, IID_PPV_ARGS(backBuffer.GetAddressOf()));
			D3D12_CPU_DESCRIPTOR_HANDLE handle = {};
			handle.ptr = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart().ptr + i * m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			m_Device->CreateRenderTargetView(backBuffer.Get(), nullptr, handle);
		}


#if _DEBUG

		ComPtr<ID3D12InfoQueue> infoQueue;
		if (SUCCEEDED(m_Device.As(&infoQueue)))
		{
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_INFO, true);

			D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };

			D3D12_MESSAGE_ID ids[] = {
				D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
				D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
				D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE
			};

			D3D12_INFO_QUEUE_FILTER filter = {};
			filter.DenyList.NumIDs = 3;
			filter.DenyList.pIDList = ids;
			filter.DenyList.NumSeverities = 1;
			filter.DenyList.pSeverityList = severities;

			ThrowIfFailed(infoQueue->PushRetrievalFilter(&filter));
		}

#endif

		m_Gpass = std::make_shared<GPass>();
		m_Gpass->Initialize(m_Device);

		return true;
	}
	catch (...)
	{
		return false;
	}
}

void DX12Graphics::Update()
{
	m_Gpass->Draw(m_SwapChain, m_Device, m_GraphicsCommandQueue, m_DescriptorHeap);
}

ID3D12Device* DX12Graphics::GetDevice() const
{
	return m_Device.Get();
}
