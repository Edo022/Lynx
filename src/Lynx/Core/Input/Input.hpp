#pragma once
////#define LNX_H_INPUT
#include "Lynx/Core/Input/InputState.hpp"
#include "Lynx/Utils.hpp"
#include "Lynx/Core/Input/KeyboardInput.hpp"




namespace lnx::input {
	// extern InputState* inStates;		//The current input state
	// __attribute__((deprecated)) inline void setInputState(InputState* inputState) { inStates = inputState; inStates->sort(); }

	//Converts a glfw action to a lnx key state
	// __attribute__((deprecated)) static inline constexpr uint16 __lp_to_lnx_act(int32 glfwAction) { return (uint16)1 << (16 - (glfwAction)-1); }

	void onClick(GLFWwindow* window, int32 button,				int32 action, int32 mods);
	void onEnter(GLFWwindow* window, int32 entered);
	void onMove (GLFWwindow* window, float64 x, float64 y);
	void onAxis (GLFWwindow* window, float64 x, float64 y);
	__attribute__((deprecated)) void onKey  (GLFWwindow* window, int32 key, int32 scancode,	int32 action, int32 mods);
}
