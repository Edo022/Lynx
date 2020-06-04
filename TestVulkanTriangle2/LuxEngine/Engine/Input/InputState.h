#pragma once

#include <GLFW/glfw3.h>
#include "MouseInput.h"
#include "KeyboardInput.h"
#include "LuxEngine/Types/Containers/LuxDynArray.h"



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
	LuxArray<uint16> keySequenceCode;
	//This is the function that will be called when the sequence is performed. It must be of type void and take the key sequence as a parameter
	LuxKeyBindingCallback bindedFunction;
};

typedef LuxArray<LuxKeyBinding> LuxInputState;