#pragma once

#include "KeyCode.h"
#include "float2.h"

class Input
{
public:
	Input();
	Input(const Input&) = delete;
	Input(Input&&) = delete;
	Input& operator=(const Input&) = delete;

	void Initialize();
	void Update();
	void LateUpdate();
	static Input* Instance;
	
	bool GetKey(KeyCode::Key keyCode);
	bool GetKeyDown(KeyCode::Key keyCode);
	bool GetKeyUp(KeyCode::Key keyCode);

	bool GetMouseButton(int);
	bool GetMouseButtonDown(int);
	bool GetMouseButtonUp(int);

	float2 GetMousePosition();
	float GetScrollWheel();

	void SetMousePosition(float x,float y);
	void SetKeyStatus(int keyCode, bool isPress);
	void SetScrollWheelStatus(float deltaMove);
private:

	bool m_KeyMap[255]; //记录当前按键的状态  false:抬起状态 true:按下状态
	float2 m_MousePosition;
	int m_ScrollWheelSign;

	const float SCROLL_WHEEL_DELTA = 0.1f;
};

