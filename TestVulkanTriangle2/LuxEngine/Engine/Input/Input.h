#pragma once

#ifndef  __INPUT
#define __INPUT
#include "LuxEngine/Engine/Engine.h"
#include "InputState.h"




class Engine;
extern Engine* __lp_input_engine_ptr;			//? TODO
extern LuxInputState* __lp_input_states;		//The current input state


inline static void __lp_input_set_engine_ptr(Engine* enginePtr) { 
	__lp_input_engine_ptr = enginePtr; 
}

inline static void luxInputSetInputState(LuxInputState* inputState) { 
	__lp_input_states = inputState;		//Set input state
	__lp_input_states->sort();			//Sort sequences
}







static void __lp_mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	printf("mouse");
}



//This function manages the input from the keyboard and calls the functions binded to the input state's key bindings
static void __lp_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	static uint16 yMin = 0, yMax = __lp_input_states->bindings.__lp_size - 1, x = 0;
	#define __lp_to_lux_act(glfwAction) ((uint16)1 << (16 - glfwAction - 1))
	
	if (action != GLFW_REPEAT) {																			//If the action is not repeat
		uint16 keyCode = (key | __lp_to_lux_act(action));
		int i = yMin;
		while (keyCode != __lp_input_states->bindings.__lp_data[i].sequence.__lp_data[x]) {		//Find the minimum y. 
			i++;																				//Increase the counter until the input key is equal to the saved binding's key
			if (i > yMax) {		//If there are no equal keys
				x = 0;//Reset the key counter
				return; //exit the function
			}
		}
		yMin = i;//If there is an equal key, set the minimum y

		while (true){
			if (keyCode == __lp_input_states->bindings.__lp_data[i].sequence.__lp_data[x]) {		//Find the maximum y 
				if (i >= __lp_input_states->bindings.__lp_data[i].sequence.__lp_size) break;
				i++;																				//Increase the counter until the input key is not equal to the saved binding's key
			}
			else {
				i--;
				break;
			}
		}
		yMax = i;//If there is an equal key, set the maximum y

		if (yMin == yMax) {
			__lp_input_states->bindings.__lp_data[i].bindedFunction(__lp_input_states->bindings[i].sequence); //If the maximum and minimum y are the same, the binding is found
			yMin = 0;
			yMax = __lp_input_states->bindings.__lp_size - 1;
			x = 0;
			return;
		}
		x++;																							//Update the number of input keys
	}
}




#endif // ! __INPUT
