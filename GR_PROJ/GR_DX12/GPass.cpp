#define NOMINMAX
#include "GPass.h"

#include <d3dcompiler.h>

#include "Helper.h"
#include "d3dx12.h"
#include "Model.h"
#include "Camera.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

extern int g_iWidth;
extern int g_iHeight;

static Camera* camera = nullptr;

void GPass::Initialize(ComPtr<ID3D12Device> _device)
{
	camera = new Camera();
	camera->LookAtLH(glm::vec3(0, 0, -200.0f), glm::vec3(0.0f), glm::vec3(0, 1.0f, 0));
	camera->OrthoLH(-2000, 2000, -2000, 2000, -1000, 1000);
	camera->PerspectiveLH(100 * 3.1415927f / 180, 1, 10.0f, 10000.0f);

	InitializeInputData(_device);
	CreateCBs(_device);
	CreateRTVs(_device);
	CreateDSV(_device);

	ComPtr<ID3DBlob> errorMsg;
	HRESULT hr = D3DCompileFromFile(L"./Shader/gpass_vs.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, m_VSBlob.GetAddressOf(), errorMsg.GetAddressOf());
	if (errorMsg != nullptr)
	{
		OutputDebugStringA(static_cast<LPCSTR>(errorMsg->GetBufferPointer()));
	}
	ThrowIfFailed(hr);

	hr = D3DCompileFromFile(L"./Shader/gpass_ps.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, m_PSBlob.GetAddressOf(), errorMsg.GetAddressOf());
	if (errorMsg != nullptr)
	{
		OutputDebugStringA(static_cast<LPCSTR>(errorMsg->GetBufferPointer()));
	}
	ThrowIfFailed(hr);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineState = {};
	graphicsPipelineState.VS = { m_VSBlob->GetBufferPointer(), m_VSBlob->GetBufferSize() };
	graphicsPipelineState.PS = { m_PSBlob->GetBufferPointer(),m_PSBlob->GetBufferSize() };

	graphicsPipelineState.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

	graphicsPipelineState.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	graphicsPipelineState.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	graphicsPipelineState.RasterizerState.FrontCounterClockwise = FALSE;
	graphicsPipelineState.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	graphicsPipelineState.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	graphicsPipelineState.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	graphicsPipelineState.RasterizerState.DepthClipEnable = TRUE;
	graphicsPipelineState.RasterizerState.MultisampleEnable = FALSE;
	graphicsPipelineState.RasterizerState.AntialiasedLineEnable = FALSE;
	graphicsPipelineState.RasterizerState.ForcedSampleCount = 0;
	graphicsPipelineState.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	//raphicsPipelineState.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	graphicsPipelineState.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	graphicsPipelineState.SampleMask = 0xffffffff;
	graphicsPipelineState.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	graphicsPipelineState.NumRenderTargets = 1;
	graphicsPipelineState.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	graphicsPipelineState.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	graphicsPipelineState.SampleDesc.Count = 1;
	graphicsPipelineState.SampleDesc.Quality = 0;
	graphicsPipelineState.NodeMask = 0;

	D3D12_INPUT_ELEMENT_DESC elements[3];
	graphicsPipelineState.InputLayout.NumElements = 3;
	graphicsPipelineState.InputLayout.pInputElementDescs = elements;
	elements[0].AlignedByteOffset = 0;
	elements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	elements[0].InputSlot = 0;
	elements[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	elements[0].SemanticIndex = 0;
	elements[0].SemanticName = "POSITION";
	elements[0].InstanceDataStepRate = 0;
	elements[1].AlignedByteOffset = sizeof(float) * 3;
	elements[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	elements[1].InputSlot = 0;
	elements[1].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	elements[1].SemanticIndex = 0;
	elements[1].SemanticName = "COLOR";
	elements[1].InstanceDataStepRate = 0;
	elements[2].AlignedByteOffset = sizeof(float) * 7;
	elements[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	elements[2].InputSlot = 0;
	elements[2].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	elements[2].SemanticIndex = 0;
	elements[2].SemanticName = "NORMAL";
	elements[2].InstanceDataStepRate = 0;

	D3D12_ROOT_PARAMETER rootParameter = {};
	rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	rootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameter.Constants.Num32BitValues = 32;

	D3D12_ROOT_PARAMETER rtvParameter = {};
	rtvParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rtvParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rtvParameter.DescriptorTable.NumDescriptorRanges = 3;
	D3D12_DESCRIPTOR_RANGE range = {};
	range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	range.BaseShaderRegister = 0;
	range.NumDescriptors = 3;
	range.OffsetInDescriptorsFromTableStart = 0;
	range.RegisterSpace = 0;
	rtvParameter.DescriptorTable.pDescriptorRanges = &range;

	D3D12_ROOT_PARAMETER parameters[1] = { rootParameter };

	ComPtr<ID3DBlob> rootSignatureBlob, errBlob;
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(1, parameters,0,nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	
	
	D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, rootSignatureBlob.GetAddressOf(), errBlob.GetAddressOf());
	_device->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(), rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(m_RootSignature.GetAddressOf()));
	graphicsPipelineState.pRootSignature = m_RootSignature.Get();

	ThrowIfFailed(_device->CreateGraphicsPipelineState(&graphicsPipelineState, IID_PPV_ARGS(m_PipelineState.GetAddressOf())));

	
	//Create CommandList & CommandAllocator
	_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_CommandAllocator.GetAddressOf()));	
	_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator.Get(), nullptr, IID_PPV_ARGS(m_CommandList.GetAddressOf()));
	m_CommandList->Close();

	//Create Fence & Event to sync 	
	_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_Fence.GetAddressOf()));
	m_SyncEvent = CreateEvent(nullptr, false, true, nullptr);	
}

