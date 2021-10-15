#include "Texture2D.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include <cassert>
#include "Graphics.h"
#include "d3dx12.h"


Texture2D::Texture2D() :m_ImageData(nullptr),m_Resource(nullptr)
{

}

Texture2D::Texture2D(const char* imagePath)
{
	assert(imagePath != nullptr);
	m_ImageData = stbi_load(imagePath, &m_ImageWidth, &m_ImageHeight, &m_ImageChannel, 0);
	CreateImageResource();
}

Texture2D::~Texture2D()
{
	if (!m_ImageData)
		stbi_image_free(m_ImageData);
}

unsigned char* Texture2D::GetImageData() const
{
	return m_ImageData;
}

void Texture2D::CreateImageResource()
{
	if (m_ImageData == nullptr) return;

	//upload heap => default heap
	auto device = DX12Graphics::Instance->GetDevice();
	auto cmdQueue = DX12Graphics::Instance->GetCommandQueue();
	
	size_t bufferSize = m_ImageWidth * m_ImageHeight * 3 * sizeof(float);
	//Craete default heap && upload heap
	auto properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
	device->CreateCommittedResource(
		&properties,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(m_Resource.GetAddressOf()));
	
	ComPtr<ID3D12Resource> uploadRes;
	properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	resDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
	device->CreateCommittedResource(
		&properties,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&uploadRes));
	
	D3D12_SUBRESOURCE_DATA data = {};
	data.pData = m_ImageData;
	data.SlicePitch = 0;
	data.RowPitch = 0;
	
	ComPtr<ID3D12CommandAllocator> commandAllocator;
	ComPtr<ID3D12GraphicsCommandList> commandlist;
	ComPtr<ID3D12Fence> fence;
	auto waitEvent = CreateEvent(nullptr, false, true, nullptr);
	device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf()));
	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator.GetAddressOf()));
	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(commandlist.GetAddressOf()));
	commandlist->Close();


	commandAllocator->Reset();
	commandlist->Reset(commandAllocator.Get(), nullptr);

	D3D12_RESOURCE_BARRIER dstImageBarrier;
	dstImageBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	dstImageBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	dstImageBarrier.Transition.pResource = m_Resource.Get();
	dstImageBarrier.Transition.Subresource = 0;
	dstImageBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
	dstImageBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
	
	commandlist->ResourceBarrier(1, &dstImageBarrier);
	commandlist->CopyResource(m_Resource.Get(), uploadRes.Get());
	//UpdateSubresources(commandlist.Get(), m_Resource.Get(), uploadRes.Get(), 0, 0, 1, &data);
	commandlist->Close();
	cmdQueue->ExecuteCommandLists(1, (ID3D12CommandList**)commandlist.GetAddressOf());
	cmdQueue->Signal(fence.Get(), 1);
	fence->SetEventOnCompletion(1, waitEvent);
	WaitForSingleObject(waitEvent,INFINITE);
}


HObject::ComPtr<struct ID3D12Resource> Texture2D::GetResource() const
{
	return m_Resource;
}



