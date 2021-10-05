#include "UploadBuffer.h"

#include <cassert>
#include "d3dx12.h"
#include "Graphics.h"
#include "Helper.h"

UploadBuffer::UploadBuffer(size_t size) : m_SizeOfPage(size)
{
	
}

BufferAllocation UploadBuffer::Allocate(size_t size, int alignment)
{
	for(auto p = m_AvaliablePages.begin();p != m_AvaliablePages.end();p++)
	{		
		if((*p)->HasSpace(size,alignment))
		{
			return (*p)->Allocate(size, alignment);
		}
	}

	auto p = CreateUploadPage();
	return p->Allocate(size, alignment);
}

void UploadBuffer::Reset() const
{
	for(auto p = m_AvaliablePages.begin();p != m_AvaliablePages.end();p++)
	{
		(*p)->Reset();
	}
}

UploadBuffer::UploadPage* UploadBuffer::CreateUploadPage()
{
	SharedPtr<UploadPage> newPage = std::make_shared<UploadPage>(m_SizeOfPage);
	m_AvaliablePages.emplace(newPage);
	return newPage.get();
}



UploadBuffer::UploadPage::UploadPage(size_t size) : GpuAddress(0),CpuAddress(nullptr), m_Offset(0), m_Resource(nullptr), m_PageSize(size)
{
	auto device = DX12Graphics::Instance->GetDevice();
	assert(device != nullptr);

	D3D12_HEAP_PROPERTIES heapProperties = {};
	heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProperties.CreationNodeMask = 0;
	heapProperties.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC bufferDesc =  CD3DX12_RESOURCE_DESC::Buffer(size);	
	ThrowIfFailed(device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(m_Resource.GetAddressOf())));

	m_Resource->Map(0, nullptr, &CpuAddress);
	GpuAddress = m_Resource->GetGPUVirtualAddress();
}

UploadBuffer::UploadPage::~UploadPage()
{
	if(m_Resource != nullptr)
	{
		m_Resource->Unmap(0,nullptr);
		CpuAddress = nullptr;
		GpuAddress = 0;
	}
}


BufferAllocation UploadBuffer::UploadPage::Allocate(size_t size, int alignment)
{
	size = AlignUp(size, alignment);
	
	
	BufferAllocation allocation;
	allocation.CpuAddress = static_cast<uint8_t*>(CpuAddress) + m_Offset;
	allocation.GpuAddress = GpuAddress + m_Offset;

	m_Offset += size;
	return allocation;
	
}

bool UploadBuffer::UploadPage::HasSpace(size_t size, int alignment) const
{
	size = AlignUp(size, alignment);
	return (m_Offset + size) <= m_PageSize;
}

void UploadBuffer::UploadPage::Reset()
{
	m_Offset = 0;
}








