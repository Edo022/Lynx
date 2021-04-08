#pragma once
#define LNX_H_INPUT
#include "Lynx/Core/Input/InputState.hpp"
#include "Lynx/macros.hpp"
#include "Lynx/Core/Input/KeyboardInput.hpp"




namespace lnx::input {
	extern InputState* inStates;		//The current input state
	inline void setInputState(InputState* inputState) { inStates = inputState; inStates->sort(); }

	//Converts a glfw action to a lnx key state
	static inline constexpr uint16 __lp_to_lnx_act(int32 glfwAction) { return (uint16)1 << (16 - (glfwAction)-1); }

	void mouseAxisCallback(			GLFWwindow* window, float64 x, float64 y);
	void mouseCursorPosCallback(	GLFWwindow* window, float64 x, float64 y);
	void mouseButtonCallback(		GLFWwindow* window, int32 button,				int32 action, int32 mods);
	void keyCallback(				GLFWwindow* window, int32 key, int32 scancode,	int32 action, int32 mods);
}
