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
	LuxArray<LuxKeySequence> sequences;		//The sequences of keys
	bool sorted = false;					//Whether the sequence is sorted or not

	//Initializes the input state with a list of key sequences and sorts them
	LuxInputState(std::initializer_list<LuxKeySequence> c) {
		sequences = c;
		sort();
	}

	//Sorts the sequences
	void sort() {																					//If the sequence is not sorted
		if (!sorted) {
			sorted = true;
			for (int i = 0; i < sequences.size(); i++) {												//For every sequence
				for (int j = i; j < sequences.size(); j++) {												//Checking every other sequence that comes after it
					if (j == i) continue;																		//Skip useless iterations
					for (int k = 0; k < min(sequences.__lp_data[j].sequence.__lp_size, sequences.__lp_data[i].sequence.__lp_size);) {//For every key of the sequence
						if (sequences.__lp_data[j].sequence[k] == sequences.__lp_data[i].sequence[k]) {									//Compare the 2 keys at the same index of the sequences. If they're equals
							k++;																						//Increase the key counter
							continue;																					//And check the next key
						}
						if (sequences.__lp_data[j].sequence.__lp_data[k] < sequences.__lp_data[i].sequence[k]) {							//If the first is greater than the second
							LuxKeySequence b = sequences.__lp_data[i];																//Swap the whole sequence
							sequences.__lp_data[i] = sequences.__lp_data[j];
							sequences.__lp_data[j] = b;
							break;																						//Exit the loop
						}
					}
				}
			}
		}
	}
};