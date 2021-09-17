﻿// Win32_Message.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>

#include "Graphics.h"
#include "Input.h"


LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);

int g_iWidth = 1600;
int g_iHeight = 1080;

Input* g_Input = nullptr;
DX12Graphics* g_Graphics = nullptr;

int main()
{
	WNDCLASSA wndClass = {};
	wndClass.hCursor = GetCursor();
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClass.lpszClassName = "H_DX12";
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WinProc;

	RegisterClassA(&wndClass);

	HWND hwnd = CreateWindowA("H_DX12", "Win Message", WS_OVERLAPPEDWINDOW, 300, 300, g_iWidth, g_iHeight, nullptr, nullptr, nullptr, nullptr);

	UpdateWindow(hwnd);
	ShowWindow(hwnd, SW_NORMAL);

	g_Input = new Input();
	g_Graphics = new DX12Graphics();

	if (!g_Graphics->Initialize())
		std::cout << "Initialize Graphics Failed." << std::endl;
	else
		std::cout << "Create D3D12 Device Success" << std::endl;

	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		if (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{	
			g_Input->Update();
			g_Graphics->Update();

			g_Input->GraphicsEndUpdate();
		}	
	}
}

LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SYSKEYUP:
	case WM_KEYUP:
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
	{
		bool isPressed = !(HIWORD(lParam) & 0x8000);  //1 released  0 pressed
		bool alt = ((GetAsyncKeyState(VK_MENU) & 0x8000) != 0);
		bool shift = ((GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0);
		bool control = ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0);

		int vkeyCode = wParam;
		//unsigned short scanCode = HIWORD(lParam) & 0xf;
		int scanCode = (HIWORD(lParam) & (KF_EXTENDED | 0xff));
		if (!scanCode)
		{
			// NOTE: Some synthetic key messages have a scancode of zero
			// HACK: Map the virtual key back to a usable scancode
			std::cout << "ScanCode Is NULL" << std::endl;
			scanCode = MapVirtualKeyW((UINT)wParam, MAPVK_VK_TO_VSC);
		}

		g_Input->SetKeyStatus((KeyCode::Key)vkeyCode, isPressed);

#if __DEBUG
		std::cout << "Alt:" << alt << " " << "Control:" << control << " " << "shift:" << shift << std::endl;
		std::cout << "virtual code : " << vkeyCode << std::endl;
		std::cout << "scan code : " << scanCode << std::endl;
		std::cout << "isPressed:" << isPressed << std::endl;
		std::cout << std::endl;
#endif
		break;
	}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	{
		bool control = wParam & MK_CONTROL;
		bool shift = wParam & MK_SHIFT;
		bool lButton = wParam & MK_LBUTTON;
		bool rButton = wParam & MK_RBUTTON;
		bool mButton = wParam & MK_MBUTTON;

		int x = LOWORD(lParam);
		int y = HIWORD(lParam);

		g_Input->SetKeyStatus(KeyCode::LButton, lButton);
		g_Input->SetKeyStatus(KeyCode::RButton, rButton);
		g_Input->SetKeyStatus(KeyCode::MButton, mButton);

#if __DEBUG
		std::cout << "LButton:" << lButton << std::endl;
		std::cout << "rButton:" << rButton << std::endl;
		std::cout << "mButton:" << mButton << std::endl;
#endif
		break;
	}
	case WM_MOUSEMOVE:
	{
		bool control = wParam & MK_CONTROL;
		bool shift = wParam & MK_SHIFT;
		bool lButton = wParam & MK_LBUTTON;
		bool rButton = wParam & MK_RBUTTON;
		bool mButton = wParam & MK_MBUTTON;

		int x = LOWORD(lParam);
		int y = HIWORD(lParam);

		g_Input->SetMousePosition(x, y);

#if __DEBUG
		std::cout << "WM_MOUSEMOVE (" << x << "," << y << ")" << std::endl;
#endif
		break;
	}
	case WM_MOUSEWHEEL:
	{
		bool control = wParam & MK_CONTROL;
		bool shift = wParam & MK_SHIFT;
		bool lButton = wParam & MK_LBUTTON;
		bool rButton = wParam & MK_RBUTTON;
		bool mButton = wParam & MK_MBUTTON;

		int x = LOWORD(lParam);
		int y = HIWORD(lParam);

		short deleteMove = static_cast<short>(HIWORD(wParam)) / WHEEL_DELTA;
		g_Input->SetScrollWheelStatus(deleteMove);

#if __DEBUG
		std::cout << "rotate:" << deleteMove << std::endl;
#endif
		break;
	}
	default:
		return DefWindowProcA(hwnd, message, wParam, lParam);
	}
	return 0;
}

