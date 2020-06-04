
//^(?!(\s*\*))(?!(\s*\-\-\>))(?!(\s*\<\!\-\-))(?!(\s*\n))(?!(\s*\*\/))(?!(\s*\/\*))(?!(\s*\/\/\/))(?!(\s*\/\/))(?!(\s(using))).*$


/*
	LD 0
		queue 0
			command pool 0
				command buffer 0
					command 0
					command 1
					command 2
					...
				...
			...
		...
	LD 1
		...
	...
*/



#include "LuxEngine/LuxEngine.h"


void hh(LuxArray<uint16> keySequenceCode) {
	printf("AAAAAAA");
}
void hh2(LuxArray<uint16> keySequenceCode) {
	printf("BBBB");
}


int main() {
	luxInit(false);
	LuxInputState inputStateTest{ 
		LuxKeyBinding{ {LUX_KEY_W | LUX_PRESS, LUX_KEY_W | LUX_RELEASE}, &hh },
		LuxKeyBinding{ {LUX_KEY_S | LUX_PRESS, LUX_KEY_S | LUX_RELEASE}, &hh2 } 
	};


	luxInputSetInputState(&inputStateTest);
	Frame{
		Sleep(1);
	}
	return 0;
}