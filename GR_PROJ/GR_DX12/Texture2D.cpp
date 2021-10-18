#include "Texture2D.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include <cassert>
#include "Graphics.h"
#include "d3dx12.h"
#include <iostream>
#include <wrl.h>
#include <string>

using namespace DirectX;

static std::unordered_map<std::wstring, ID3D12Resource*> g_TexResMap = {};
static int g_count = 0;

Texture2D::Texture2D() :m_ImageData(nullptr),m_Resource(nullptr)
{

}

Texture2D::Texture2D(const wchar_t* imagePath)
{
	assert(imagePath != nullptr);
	auto s = std::wstring(imagePath);
	if (g_TexResMap.count(s) != 0)
	{
		m_Resource = g_TexResMap[std::wstring(imagePath)];
	}
	else
	{
		LoadFromTGAFile(imagePath, &m_Meta, m_Image);
		//m_ImageData = stbi_load(imagePath, &m_ImageWidth, &m_ImageHeight, &m_ImageChannel, 0);
		CreateImageResource();

		g_TexResMap.emplace(std::wstring(imagePath), m_Resource.Get());
		std::wcout << "LoadImage : " << (++g_count) << imagePath << std::endl;
	}
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
	//upload heap => default heap
	auto device = DX12Graphics::Instance->GetDevice();
	auto cmdQueue = DX12Graphics::Instance->GetCommandQueue();
	
	//Craete default heap && upload heap
	auto properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto resDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM,m_Meta.width, m_Meta.height);
	device->CreateCommittedResource(
		&properties,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(m_Resource.GetAddressOf()));
	
	int bufSize = GetRequiredIntermediateSize(m_Resource.Get(), 0, 1);
	ComPtr<ID3D12Resource> uploadRes;
	properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	resDesc = CD3DX12_RESOURCE_DESC::Buffer(bufSize);;
	device->CreateCommittedResource(
		&properties,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&uploadRes));
	
	std::vector<D3D12_SUBRESOURCE_DATA> subresources(m_Image.GetImageCount());
	const Image* pImages = m_Image.GetImages();
	for (int i = 0; i < m_Image.GetImageCount(); ++i)
	{
		auto& subresource = subresources[i];
		subresource.RowPitch = pImages[i].rowPitch;
		subresource.SlicePitch = pImages[i].slicePitch;
		subresource.pData = pImages[i].pixels;
	}
	
	ComPtr<ID3D12CommandAllocator> commandAllocator;
	ComPtr<ID3D12GraphicsCommandList> commandlist;
	ComPtr<ID3D12Fence> fence;
	auto waitEvent = CreateEvent(nullptr, false, false, nullptr);
	device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf()));
	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator.GetAddressOf()));
	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(commandlist.GetAddressOf()));
	commandlist->Close();
	fence->Signal(0);

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
	UpdateSubresources(commandlist.Get(), m_Resource.Get(), uploadRes.Get(), 0, 0, static_cast<uint32_t>(subresources.size()),
		subresources.data());

	commandlist->Close();
	cmdQueue->ExecuteCommandLists(1, (ID3D12CommandList**)commandlist.GetAddressOf());
	fence->SetEventOnCompletion(1, waitEvent);
	cmdQueue->Signal(fence.Get(), 1);	
	WaitForSingleObject(waitEvent,INFINITE);
}


HObject::ComPtr<struct ID3D12Resource> Texture2D::GetResource() const
{
	return m_Resource;
}



