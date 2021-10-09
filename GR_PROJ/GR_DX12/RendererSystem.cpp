#include "RendererSystem.h"
#include <cassert>
#include <iostream>

#include "Drawable.h"
#include "GameObject.h"
#include "Graphics.h"
#include "MeshRenderer.h"
#include "Scene.h"


RendererSystem::RendererSystem()
{
	
}

void RendererSystem::Initialize()
{
	m_Device = DX12Graphics::Instance->GetDevice();
	assert(m_Device != nullptr);

	m_Device->CreateFence(1, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_Fence.GetAddressOf()));
	m_Swapchain = DX12Graphics::Instance->GetSwapchain();
	m_CommandQueue = DX12Graphics::Instance->GetCommandQueue();	
	m_SyncEvent = CreateEvent(nullptr, false, true, nullptr);
		
	m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_CommandAllocator.GetAddressOf()));
	m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator.Get(), nullptr, IID_PPV_ARGS(m_CommandListBegin.GetAddressOf()));
	m_CommandListBegin->Close();
	m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator.Get(), nullptr, IID_PPV_ARGS(m_CommandListEnd.GetAddressOf()));
	m_CommandListEnd->Close();
}

void RendererSystem::Render()
{
	WaitForSingleObject(m_SyncEvent, INFINITE);
	m_Fence->Signal(0);
	
	std::vector<ID3D12CommandList*> cmds;	
	ComPtr<ID3D12Resource> rtvResource = DX12Graphics::Instance->GetCurrentRtvResource();
	m_CommandAllocator->Reset();
	
	//CommandBegin
	m_CommandListBegin->Reset(m_CommandAllocator.Get(), nullptr);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = DX12Graphics::Instance->GetCurrentRtvDescriptor();
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = DX12Graphics::Instance->GetDsvDescriptor();

	D3D12_RESOURCE_BARRIER rtvBarrier = {};
	rtvBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	rtvBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	rtvBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	rtvBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	rtvBarrier.Transition.Subresource = 0;
	rtvBarrier.Transition.pResource = rtvResource.Get();
	m_CommandListBegin->ResourceBarrier(1, &rtvBarrier);

	
	m_CommandListBegin->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	D3D12_VIEWPORT viewPort = { 0,0,DX12Graphics::Instance->GetGraphicsWidth(),DX12Graphics::Instance->GetGraphicsHeight(),0,1 };
	m_CommandListBegin->RSSetViewports(1, &viewPort);
	D3D12_RECT sissorRect = { 0,0,DX12Graphics::Instance->GetGraphicsWidth(),DX12Graphics::Instance->GetGraphicsHeight() };
	m_CommandListBegin->RSSetScissorRects(1, &sissorRect);
	m_CommandListBegin->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

	const float clearColor[4] = { 0.5f,0.5f,0.5f,0.0f };
	m_CommandListBegin->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	m_CommandListBegin->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1, 0, 0, nullptr);
	m_CommandListBegin->Close();

	cmds.emplace_back(m_CommandListBegin.Get());

	//场景中可以渲染的物体 调用每个drawable接口的Draw函数 获得CommandList
	auto objs = Scene::Default->GetRootGameObjects();
	std::vector<Drawable*> drawables;
	for(auto p = objs.begin();p != objs.end();p++)
	{
		std::vector<Drawable*> rets = (*p)->GetComponentsInChildren<Drawable>();
		for(auto q = rets.begin();q != rets.end();q++)
		{
			cmds.emplace_back((*q)->Draw().Get());
		}
	}
	
	
	//CommandEnd
	m_CommandListEnd->Reset(m_CommandAllocator.Get(), nullptr);

	D3D12_RESOURCE_BARRIER presentBarrier = {};
	presentBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	presentBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	presentBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	presentBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	presentBarrier.Transition.Subresource = 0;
	presentBarrier.Transition.pResource = rtvResource.Get();
	m_CommandListEnd->ResourceBarrier(1, &presentBarrier);
	m_CommandListEnd->Close();
	cmds.emplace_back(m_CommandListEnd.Get());
	
	m_CommandQueue->ExecuteCommandLists(cmds.size(), (ID3D12CommandList**)&cmds[0]);
	m_Fence->SetEventOnCompletion(1, m_SyncEvent);
	m_CommandQueue->Signal(m_Fence.Get(), 1);
	m_Swapchain->Present(1, 0);
}

