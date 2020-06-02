#pragma once
#include <GLFW/glfw3.h>
#include "LuxEngine/Engine/Input/InputState.h"
#include <stdio.h>



enum LuxMouseButtons {
	LUX_MOUSE_BUTTON_1 = 0,		//Same as LUX_MOUSE_BUTTON_RIGHT
	LUX_MOUSE_BUTTON_2 = 1,		//Same as LUX_MOUSE_BUTTON_LEFT
	LUX_MOUSE_BUTTON_3 = 2,		//Same as LUX_MOUSE_BUTTON_MIDDLE
	LUX_MOUSE_BUTTON_4 = 3,
	LUX_MOUSE_BUTTON_5 = 4,
	LUX_MOUSE_BUTTON_6 = 5,
	LUX_MOUSE_BUTTON_7 = 6,
	LUX_MOUSE_BUTTON_8 = 7,
	LUX_MOUSE_BUTTON_RIGHT = LUX_MOUSE_BUTTON_1,
	LUX_MOUSE_BUTTON_LEFT = LUX_MOUSE_BUTTON_2,
	LUX_MOUSE_BUTTON_MIDDLE = LUX_MOUSE_BUTTON_3
};


static void __lp_mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	printf("mouse");
}

