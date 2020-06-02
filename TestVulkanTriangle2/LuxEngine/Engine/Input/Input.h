#pragma once

#ifndef  __INPUT
#define __INPUT


#include "LuxEngine/Engine/Engine.h"
#include "InputState.h"



static Engine* __lp_input_state_engine_ptr;
static void __lp_input_state_set_engine_ptr(Engine* enginePtr) { __lp_input_state_engine_ptr = __lp_input_state_engine_ptr; }




static void __lp_mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	printf("mouse");
}



static void __lp_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

}

#endif // ! __INPUT
