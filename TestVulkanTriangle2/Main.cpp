
//^(?!(\s*\*))(?!(\s*\-\-\>))(?!(\s*\<\!\-\-))(?!(\s*\n))(?!(\s*\*\/))(?!(\s*\/\*))(?!(\s*\/\/\/))(?!(\s*\/\/))(?!(\s(using))).*$



#include "LuxEngine/LuxEngine.h"
#include "LuxEngine/Types/LuxObject/2D/2DLines.h"
#include "LuxEngine/Types/LuxObject/2D/2DRenderSpace.h"
#include "LuxEngine/Engine/Input/KeyboardInput.h"
#include "LuxEngine/Engine/Input/Input.h"



int main() {
	//LuxObjectLine l;
	//LuxRenderSpace2D r;
	LuxInit(false);

	
	lux::input::InputState inputStateTest{
		lux::input::KeySequence{ {LUX_KEY_W | LUX_PRESS}, [](LuxArray<uint16> keySequenceCode) {  printf("AAAAA"); }},
		lux::input::KeySequence{ {LUX_KEY_S | LUX_PRESS, LUX_KEY_S | LUX_RELEASE}, [](LuxArray<uint16> keySequenceCode) {  printf("BBBB"); } } ,
		lux::input::KeySequence{ {LUX_KEY_S | LUX_PRESS, LUX_KEY_A | LUX_PRESS, LUX_KEY_A | LUX_RELEASE}, [](LuxArray<uint16> keySequenceCode) {  printf("CCC"); } },
		lux::input::KeySequence{ {LUX_KEY_S | LUX_PRESS, LUX_KEY_A | LUX_PRESS, LUX_KEY_D | LUX_PRESS}, [](LuxArray<uint16> keySequenceCode) {  printf("DD"); } }
	};
	lux::input::setInputState(&inputStateTest);


	{
		lux::obj::Line2D lineTest;
		lux::obj::spawnObject(&lineTest);
		*lineTest.col0 = vec4f32{ 1, 0.1, 0, 1 };
		*lineTest.col1 = vec4f32{ 0, 0.2, 1, 0 };
		*lineTest.wd0 = 100;
		*lineTest.wd1 = 200;
		*lineTest.p0 = vec2i32{ 2000, 500 };
		*lineTest.p1 = vec2i32{ 1700, 800 };

		//Line2D lineTest2;
		//spawnObject(&lineTest2);
		//*lineTest2.col0 = vec4f32{ 1, 0.1, 0, 1 };
		//*lineTest2.col1 = vec4f32{ 0, 0.2, 1, 0 };
		//*lineTest2.wd0 = 1;
		//*lineTest2.wd1 = 1;
		//*lineTest2.p0 = vec2i32{ 2000, 500 };
		//*lineTest2.p1 = vec2i32{ 1700, 800 };
	}


	while (engine.running) {
		sleep(5);
	}
	return 0;
}