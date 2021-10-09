#pragma once
////#define LNX_H_INPUT_STATE
#include <GLFW/glfw3.h>
#include <initializer_list>
#include "Lynx/macros.hpp"
#include "Lynx/Types/Containers/RtArray.hpp"
#include "Lynx/Types/Integers/Integers.hpp"





namespace lnx::input{
	enum LnxKeyState : uint16 {
		eRelease = 1 << 15,
		ePress = 1 << 14,
		eRepeat = 1 << 13
	};




	typedef void (*LnxKeyBindingCallback)(lnx::RtArray<uint16>);
	//This struct contains a sequence of keys and a function to call when the sequence is performed
	//The sequence is saved as an array of uint16. The action and the key are in the same varibale for better performances.
	//There is no limit to the length of a key sequence
	//   -     |-----|Unused
	//   1 0 0 0 0 0 0 1 0 0 0 0 0 0 1 1
	//   |---|Action   |---------------|Key
	struct KeySequence {
		//The actual sequence of keys. Each element represents a key and its action
		//e.g. "ctrl + k" = { LNX_KEY_LEFT_CTRL | LNX_PRESS, LNX_KEY_K | LNX_PRESS }
		lnx::RtArray<uint16> sequence;
		//This is the function that will be called when the sequence is performed. It must be of type void and take a KeySequence as a parameter
		//TODO use ExecFuncData
		LnxKeyBindingCallback bindedFunction = nullptr;
	};




	// struct InputState {
	// 	lnx::RtArray<KeySequence, uint16> sequences;		//The sequences of keys
	// 	bool sorted = false;					//Whether the sequence is sorted or not

	// 	//TODO use ExecFuncData
	// 	//Initializes the input state with a list of key sequences and sorts them
	// 	InputState(std::initializer_list<KeySequence> c) {
	// 		sequences = c;
	// 		sort();
	// 	}

	// 	void sort();
	// };
}