#pragma once

#ifndef  __INPUT
#define __INPUT
#include "LuxEngine/Engine/Engine.h"
#include "InputState.h"



#define __lp_to_lux_act(glfwAction) ((uint16)1 << (16 - glfwAction - 1))
//#define __lp_to_lux_act(glfwAction) ((action == GLFW_PRESS) ? LUX_PRESS : ((action == GLFW_RELEASE) ? LUX_RELEASE : LUX_REPEAT))

class Engine;
extern Engine* __lp_input_engine_ptr;			//? TODO
extern LuxInputState* __lp_input_states;		//The current input state
//extern LuxArray<uint16> __lp_input_key_queue;	//The list of keys and actions since the last performed key sequence
extern uint16 inputKeysNum;

inline static void __lp_input_set_engine_ptr(Engine* enginePtr) { __lp_input_engine_ptr = enginePtr; }
inline static void luxInputSetInputState(LuxInputState* inputState) { __lp_input_states = inputState; }




static void __lp_mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	printf("mouse");
}




static void __lp_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	static LuxArray<uint16> inputKeyList(100);		//The list of keys and actions since the last performed key sequence
	//static uint16 inputKeysNum = 0;					//The size of the current key list
	uint16 abc = (key | __lp_to_lux_act(action));

	if (action != GLFW_REPEAT) {
		inputKeyList[inputKeysNum] = (key | __lp_to_lux_act(action));
		inputKeysNum++; //TODO keys num resets
	}

	//Per ogni sequenza
	for (uint16 bindingIndex = 0; bindingIndex < __lp_input_states->size(); bindingIndex++) {
		//Per ogni tasto della sequenza di input
		//TODO greater value check
		for (uint16 keyIndex = 0; keyIndex < inputKeysNum; keyIndex++) {
			//Se è diverso da quello della sequenza che si sta controllando, esci dal for
			if (inputKeyList[keyIndex] != (*__lp_input_states)[bindingIndex].code[keyIndex]) break;
			//se le sequenze sono uguali, esegui la funzione e resetta l'input
			else if (keyIndex == __lp_input_states->__lp_data[bindingIndex].code.__lp_size - 1) {
				inputKeysNum = 0;
				(*__lp_input_states)[bindingIndex].bindingCallback((*__lp_input_states)[bindingIndex].code);
				goto exit_;
			}
		}
		//Se non si è trovato nessuna sequenza uguale
		if (bindingIndex == __lp_input_states->size() - 1) {
			//__noop;
			int a = 0;
			//inputKeysNum = 0;//TODO keys num resets
		}
	}
	exit_:
	//__noop;
	int a = 0;

	//switch (action) {
	//	case GLFW_PRESS:  if ((key | LUX_PRESS) == (*__lp_input_states)[0].code[0]){
	//	case GLFW_RELEASE: if ((key | LUX_RELEASE) == (*__lp_input_states)[0].code[0]){
	//	case GLFW_REPEAT: if ((key | LUX_REPEAT) == (*__lp_input_states)[0].code[0]){
	//if ((key | __lp_to_lux_act(action)) == (*__lp_input_states)[0].code[0]) {
	//	(*__lp_input_states)[0].bindingCallback((*__lp_input_states)[0].code);
	//}
}



#endif // ! __INPUT
