#pragma once
#define LUX_H_INPUT
#include "LuxEngine/Core/Input/InputState.hpp"
#include "LuxEngine/macros.hpp"
#include "LuxEngine/Core/Input/KeyboardInput.hpp"




namespace lux::input {
	extern InputState* inStates;		//The current input state
	inline void setInputState(InputState* inputState) { inStates = inputState; inStates->sort( ); }

	//Converts a glfw action to a lux key state
	static inline constexpr uint16 __lp_to_lux_act(int32 glfwAction) { return (uint16)1 << (16 - (glfwAction)-1); }

	void mouseAxisCallback(			GLFWwindow* window, float64 x, float64 y);
	void mouseCursorPosCallback(	GLFWwindow* window, float64 x, float64 y);
	void mouseButtonCallback(		GLFWwindow* window, int32 button,				int32 action, int32 mods);
	void keyCallback(				GLFWwindow* window, int32 key, int32 scancode,	int32 action, int32 mods);
}
