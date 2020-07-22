#pragma once

#ifndef  __INPUT	//This is useless, but it doesn't work without it
#define __INPUT
#include "InputState.h"
#include "GLFW/glfw3.h"                           // for GLFWwindow, GLFW_REPEAT
#include "LuxEngine/macros.h"                     // for scast
#include "LuxEngine/Types/Containers/LuxArray.h"  // for lux::Array
#include "LuxEngine/Types/Integers/Integers.h"    // for uint16
#include "stdio.h"                                // for printf
#include "LuxEngine/Engine/Engine.h"
#include "KeyboardInput.h"



namespace lux::input {
	extern InputState* inStates;		//The current input state
	inline void setInputState(InputState* inputState);

	void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	void mouseAxisCallback(GLFWwindow* window, double x, double y);
	void mouseCursorPosCallback(GLFWwindow* window, double x, double y);
	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
}



#endif //! __INPUT
