#pragma once
#include "Renderer.h"


static const InterfaceID IID_MESHRENDERER = { 4,2,0,{0,0,0,0,0,0,0,0} };

class Material;
class UploadBuffer;

class MeshRenderer : public Renderer
{
public:
	IMPLEMENT_QUERY_INTERFACE_INPLACE(IID_MESHRENDERER, Renderer)
public:
	
	MeshRenderer();
	~MeshRenderer() override = default;
	
	ComPtr<ID3D12CommandList> Draw() override;
	bool IsNeedToDraw() const override;
	
	void SetMaterial(Material* material);

	static InterfaceID GetIID() { return IID_MESHRENDERER; }
private:
	void Initialize();

private:
	SharedPtr<Material> m_Material;
	ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_CommandList;	
	ID3D12Device* m_Device;

	D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
	D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;

	UploadBuffer* m_UploadBuffer;
	
	bool m_bInit = false;
};

