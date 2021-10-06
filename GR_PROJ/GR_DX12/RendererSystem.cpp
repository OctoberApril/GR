#include "RendererSystem.h"
#include <cassert>
#include "Graphics.h"
#include "Scene.h"


RendererSystem::RendererSystem()
{
	m_Device = DX12Graphics::Instance->GetDevice();
	assert(m_Device != nullptr);

	m_Device->CreateFence(1, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_Fence.GetAddressOf()));
	m_SyncEvent = CreateEvent(nullptr, false, true, nullptr);
}

void RendererSystem::Render()
{
	//分析场景图 对实现Drawable接口的Componnent调用绘制
	//整体渲染前准备
	// --将BackBuffer 转变成 RenderTarget 资源类型
	// -- ClearRenderTargetView
	// -- ClearDepthStencilView
	// -- CreateFence

	// 场景中各个物体将渲染Commandlist添加进入CommandQueue

	// -- CommandQueue 开始执行渲染Queue 直到执行完毕Present当前的缓冲区

	//{
	//create commandallocator
	//create commandlist

	//获取当前场景的摄像机 创建viewport
	//
	//IASetVetexBuffer
	//IASetIndexBuffer
	//SetGraphicsRootSignature
	//SetPipelineState
	//SetPrimitiveTopology
	//SetRenderTargetView
	//}
}

