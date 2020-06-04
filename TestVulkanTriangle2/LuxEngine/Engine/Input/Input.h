#pragma once

#ifndef  __INPUT
#define __INPUT
#include "LuxEngine/Engine/Engine.h"
#include "InputState.h"

//TODO use [] and .size() instead of __lp_ values
//TODO [] and .size() generates exception

#define __lp_to_lux_act(glfwAction) ((uint16)1 << (16 - glfwAction - 1))

class Engine;
extern Engine* __lp_input_engine_ptr;			//? TODO
extern LuxInputState* __lp_input_states;		//The current input state
extern uint16 maxInputStateSequenceLength;		//The length of the longest sequence in the input state


inline static void __lp_input_set_engine_ptr(Engine* enginePtr) { __lp_input_engine_ptr = enginePtr; }

inline static void luxInputSetInputState(LuxInputState* inputState) { 
	__lp_input_states = inputState; 

	//Calculate and save the maximum sequence length
	maxInputStateSequenceLength = 0;
	for (int i = 0; i < __lp_input_states->bindings.size(); i++) {
		if (__lp_input_states->bindings.__lp_data[0].sequence.__lp_size > maxInputStateSequenceLength) 
			maxInputStateSequenceLength = __lp_input_states->bindings.__lp_data[0].sequence.__lp_size;
	}

	//Sort sequences
	__lp_input_states->sort();
}







static void __lp_mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	printf("mouse");
}



//This function manages the input from the keyboard and calls the functions binded to the input state's key bindings
static void __lp_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	static LuxArray<uint16> inputKeyList(100);		//The list of keys and actions since the last performed key sequence
	static uint16 inputKeysNum = 0;					//The size of the current key list
	uint16 maxTestedInputStateSequenceLength = 0;
	uint16 abc = (key | __lp_to_lux_act(action)); //TODO remove


	//TODO ordered search
	if (action != GLFW_REPEAT) {																			//If the action is not repeat
		inputKeyList[inputKeysNum] = (key | __lp_to_lux_act(action));											//Add the key code to the input sequence
		inputKeysNum++;																							//Update the number of input keys

		for (uint16 bindingIndex = 0; bindingIndex < __lp_input_states->bindings.size(); bindingIndex++) {				//For every input state's sequence
			for (uint16 keyIndex = 0; keyIndex < inputKeysNum; keyIndex++) {										//For every key of the input sequence	//No need to check for (inputKeysNum < code.__lp_size). In that case, nothing in the for loop will be executed
				if (inputKeyList[keyIndex] != __lp_input_states->bindings[bindingIndex].sequence[keyIndex]) break;		//If it's different than the key of the sequence of the input state, exit the loop and try with the next sequence
				else if (keyIndex == __lp_input_states->bindings[bindingIndex].sequence.__lp_size - 1) {		//If the sequence are equals
					inputKeysNum = 0;																						//Reset the input sequence
					__lp_input_states->bindings[bindingIndex].bindedFunction(__lp_input_states->bindings[bindingIndex].sequence);	//Execute the binded function
					return;																									//Exit the function
				}
			}
			if (bindingIndex == __lp_input_states->bindings.size() - 1 && inputKeysNum >= maxInputStateSequenceLength) {		//If there are no sequences that match the input (The current binding is the last binding and the input size is greater or equal to the maximum size of a sequence of the input state)
				inputKeysNum = 0;																						//Reset the input sequence
				return;																									//Exit the function
			}
		}
	}
}




#endif // ! __INPUT
