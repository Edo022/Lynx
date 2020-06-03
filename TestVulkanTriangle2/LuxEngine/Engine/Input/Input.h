#pragma once

#ifndef  __INPUT
#define __INPUT
#include "LuxEngine/Engine/Engine.h"
#include "InputState.h"




class Engine;
extern Engine* __lp_input_engine_ptr;
extern LuxInputState* __lp_input_state; //TODO make pointer

inline static void __lp_input_set_engine_ptr(Engine* enginePtr) { __lp_input_engine_ptr = enginePtr; }
inline static void luxInputSetInputState(LuxInputState* inputState) { __lp_input_state = inputState; }




static void __lp_mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	printf("mouse");
}

static void __lp_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if ((key | ((action == GLFW_PRESS) ? LUX_PRESS : LUX_RELEASE)) == (*__lp_input_state)[0].code[0]) printf("key");
}



#endif // ! __INPUT
