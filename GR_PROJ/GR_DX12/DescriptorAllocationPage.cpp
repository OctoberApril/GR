#include "DescriptorAllocationPage.h"
#include "DescriptorAllocation.h"

#include <algorithm>

#include "Graphics.h"
#include <cassert>
#include "Helper.h"

DescriptorAllocationPage::DescriptorAllocationPage(D3D12_DESCRIPTOR_HEAP_TYPE descriptor_type, size_t descriptor_num)
{
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Type = descriptor_type;
	heapDesc.NumDescriptors = descriptor_num;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; //Set Flag = None to later Copy to DynamicHeapDescriptorHeap
	heapDesc.NodeMask = 0;

	ThrowIfFailed(DX12Graphics::Instance->GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(m_DescriptorHeap.GetAddressOf())));
	DESCRIPTOR_INCREMENT_SIZE = DX12Graphics::Instance->GetDevice()->GetDescriptorHandleIncrementSize(descriptor_type);

	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	
	DescriptorAllocation allocation(cpuHandle, gpuHandle, descriptor_num, DESCRIPTOR_INCREMENT_SIZE, this);
	allocation.parent = 0;
	allocation.next = descriptor_num;

	m_AvaliableBlocks.emplace(allocation);
}

bool DescriptorAllocationPage::HasSpace(size_t descriptor_num) const
{
	auto iter = std::find(m_AvaliableBlocks.begin(), m_AvaliableBlocks.end(), descriptor_num);
	return iter == m_AvaliableBlocks.end();
}

DescriptorAllocation DescriptorAllocationPage::Allocate(size_t descriptor_num)
{
	auto cur_block = std::find(m_AvaliableBlocks.begin(), m_AvaliableBlocks.end(), descriptor_num);
	assert(cur_block != m_AvaliableBlocks.end());

	DescriptorAllocation use_block = { cur_block->CPU,cur_block->GPU,descriptor_num,DESCRIPTOR_INCREMENT_SIZE,this };
	use_block.parent = cur_block->parent;
	use_block.next = use_block.parent + descriptor_num;

	D3D12_CPU_DESCRIPTOR_HANDLE newCpuHandle = cur_block->CPU;
	newCpuHandle.ptr += descriptor_num * DESCRIPTOR_INCREMENT_SIZE;
	D3D12_GPU_DESCRIPTOR_HANDLE newGpuHandle = cur_block->GPU;
	newGpuHandle.ptr += descriptor_num * DESCRIPTOR_INCREMENT_SIZE;
	
	DescriptorAllocation new_block = { newCpuHandle,newGpuHandle, cur_block->Length - descriptor_num,DESCRIPTOR_INCREMENT_SIZE,this };
	new_block.parent = use_block.next;
	new_block.next = cur_block->next;

	m_AvaliableBlocks.erase(cur_block);
	m_AvaliableBlocks.emplace(new_block);

	return use_block;
}

void DescriptorAllocationPage::Free(DescriptorAllocation block, uint64_t current_frame)
{
	auto iter = m_StaleDescriptors.find(current_frame);
	if (iter == m_StaleDescriptors.end())
	{
		std::vector<DescriptorAllocation> v = { block };
		m_StaleDescriptors.insert(std::pair<uint64_t, std::vector<DescriptorAllocation>>(current_frame, v));
	}
	else
	{
		iter->second.push_back(block);
	}
}

void DescriptorAllocationPage::Release(uint64_t frame_num)
{
	auto iter = m_StaleDescriptors.find(frame_num);
	if (iter == m_StaleDescriptors.end()) return;

	std::sort(iter->second.begin(), iter->second.end(), [](DescriptorAllocation a, DescriptorAllocation b) {
		return a.parent < b.parent;
		});

	for (auto i = iter->second.begin(), j = i + 1; i != iter->second.end() && j != iter->second.end();)
	{
		if (i->next == j->parent)
		{
			i->next = j->next;
			i->Length += j->Length;
			j = j + 1;
		}
		else
		{
			//不可合并 将当前块压入可用列表 更新当前指针 继续合并
			m_AvaliableBlocks.emplace(*i);
			j = i;
		}
	}
}





