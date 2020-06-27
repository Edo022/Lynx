
//^(?!(\s*\*))(?!(\s*\-\-\>))(?!(\s*\<\!\-\-))(?!(\s*\n))(?!(\s*\*\/))(?!(\s*\/\*))(?!(\s*\/\/\/))(?!(\s*\/\/))(?!(\s(using))).*$



#include "LuxEngine/LuxEngine.h"
#include <iostream>







int main() {
	float32 a = INT32_MAX;
	printf("%f", a);

	LuxInit(false);
	LuxInputState inputStateTest{
		LuxKeySequence{ {LUX_KEY_W | LUX_PRESS}, [](LuxArray<uint16> keySequenceCode)->void {  printf("AAAAA"); }},
		LuxKeySequence{ {LUX_KEY_S | LUX_PRESS, LUX_KEY_S | LUX_RELEASE}, [](LuxArray<uint16> keySequenceCode)->void {  printf("BBBB"); } } ,
		LuxKeySequence{ {LUX_KEY_S | LUX_PRESS, LUX_KEY_A | LUX_PRESS, LUX_KEY_A | LUX_RELEASE}, [](LuxArray<uint16> keySequenceCode)->void {  printf("CCC"); } },
		LuxKeySequence{ {LUX_KEY_S | LUX_PRESS, LUX_KEY_A | LUX_PRESS, LUX_KEY_D | LUX_PRESS}, [](LuxArray<uint16> keySequenceCode)->void {  printf("DD"); } }
	};
	luxInputSetInputState(&inputStateTest);


	while (engine.running) {
		//int h = -573289;
		//printf("%lf\n", fcos(h));
		sleep(1);
	}
	return 0;
}