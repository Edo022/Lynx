
//^(?!(\s*\*))(?!(\s*\-\-\>))(?!(\s*\<\!\-\-))(?!(\s*\n))(?!(\s*\*\/))(?!(\s*\/\*))(?!(\s*\/\/\/))(?!(\s*\/\/))(?!(\s(using))).*$



#include "LuxEngine/LuxEngine.h"
#include "LuxEngine/Types/LuxObject/2D/2DLines.h"
#include "LuxEngine/Types/LuxObject/2D/2DRenderSpace.h"
#include "LuxEngine/Engine/Input/KeyboardInput.h"
#include "LuxEngine/Engine/Input/Input.h"



int main() {

	//LuxObjectLine l;
	//LuxRenderSpace2D r;

	//int h = l.ID;
	//int h2 = r.ID;
	engine.spawnObjFence.set(0);
	std::thread t([&]() {LuxInit(false); });
	engine.spawnObjFence.wait(1);


	LuxInputState inputStateTest{
		LuxKeySequence{ {LUX_KEY_W | LUX_PRESS}, [](LuxArray<uint16> keySequenceCode) {  printf("AAAAA"); }},
		LuxKeySequence{ {LUX_KEY_S | LUX_PRESS, LUX_KEY_S | LUX_RELEASE}, [](LuxArray<uint16> keySequenceCode) {  printf("BBBB"); } } ,
		LuxKeySequence{ {LUX_KEY_S | LUX_PRESS, LUX_KEY_A | LUX_PRESS, LUX_KEY_A | LUX_RELEASE}, [](LuxArray<uint16> keySequenceCode) {  printf("CCC"); } },
		LuxKeySequence{ {LUX_KEY_S | LUX_PRESS, LUX_KEY_A | LUX_PRESS, LUX_KEY_D | LUX_PRESS}, [](LuxArray<uint16> keySequenceCode) {  printf("DD"); } }
	};
	luxInputSetInputState(&inputStateTest);




	{
		LuxDynamic_LuxObjectLineCCT lineTest;
		luxSpawnObject(&lineTest);
		*lineTest.col0 = vec4f32{ 1, 0.1, 0, 1 };
		*lineTest.col1 = vec4f32{ 0, 0.2, 1, 0 };
		*lineTest.wd0 = 100;
		*lineTest.wd1 = 200;
		*lineTest.p0 = vec2i32{ 2000, 500 };
		*lineTest.p1 = vec2i32{ 1700, 800 };

		LuxDynamic_LuxObjectLineCCT lineTest2;
		luxSpawnObject(&lineTest2);
		*lineTest2.col0 = vec4f32{ 1, 0.1, 0, 1 };
		*lineTest2.col1 = vec4f32{ 0, 0.2, 1, 0 };
		*lineTest2.wd0 = 1;
		*lineTest2.wd1 = 1;
		*lineTest2.p0 = vec2i32{ 2000, 500 };
		*lineTest2.p1 = vec2i32{ 1700, 800 };
		engine.spawnObjFence.set(2);
	}


	while (engine.running) {
		sleep(5);
	}
	return 0;
}