#pragma once
#define LUX_H_INPUT_STATE
#include "GLFW/glfw3.h"
#include <initializer_list>
#include "LuxEngine/macros.hpp"
#include "LuxEngine/Types/Containers/RtArray.hpp"
#include "LuxEngine/Types/Integers/Integers.hpp"





namespace lux::input{
	enum LuxKeyState : uint16 {
		LUX_RELEASE = 1 << 15,
		LUX_PRESS = 1 << 14,
		LUX_REPEAT = 1 << 13
	};




	typedef void (*LuxKeyBindingCallback)(lux::RtArray<uint16>);
	//This struct contains a sequence of keys and a function to call when the sequence is performed
	//The sequence is saved as an array of uint16. The action and the key are in the same varibale for better performances.
	//There is no limit to the length of a key sequence
	//   -     |-----|Unused
	//   1 0 0 0 0 0 0 1 0 0 0 0 0 0 1 1
	//   |---|Action   |---------------|Key
	struct KeySequence {
		//The actual sequence of keys. Each element represents a key and its action
		//e.g. "ctrl + k" = { LUX_KEY_LEFT_CTRL | LUX_PRESS, LUX_KEY_K | LUX_PRESS }
		lux::RtArray<uint16> sequence;
		//This is the function that will be called when the sequence is performed. It must be of type void and take a KeySequence as a parameter
		//TODO use ExecFuncData
		LuxKeyBindingCallback bindedFunction = nullptr;
	};




	struct InputState {
		lux::RtArray<KeySequence, uint16> sequences;		//The sequences of keys
		bool sorted = false;					//Whether the sequence is sorted or not

		//TODO use ExecFuncData
		//Initializes the input state with a list of key sequences and sorts them
		InputState(std::initializer_list<KeySequence> c) {
			sequences = c;
			sort();
		}

		void sort();
	};
}