#include "MeshRenderer.h"


void MeshRenderer::Draw() const
{
	//整体渲染前准备
	// --将BackBuffer 转变成 RenderTarget 资源类型
	// -- ClearRenderTargetView
	// -- ClearDepthStencilView
	// -- CreateFence

	// 场景中各个物体将渲染Commandlist添加进入CommandQueue

	// -- CommandQueue 开始执行渲染Queue 直到执行完毕Present当前的缓冲区
	
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
}
