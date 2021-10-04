#include "Win32App.h"
#include <iostream>

int g_iWidth = 1600;
int g_iHeight = 800;

Input* g_Input;

void main()
{
	Win32App app("799 Scene");
	if(!app.Initialize())
	{
		std::cout << "app Initialze Failed" << std::endl;
	}
	g_Input = app.GetInputContext();
	
	app.Update();
}