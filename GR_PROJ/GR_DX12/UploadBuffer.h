#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <set>
#include <memory>

struct BufferAllocation
{
	void* CpuAddress;
	D3D12_GPU_VIRTUAL_ADDRESS GpuAddress;
	size_t Size;
};

class UploadBuffer
{
public:
	template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	template<typename T> using SharedPtr = std::shared_ptr<T>;
public:
	explicit UploadBuffer(size_t size = 1024 * 1024 * 2);
	~UploadBuffer() = default;
	
	BufferAllocation Allocate(size_t size,int alignment);

	void Reset() const;

private:

	class UploadPage
	{
	public:
		explicit UploadPage(size_t size);
		~UploadPage();

		bool HasSpace(size_t size, int alignment) const;

		BufferAllocation Allocate(size_t size, int alignment);

		void Reset();
	
	private:
		void* CpuAddress;
		
		D3D12_GPU_VIRTUAL_ADDRESS GpuAddress;

		size_t m_Offset;
		size_t m_PageSize;
		
		ComPtr<ID3D12Resource> m_Resource;
	};

	UploadPage* CreateUploadPage();

private:

	size_t m_SizeOfPage;
	
	std::set<SharedPtr<UploadPage>> m_AvaliablePages;
};

