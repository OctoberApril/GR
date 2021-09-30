#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <set>
#include <map>
#include <vector>

struct DescriptorAllocation;

class DescriptorAllocationPage
{
public:
	DescriptorAllocationPage(D3D12_DESCRIPTOR_HEAP_TYPE descriptor_type, size_t descriptor_num);

	bool HasSpace(size_t descriptor_num) const;

	DescriptorAllocation Allocate(size_t descriptor_num);

	void Free(DescriptorAllocation block, uint64_t current_frame);

	void Release(uint64_t frame_num);

private:
	template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	/// <summary>
	/// 当前可用的Descriptor Block
	/// </summary>
	std::set<DescriptorAllocation> m_AvaliableBlocks;

	/// <summary>
	/// 等待回收的Descriptor Block
	/// </summary>
	std::map<uint64_t, std::vector<DescriptorAllocation>> m_StaleDescriptors;

	ComPtr<ID3D12DescriptorHeap> m_DescriptorHeap;

	unsigned int DESCRIPTOR_INCREMENT_SIZE;
};

