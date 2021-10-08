#include "Application.h"
#include "Win32App.h"
#include "Graphics.h"
#include "Input.h"
#include "RendererSystem.h"

void Application::Initialize()
{
	m_Win32Context = std::make_shared<Win32App>(m_AppName,m_iWidth,m_iHeight);
	m_DX12GraphicsContext = std::make_shared<DX12Graphics>(m_iWidth, m_iHeight);
	m_InputContext = std::make_shared<Input>();
	m_RendererContext = std::make_shared<RendererSystem>();
	
	m_Win32Context->Initialize();
	m_DX12GraphicsContext->Initialize();
	m_InputContext->Initialize();
	m_RendererContext->Initialize();
}

void Application::Run()
{
	Setup();
	Initialize();
	Start();

	while(!m_Win32Context->Exit())
	{
		m_Win32Context->Update();
		m_DX12GraphicsContext->Update();
		m_InputContext->Update();

		m_RendererContext->Render();
		
		m_Win32Context->LateUpdate();
		m_DX12GraphicsContext->LateUpdate();
		m_InputContext->LateUpdate();
	}
	
	Stop();
}

void Application::Setup()
{
	
}

void Application::Start()
{
	
}

void Application::Stop()
{
	
}





