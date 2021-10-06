#pragma once

#define NOMINMAX
#include <string>
#include <Windows.h>

class DX12Graphics;
class Input;

class Win32App
{
public:
	Win32App(std::string window_name, int width, int height);

	bool Initialize();
	void Update();
	void LateUpdate();

	bool Exit() const;
		
	static LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	static Win32App* Instance;
private:
	
	int m_iHeight;
	int m_iWidth;	
	bool m_bExit;	
	HWND m_Hwnd;
	std::string m_WindowTitle;
};

