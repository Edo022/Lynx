#pragma once

#include <GLFW/glfw3.h>
#include "MouseInput.h"
#include "KeyboardInput.h"
#include "LuxEngine/Types/Containers/LuxDynArray.h"
#include <initializer_list>


enum LuxKeyState : uint16 {
	LUX_RELEASE = 1 << 15,
	LUX_PRESS = 1 << 14,
	LUX_REPEAT = 1 << 13
};



//TODO add max size check

typedef void (*LuxKeyBindingCallback)(LuxArray<uint16>);
//This struct contains a sequence of keys and a function to call when the sequence is performed
//The sequence is saved as an array of uint16. The action and the key are in the same varibale for better performances.
//The maximum size for the key sequence is 128
//   -     |-----|Unused
//   1 0 0 0 0 0 0 1 0 0 0 0 0 0 1 1   
//   |---|Action   |---------------|Key 
struct LuxKeyBinding {
	//Each element represents a key and its action
	//Example for the sequence "ctrl + k" : {LUX_KEY_LEFT_CTRL | LUX_PRESS, LUX_KEY_K | LUX_PRESS, LUX_KEY_K | LUX_RELEASE}
	LuxArray<uint16> keySequence;
	//This is the function that will be called when the sequence is performed. It must be of type void and take the key sequence as a parameter
	LuxKeyBindingCallback bindedFunction;
};




struct LuxInputState {
	LuxArray<LuxKeyBinding> bindings;
	bool sorted = false;

	LuxInputState(std::initializer_list<LuxKeyBinding> c) {
		bindings = c;
	}


	void sort() {
		if (!sorted) {
			sorted = true;
			for (int i = 0; i < bindings.size(); i++) { //For every sequence
				//check sequence
				for (int j = i; j < bindings.size(); j++) { //For every key binding
					if (j == i) continue;	//Skip useless iterations
					for (int k = 0; k < min(bindings[j].keySequence.__lp_size, bindings.__lp_data[i].keySequence.__lp_size);) {//For every key
						if (bindings.__lp_data[j].keySequence.__lp_data[k] == bindings.__lp_data[i].keySequence.__lp_data[k]) { //If the keys are the same
							k++;															//Check the next key
							continue;
						}
						if (bindings.__lp_data[j].keySequence.__lp_data[k] < bindings.__lp_data[i].keySequence.__lp_data[k]) {
							LuxKeyBinding b = bindings.__lp_data[i];
							bindings.__lp_data[i] = bindings.__lp_data[j];
							bindings.__lp_data[j] = b;
						}
						break;
					}
				}
			}
		}
	}
};