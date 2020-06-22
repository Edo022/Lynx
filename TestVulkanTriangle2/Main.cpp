
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


#ifndef NDEBUG
#define LUX_DEBUG
#endif

#include "LuxEngine/LuxEngine.h"
#include <filesystem>
#include <iostream>

void hh(LuxArray<uint16> keySequenceCode) { printf("AAAAA"); }
void hh2(LuxArray<uint16> keySequenceCode) { printf("BBBB"); }
void hh3(LuxArray<uint16> keySequenceCode) { printf("CCC"); }
void hh4(LuxArray<uint16> keySequenceCode) { printf("DD"); }




int main() {
	//std::string path = "./";
	//for (const auto& entry : std::filesystem::directory_iterator(path))
	//	std::cout << entry.path() << std::endl;

	LuxInit(false);
	//LuxTime time = luxGetTime();

	//double* h = (double*)malloc(sizeof(double) * FUNC_PRECISION * 1000);
	//	for (uint64 i = 0; i < FUNC_PRECISION * 1000; ++i) {
	//		h[i] = fsin(i);
	//		if (i == 1000 * FUNC_PRECISION -1) printf("ciao");;
	//	}
	//
	//printf("\n%lf\n", luxTimeGetDuration(time));
	//flushall();
	LuxInputState inputStateTest{
		LuxKeySequence{ {LUX_KEY_W | LUX_PRESS}, &hh },
		LuxKeySequence{ {LUX_KEY_S | LUX_PRESS, LUX_KEY_S | LUX_RELEASE}, &hh2 } ,
		LuxKeySequence{ {LUX_KEY_S | LUX_PRESS, LUX_KEY_A | LUX_PRESS, LUX_KEY_A | LUX_RELEASE}, &hh3 },
		LuxKeySequence{ {LUX_KEY_S | LUX_PRESS, LUX_KEY_A | LUX_PRESS, LUX_KEY_D | LUX_PRESS}, &hh4 }
	};
	luxInputSetInputState(&inputStateTest);


	while (engine.running) {
		int h = -573289;
		printf("%lf\n", fcos(h));
		sleep(1);
	}
	return 0;
}