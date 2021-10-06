#pragma once
#include <memory>


class Win32App;
class DX12Graphics;
class Input;
class RendererSystem;

class Application
{
public:
	Application(const char* app_name = "OctApp", int w = 1600, int h = 800) :m_AppName(app_name), m_iWidth(w), m_iHeight(h) {}
	~Application() = default;
	Application(const Application&) = delete;
	Application(Application&&) = delete;
	Application& operator=(const Application&) = delete;
	Application& operator=(Application&&) = delete;

public:
	void Run();

private:	
	void Initialize();

protected:
	
	/// Setup before engine initialization.
	virtual void Setup();

	/// Setup after engine initialization.
	virtual void Start();

	/// Cleanup after the main loop. Called by Application.
	virtual void Stop();

private:
	template<typename T> using SharedPtr = std::shared_ptr<T>;


	SharedPtr<Win32App> m_Win32Context;
	SharedPtr<DX12Graphics> m_DX12GraphicsContext;
	SharedPtr<Input> m_InputContext;
	SharedPtr<RendererSystem> m_RendererContext;
	
	const char* m_AppName;
	int m_iWidth;
	int m_iHeight;
};

