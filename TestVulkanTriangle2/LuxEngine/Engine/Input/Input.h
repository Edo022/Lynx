#pragma once

#ifndef  __INPUT
#define __INPUT
#include "LuxEngine/Engine/Engine.h"
#include "InputState.h"



#define __lp_to_lux_act(glfwAction) (((uint16)1) << (16 - glfwAction - 1))
//#define __lp_to_lux_act(glfwAction) ((action == GLFW_PRESS) ? LUX_PRESS : ((action == GLFW_RELEASE) ? LUX_RELEASE : LUX_REPEAT))

class Engine;
extern Engine* __lp_input_engine_ptr;			//? TODO
extern LuxInputState* __lp_input_states;		//The current input state
extern LuxArray<uint16> __lp_input_key_queue;	//The list of keys and actions since the last performed key sequence
extern uint32 __lp_input_key_queue_size;

inline static void __lp_input_set_engine_ptr(Engine* enginePtr) { __lp_input_engine_ptr = enginePtr; }
inline static void luxInputSetInputState(LuxInputState* inputState) { __lp_input_states = inputState; }




static void __lp_mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	printf("mouse");
}

static void __lp_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	__lp_input_key_queue[__lp_input_key_queue_size] = (key | __lp_to_lux_act(action));
	__lp_input_key_queue_size++;
	for (int loopCurrentKey = 0; loopCurrentKey < __lp_input_key_queue_size; loopCurrentKey++) {
		uint16 abc = (key | __lp_to_lux_act(action));
		if (__lp_input_key_queue[loopCurrentKey] != (*__lp_input_states)[0].code[loopCurrentKey]) break;
		else if (loopCurrentKey == (*__lp_input_states).__lp_data[0].code.__lp_size - 1) {
			__lp_input_key_queue_size = 0;
			(*__lp_input_states)[0].bindingCallback((*__lp_input_states)[0].code);
		}
	}
	//switch (action) {
	//	case GLFW_PRESS:  if ((key | LUX_PRESS) == (*__lp_input_states)[0].code[0]){
	//	case GLFW_RELEASE: if ((key | LUX_RELEASE) == (*__lp_input_states)[0].code[0]){
	//	case GLFW_REPEAT: if ((key | LUX_REPEAT) == (*__lp_input_states)[0].code[0]){
	//if ((key | __lp_to_lux_act(action)) == (*__lp_input_states)[0].code[0]) {
	//	(*__lp_input_states)[0].bindingCallback((*__lp_input_states)[0].code);
	//}
}



#endif // ! __INPUT
