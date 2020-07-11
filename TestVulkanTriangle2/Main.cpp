
//^(?!(\s*\*))(?!(\s*\-\-\>))(?!(\s*\<\!\-\-))(?!(\s*\n))(?!(\s*\*\/))(?!(\s*\/\*))(?!(\s*\/\/\/))(?!(\s*\/\/))(?!(\s(using))).*$



#include "LuxEngine/LuxEngine.h"
#include "LuxEngine/Types/LuxObject/2D/2DLines.h"
#include "LuxEngine/Types/LuxObject/2D/2DRenderSpace.h"
#include "LuxEngine/Engine/Input/KeyboardInput.h"
#include "LuxEngine/Engine/Input/Input.h"


//namespace lux::obj {
//	using h = Line2D;
//}


int main() {
	//LuxObjectLine l;
	LuxInit(false);
	
	//sleep(4000);
	lux::input::InputState inputStateTest{
		lux::input::KeySequence{ {LUX_KEY_W | LUX_PRESS}, [](LuxArray<uint16> keySequenceCode) {  printf("AAAAA"); }},
		lux::input::KeySequence{ {LUX_KEY_S | LUX_PRESS, LUX_KEY_S | LUX_RELEASE}, [](LuxArray<uint16> keySequenceCode) {  printf("BBBB"); } } ,
		lux::input::KeySequence{ {LUX_KEY_S | LUX_PRESS, LUX_KEY_A | LUX_PRESS, LUX_KEY_A | LUX_RELEASE}, [](LuxArray<uint16> keySequenceCode) {  printf("CCC"); } },
		lux::input::KeySequence{ {LUX_KEY_S | LUX_PRESS, LUX_KEY_A | LUX_PRESS, LUX_KEY_D | LUX_PRESS}, [](LuxArray<uint16> keySequenceCode) {  printf("DD"); } }
	};
	lux::input::setInputState(&inputStateTest);



	{
		lux::obj::RenderSpace2D renderSpace;

		lux::obj::Line2D lineTest;
		renderSpace.spawnObject(&lineTest);
		*lineTest.col0 = vec4f32{ 1.0f, 0.1f, 0.0f, 1.0f };
		*lineTest.col1 = vec4f32{ 0.0f, 0.2f, 1.0f, 0.0f };
		*lineTest.wd0 = 100;
		*lineTest.wd1 = 200;
		*lineTest.p0 = vec2i32{ 2000, 500 };
		*lineTest.p1 = vec2i32{ 1700, 800 };
	}


	while (engine.running) {
		sleep(5);
	}
	return 0;
}