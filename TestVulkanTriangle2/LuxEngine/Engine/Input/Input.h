#pragma once

#ifndef  __INPUT
#define __INPUT
#include "LuxEngine/Engine/Engine.h"
#include "InputState.h"




class Engine;
extern Engine* __lp_input_engine_ptr;			//? TODO
extern LuxInputState* __lp_input_states;		//The current input state

inline static void __lp_input_set_engine_ptr(Engine* enginePtr) { __lp_input_engine_ptr = enginePtr; }
inline static void luxInputSetInputState(LuxInputState* inputState) { __lp_input_states = inputState; __lp_input_states->sort(); }







static void __lp_mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	printf("mouse");
}


//TODO add key seqeuence tree


//This function manages the input from the keyboard and calls the functions binded to the input state's key bindings
static void __lp_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	static uint16 yMin = 0, yMax = __lp_input_states->sequences.__lp_size - 1, x = 0;
	#define __lp_to_lux_act(glfwAction) ((uint16)1 << (16 - glfwAction - 1))
	
	if (action != GLFW_REPEAT) {															//If the action is not repeat
		uint16 keyCode = (key | __lp_to_lux_act(action));										//Claculate the key code
		int i = yMin;																			//Set the loop index as the minimum y
		while (keyCode != __lp_input_states->sequences[i].sequence[x]) {	//Find the new minimum y
			i++;																					//Increase the counter until the input key is equal to the input state sequence's key
			if (i > yMax) {																			//If there are no equal keys
				x = 0;																					//Reset the key counter
				return;																					//Exit the function
			}
		}
		yMin = i;																				//If there is an equal key, set the minimum y

		while (true){																			//Now find the maximum y 
			if (keyCode == __lp_input_states->sequences[i].sequence[x]) {		//if the input key is NOT equal to the input state sequence's key
				i++;																					//Increase the counter
				if (i >= __lp_input_states->sequences.__lp_size) {										//If there are no more different keys
					yMax = __lp_input_states->sequences.__lp_size - 1;										//Set the maximum y as the maximum index of the sequences
					break;																					//Exit the loop
				}
			}
			else {																						//If there are different keys
				yMax = i - 1;																				//Set the maximum y
				break;																						//Exit the loop
			}
		}

		if (yMin == yMax && x == __lp_input_states->sequences[yMax].sequence.__lp_size -1){
			__lp_input_states->sequences[yMax].bindedFunction(__lp_input_states->sequences[yMax].sequence); //If the maximum and minimum y are the same, the binding is found
			yMin = 0;
			yMax = __lp_input_states->sequences.__lp_size - 1;
			x = 0;
			return;
		}
		x++;																							//Update the number of input keys
	}
}




#endif // ! __INPUT
