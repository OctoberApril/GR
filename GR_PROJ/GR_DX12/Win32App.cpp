#include "Win32App.h"

#include <cassert>
#include "Input.h"
#include "Graphics.h"
#include <iostream>


Win32App* Win32App::Instance = nullptr;

Win32App::Win32App(std::string window_name, int width, int height)
	:m_WindowTitle(std::move(window_name)),
	m_iWidth(width), m_iHeight(height), m_bExit(false)
{
	assert(Instance == nullptr);
	Instance = this;
}

bool Win32App::Initialize()
{
	try
	{
		WNDCLASSA wndClass = {};
		wndClass.hCursor = GetCursor();
		wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wndClass.lpszClassName = "H_DX12";
		wndClass.style = CS_HREDRAW | CS_VREDRAW;
		wndClass.lpfnWndProc = WinProc;

		RegisterClassA(&wndClass);

		m_Hwnd = CreateWindowA("H_DX12", m_WindowTitle.c_str(), WS_OVERLAPPEDWINDOW, 100, 100, m_iWidth, m_iHeight, nullptr, nullptr, nullptr, nullptr);
		SetWindowLongPtr(m_Hwnd, GWLP_USERDATA, (LONG)this);

		UpdateWindow(m_Hwnd);
		ShowWindow(m_Hwnd, SW_NORMAL);
	}
	catch (...)
	{
		return false;
	}
	return true;
}

void Win32App::Update()
{
	MSG msg = {};
	while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void Win32App::LateUpdate()
{
	
}

bool Win32App::Exit() const
{
	return m_bExit;
}


LRESULT Win32App::WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (!Instance) 
		return DefWindowProcA(hwnd, message, wParam, lParam);

	switch (message)
	{
	case WM_DESTROY:
		Instance->m_bExit = true;
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
			
		Input::Instance->SetKeyStatus((KeyCode::Key)vkeyCode, isPressed);

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

		Input::Instance->SetKeyStatus(KeyCode::LButton, lButton);
		Input::Instance->SetKeyStatus(KeyCode::RButton, rButton);
		Input::Instance->SetKeyStatus(KeyCode::MButton, mButton);

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

		Input::Instance->SetMousePosition(x, y);

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
		Input::Instance->SetScrollWheelStatus(deleteMove);

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