static uint32_t numIndices;

void GPass::InitializeInputData(ComPtr<ID3D12Device> _device)
{	
	CD3DX12_HEAP_PROPERTIES properties(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Alignment = 0;
	resDesc.Width = 1024 * 1024 * 50;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	ThrowIfFailed(_device->CreateCommittedResource(&properties, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(m_UploadResourcesPage.GetAddressOf())));

	//Load Sponza Model
	Model model;
	model.LoadFileFromPath("./Model/Sponza/sponza.obj");
	//model.LoadFileFromPath("./Model/BackPack/BackPack.fbx");
	auto vertices = model.GetVertexs();
	auto indices = model.GetIndices();
	/*std::vector<VertexData> vertices;
	std::vector<uint32_t> indices;

	VertexData v1;
	v1.position = float3(-1, -1, 0.5f);
	VertexData v2;
	v2.position = float3(0, 1, 0.5f);
	VertexData v3;
	v3.position = float3(1, 0, 0.5f);

	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);

	indices.emplace_back(0);
	indices.emplace_back(1);
	indices.emplace_back(2);*/

	struct VSInput
	{
		float3 modelPosition;
		float4 color;
		float3 normal;
	};
	
	VSInput* pVertexInput = new VSInput[vertices.size()];
	for (int i = 0; i < vertices.size(); i++)
	{
		pVertexInput[i].modelPosition = vertices[i].position;
		pVertexInput[i].color = vertices[i].color;
		pVertexInput[i].normal = vertices[i].normal; //float3(0, 1, 0);
	}

	uint32_t* vsIndices = new uint32_t[indices.size()];
	for (int i = 0; i < indices.size(); i++)
	{
		vsIndices[i] = indices[i];
	}

	//copy vertices data to gpu
	byte* buf = nullptr;
	uint32_t bufSize = vertices.size() * sizeof(VSInput);
	m_UploadResourcesPage->Map(0, nullptr,(void**)&buf);
	memcpy(buf, pVertexInput, bufSize);
	m_VertexBufferView.BufferLocation = m_UploadResourcesPage->GetGPUVirtualAddress();
	m_VertexBufferView.SizeInBytes = bufSize;
	m_VertexBufferView.StrideInBytes = sizeof(VSInput);

	//copy indices data to gpu
	m_IndexBufferView.BufferLocation = m_UploadResourcesPage->GetGPUVirtualAddress() + bufSize;
	bufSize = indices.size() * sizeof(uint32_t);
	memcpy(buf + vertices.size() * sizeof(VSInput), vsIndices, bufSize);
	m_IndexBufferView.SizeInBytes = bufSize;
	m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;

	m_UploadResourcesPage->Unmap(0,nullptr);
	numIndices = indices.size();
}

static uint64_t g_frameCount = 0;

void GPass::Draw(ComPtr<IDXGISwapChain> _swapChain,ComPtr<ID3D12Device> _device,ComPtr<ID3D12CommandQueue> _cmdQueue,ComPtr<ID3D12DescriptorHeap> _rtvDescriptorHeap)
{
	if (m_CommandAllocator == nullptr || m_CommandList == nullptr) return;
	WaitForSingleObject(m_SyncEvent, INFINITE);
	m_Fence->Signal(0);

	camera->Update();

	float model2world[16] = { 1,0,0,583.1f, 0,1,0,-754, 0,0,1,-432, 0,0,0,1 };
	m_CommandAllocator->Reset();
	m_CommandList->Reset(m_CommandAllocator.Get(), nullptr);
	
	ComPtr<ID3D12Resource> backBuffer;
	_swapChain->GetBuffer(g_frameCount++ % 2, IID_PPV_ARGS(backBuffer.GetAddressOf()));

	D3D12_RESOURCE_BARRIER resBarrier = {};
	resBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	resBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	resBarrier.Transition.pResource = backBuffer.Get();
	resBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	resBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	resBarrier.Transition.Subresource = 0;
	m_CommandList->ResourceBarrier(1, &resBarrier);

	m_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_CommandList->SetPipelineState(m_PipelineState.Get());
	m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());
	m_CommandList->SetGraphicsRoot32BitConstants(0, 16, glm::value_ptr((camera->GetViewMatrix())), 0);
	m_CommandList->SetGraphicsRoot32BitConstants(0, 16, glm::value_ptr((camera->GetPerspectiveMatrix(false))), 16);

	m_CommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
	m_CommandList->IASetIndexBuffer(&m_IndexBufferView);
	
	D3D12_VIEWPORT viewPort = {};
	viewPort.Width = g_iWidth;
	viewPort.Height = g_iHeight;
	viewPort.MinDepth = 0;
	viewPort.MaxDepth = 1;
	m_CommandList->RSSetViewports(1, &viewPort);

	D3D12_RECT scissorRect = {};
	scissorRect.left = 0;
	scissorRect.top = 0;
	scissorRect.right = g_iWidth;
	scissorRect.bottom = g_iHeight;
	m_CommandList->RSSetScissorRects(1, &scissorRect);

	/*const float clearColor[4] = { 0.0f,0.0f,0.0f,1.0f };;
	m_CommandList->ClearRenderTargetView(m_RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), clearColor, 0, nullptr);*/
	/*D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();*/

	const float clearColor[4] = { 0.0f,0.0f,0.0f,1.0f };
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
	rtvHandle.ptr = _rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart().ptr + ((g_frameCount - 1)%2) * _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_DsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	m_CommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	m_CommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAGS::D3D12_CLEAR_FLAG_DEPTH, 1, 0, 0, nullptr);

	
	m_CommandList->OMSetRenderTargets(1, &rtvHandle, true, &dsvHandle);
	
	m_CommandList->DrawIndexedInstanced(numIndices, 1, 0, 0, 0);

	D3D12_RESOURCE_BARRIER presentBarrier = {};
	presentBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	presentBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	presentBarrier.Transition.pResource = backBuffer.Get();
	presentBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	presentBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	presentBarrier.Transition.Subresource = 0;
	m_CommandList->ResourceBarrier(1, &presentBarrier);

	m_CommandList->Close();

	_cmdQueue->ExecuteCommandLists(1, (ID3D12CommandList**)m_CommandList.GetAddressOf());
	m_Fence->SetEventOnCompletion(1, m_SyncEvent);
	_cmdQueue->Signal(m_Fence.Get(), 1);			
	_swapChain->Present(1, 0);
}

