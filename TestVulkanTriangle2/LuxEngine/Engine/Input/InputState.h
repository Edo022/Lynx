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
struct LuxKeySequence {
	//The actual sequence of keys. Each element represents a key and its action
	//Example for the sequence "ctrl + k" : {LUX_KEY_LEFT_CTRL | LUX_PRESS, LUX_KEY_K | LUX_PRESS, LUX_KEY_K | LUX_RELEASE}
	LuxArray<uint16> sequence;
	//This is the function that will be called when the sequence is performed. It must be of type void and take the key sequence as a parameter
	LuxKeyBindingCallback bindedFunction;
};




struct LuxInputState {
	LuxArray<LuxKeySequence> bindings;
	bool sorted = false;

	LuxInputState(std::initializer_list<LuxKeySequence> c) {
		bindings = c;
	}

	//
	void sort() {																					//If the sequence is not sorted
		if (!sorted) {
			sorted = true;
			for (int i = 0; i < bindings.size(); i++) {													//For every sequence
				for (int j = i; j < bindings.size(); j++) {													//Checking every other sequence that comes after it
					if (j == i) continue;																		//Skip useless iterations
					for (int k = 0; k < min(bindings[j].sequence.__lp_size, bindings[i].sequence.__lp_size);) {	//For every key of the sequence
						if (bindings[j].sequence[k] == bindings[i].sequence[k]) {									//Compare the 2 keys at the same index of the sequences. If they are equals
							k++;																						//Increase the key counter
							continue;																					//And check the next key
						}
						if (bindings[j].sequence.__lp_data[k] < bindings[i].sequence[k]) {							//If the first is greater than the second
							LuxKeySequence b = bindings[i];																//Swap the whole sequence
							bindings[i] = bindings[j];
							bindings[j] = b;
						}
						break;																						//Exit the loop
					}
				}
			}
		}
	}
};