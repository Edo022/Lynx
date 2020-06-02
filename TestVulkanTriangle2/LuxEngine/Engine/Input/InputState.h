#pragma once

#include <GLFW/glfw3.h>
#include "MouseInput.h"
#include "KeyboardInput.h"
#include "LuxEngine/Types/Containers/LuxDynArray.h"


enum LuxKeyState {
	LUX_PRESS = 0x100000,
	LUX_RELEASE = 0
};


//This struct contains a sequence of keys and a function to call when the sequence is performed
//The sequence is saved as an array of uint16. The action and the key are in the same varibale for better performances.
//   - |---------|Unused
//   1 0 0 0 0 0 0 1 0 0 0 0 0 0 1 1   
//   |Action       |_______________|Key 
struct LuxKeyBinding {
	//Each element represents a key and its action
	//Example for the sequence "ctrl + k" : {LUX_KEY_LEFT_CTRL | LUX_PRESS, LUX_KEY_K | LUX_PRESS, LUX_KEY_K | LUX_RELEASE}
	LuxArray<int64> code;
	//This is the function that will be called when the sequence is performed
	void* bindingCallback;
};


//
struct InputState {
	LuxDynArray<LuxKeyBinding> keyBindings;

};