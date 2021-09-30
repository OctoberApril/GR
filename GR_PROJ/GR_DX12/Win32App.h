#pragma once

#define NOMINMAX
#include <string>
#include <utility>
#include <Windows.h>

class DX12Graphics;
class Input;

class Win32App
{
public:
	Win32App(std::string window_name, int width = 1600, int height = 800);

	bool Initialize();
	void Update();
	Input* GetInputContext() const;
	DX12Graphics* GetGraphicsContext() const;
	
	static LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:

	DX12Graphics* m_GraphicsContext;
	Input* m_InputContext;

	std::string m_WindowTitle;
	int m_Height;
	int m_Width;
	HWND m_Hwnd;
	static Win32App* _win32App;
};