void GPass::CreateCBs(ComPtr<ID3D12Device> _device)
{
	
}

void GPass::CreateDSV(ComPtr<ID3D12Device> _device)
{
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
	descriptorHeapDesc.NumDescriptors = 1;
	descriptorHeapDesc.NodeMask = 0;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	ThrowIfFailed(_device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(m_DsvDescriptorHeap.GetAddressOf())));

	CD3DX12_HEAP_PROPERTIES dsvProperties(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Alignment = 0;
	resourceDesc.Width = g_iWidth;
	resourceDesc.Height = g_iHeight;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	clearValue.DepthStencil.Depth = 1.0f;

	ThrowIfFailed(_device->CreateCommittedResource(&dsvProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc,D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearValue, IID_PPV_ARGS(m_DsvResource.GetAddressOf())));

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvViewDesc = {}; 
	dsvViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvViewDesc.Flags = D3D12_DSV_FLAG_READ_ONLY_DEPTH;
	_device->CreateDepthStencilView(m_DsvResource.Get(), &dsvViewDesc, m_DsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
}

void GPass::CreateRTVs(ComPtr<ID3D12Device> _device)
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc = {};
	rtvDescriptorHeapDesc.NodeMask = 0;
	rtvDescriptorHeapDesc.NumDescriptors = 3;
	rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	_device->CreateDescriptorHeap(&rtvDescriptorHeapDesc, IID_PPV_ARGS(m_RtvDescriptorHeap.GetAddressOf()));

	D3D12_HEAP_PROPERTIES heapProperties = {};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProperties.CreationNodeMask = 0;
	heapProperties.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resDesc.Alignment = 0;
	resDesc.Width = g_iWidth;
	resDesc.Height = g_iHeight;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	D3D12_CLEAR_VALUE clearColor = {};
	for (int i = 0; i < 3; i++)
	{
		clearColor.Format = m_RtvFormats[i];
		resDesc.Format = m_RtvFormats[i];
		ThrowIfFailed(_device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_RENDER_TARGET, &clearColor, IID_PPV_ARGS(m_RtvResources[i].GetAddressOf())));
	}

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.Texture2D.PlaneSlice = 0;

	for (int i = 0; i < 3; i++)
	{
		rtvDesc.Format = m_RtvFormats[i];
		D3D12_CPU_DESCRIPTOR_HANDLE handle;
		handle.ptr = m_RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart().ptr + i * _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		_device->CreateRenderTargetView(m_RtvResources[i].Get(), &rtvDesc, handle);
	}
}