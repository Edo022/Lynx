#pragma once

#include <GLFW/glfw3.h>
#include <initializer_list>
#include "LuxEngine/macros.h"                     // for sc, min
#include "LuxEngine/Types/Containers/LuxArray.h"  // for LuxArray
#include "LuxEngine/Types/Integers/Integers.h"    // for uint16


enum LuxKeyState : uint16 {
	LUX_RELEASE = 1 << 15,
	LUX_PRESS = 1 << 14,
	LUX_REPEAT = 1 << 13
};




typedef void (*LuxKeyBindingCallback)(LuxArray<uint16>);
//This struct contains a sequence of keys and a function to call when the sequence is performed
//The sequence is saved as an array of uint16. The action and the key are in the same varibale for better performances.
//There is no limit to the length of a key sequence
//   -     |-----|Unused
//   1 0 0 0 0 0 0 1 0 0 0 0 0 0 1 1   
//   |---|Action   |---------------|Key 
struct LuxKeySequence {
	//The actual sequence of keys. Each element represents a key and its action
	//e.g. "ctrl + k" = {LUX_KEY_LEFT_CTRL | LUX_PRESS, LUX_KEY_K | LUX_PRESS}
	LuxArray<uint16> sequence;
	//This is the function that will be called when the sequence is performed. It must be of type void and take a KeySequence as a parameter
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
	
	//This function sorts the sequences. You don't have to call it, they'll be sorted when needed.
	//Additional calls to this function does not affect performances
	void __vectorcall sort() {																						
		if (!sorted) {															//If the sequence is not sorted
			sorted = true;															//Set it as sorted
			for (int i = 0; i < sequences.size(); i++) {							//For every key sequence
				for (int j = i; j < sequences.size(); j++) {							//Checking every other sequence that comes after it
					if (j == i) continue;													//Skip useless iterations            // ↓ For every key of the second sequence
					for (int k = 0; k < min(sc<LuxArray<uint16>>(sequences[j].sequence).size(), sc<LuxArray<uint16>>(sequences[i].sequence).size()); k++) {
						if (sequences[j].sequence[k] == sequences[i].sequence[k]) continue;	//Search for the next index with different keys
						if (sequences[j].sequence[k] < sequences[i].sequence[k]) {			//If the first is greater than the second
							LuxKeySequence b = sequences[i];									//Swap the bindings and the whole sequences
							sequences[i] = sequences[j];
							sequences[j] = b;
							break;																//Exit the loop of the second sequence's keys
						}
					}
				}
			}
		}
	}
};