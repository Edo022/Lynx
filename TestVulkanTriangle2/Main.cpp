
//^(?!(\s*\*))(?!(\s*\-\-\>))(?!(\s*\<\!\-\-))(?!(\s*\n))(?!(\s*\*\/))(?!(\s*\/\*))(?!(\s*\/\/\/))(?!(\s*\/\/))(?!(\s(using))).*$



#include "LuxEngine/LuxEngine.h"
#include "LuxEngine/Types/LuxObject/2D/2DLines.h"
#include "LuxEngine/Types/LuxObject/2D/2DRenderSpace.h"
#include "LuxEngine/Engine/Input/KeyboardInput.h"
#include "LuxEngine/Engine/Input/Input.h"





int main() {
	LuxObjectLine l;
	LuxRenderSpace2D r;

	int h = l.ID;
	int h2 = r.ID;
	LuxInit(false);
	LuxInputState inputStateTest{
		LuxKeySequence{ {LUX_KEY_W | LUX_PRESS}, [](LuxArray<uint16> keySequenceCode) {  printf("AAAAA"); }},
		LuxKeySequence{ {LUX_KEY_S | LUX_PRESS, LUX_KEY_S | LUX_RELEASE}, [](LuxArray<uint16> keySequenceCode) {  printf("BBBB"); } } ,
		LuxKeySequence{ {LUX_KEY_S | LUX_PRESS, LUX_KEY_A | LUX_PRESS, LUX_KEY_A | LUX_RELEASE}, [](LuxArray<uint16> keySequenceCode) {  printf("CCC"); } },
		LuxKeySequence{ {LUX_KEY_S | LUX_PRESS, LUX_KEY_A | LUX_PRESS, LUX_KEY_D | LUX_PRESS}, [](LuxArray<uint16> keySequenceCode) {  printf("DD"); } }
	};
	luxInputSetInputState(&inputStateTest);


	while (engine.running) {
		sleep(5);
	}
	return 0;
}