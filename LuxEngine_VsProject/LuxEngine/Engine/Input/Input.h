#pragma once

#ifndef  __INPUT	//This is useless, but it doesn't work without it
#define __INPUT
#include "LuxEngine/Engine/Engine.h"
#include "InputState.h"
#include "LuxEngine/macros.h"                     // for scast
#include "KeyboardInput.h"



namespace lux::input {
	extern InputState* inStates;		//The current input state
	inline void setInputState(InputState* inputState){ inStates = inputState; inStates->sort( ); };

	//Converts a glfw action to a lux key state
	static inline constexpr uint16 __lp_to_lux_act(int glfwAction) { return (uint16)1 << (16 - (glfwAction)-1); }

	void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	void mouseAxisCallback(GLFWwindow* window, double x, double y);
	void mouseCursorPosCallback(GLFWwindow* window, double x, double y);
	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
}



#endif //! __INPUT
