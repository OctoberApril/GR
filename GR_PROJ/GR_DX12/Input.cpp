#include <exception>

#include "Input.h"
#include "assert.h"

Input* Input::Instance = nullptr;

Input::Input()
{
	assert(Instance == nullptr);
	Instance = this;
	for (int i = 0; i < 255; i++) m_KeyMap[i] = false;
}

void Input::Initialize()
{
	
}

void Input::Update()
{
	
}

void Input::LateUpdate()
{
	m_ScrollWheelSign = 0;
}

bool Input::GetKey(KeyCode::Key keyCode)
{
	//return true while user hold down the key
	return m_KeyMap[keyCode];
}

bool Input::GetKeyDown(KeyCode::Key keyCode)
{
	return m_KeyMap[keyCode];
}

bool Input::GetKeyUp(KeyCode::Key keyCode)
{
	return !m_KeyMap[keyCode];
}

bool Input::GetMouseButton(int mouseKey)
{
	if (mouseKey == 0) //left
		return m_KeyMap[KeyCode::LButton];
	else if (mouseKey == 1) //right
		return m_KeyMap[KeyCode::RButton];
	else if (mouseKey == 2)
		return m_KeyMap[KeyCode::MButton];
	else
		throw std::exception("Argument Error.");
}

bool Input::GetMouseButtonDown(int mouseKey)
{
	if (mouseKey == 0) //left
		return m_KeyMap[KeyCode::LButton];
	else if (mouseKey == 1) //right
		return m_KeyMap[KeyCode::RButton];
	else if (mouseKey == 2)
		return m_KeyMap[KeyCode::MButton];
	else
		throw std::exception("Argument Error.");
}

bool Input::GetMouseButtonUp(int mouseKey)
{
	if (mouseKey == 0) //left
		return !m_KeyMap[KeyCode::LButton];
	else if (mouseKey == 1) //right
		return !m_KeyMap[KeyCode::RButton];
	else if (mouseKey == 2)
		return !m_KeyMap[KeyCode::MButton];
	else
		throw std::exception("Argument Error.");
}

float2 Input::GetMousePosition()
{
	return m_MousePosition;
}

float Input::GetScrollWheel()
{
	return m_ScrollWheelSign * SCROLL_WHEEL_DELTA;
}

void Input::SetMousePosition(float x, float y)
{
	m_MousePosition.x = x;
	m_MousePosition.y = y;
}

void Input::SetKeyStatus(int keyCode, bool isPress)
{
	int key = keyCode & 0xff;
	assert(key != 255);
	m_KeyMap[key] = isPress;
}

void Input::SetScrollWheelStatus(float delteMove)
{
	m_ScrollWheelSign = delteMove > 0 ? 1 : -1;
}
