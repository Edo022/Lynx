
//^(?!(\s*\*))(?!(\s*\-\-\>))(?!(\s*\<\!\-\-))(?!(\s*\n))(?!(\s*\*\/))(?!(\s*\/\*))(?!(\s*\/\/\/))(?!(\s*\/\/))(?!(\s(using))).*$



#include "LuxEngine/LuxEngine.h"
#include "LuxEngine/Types/LuxObject/2D/2DLines.h"
#include "LuxEngine/Types/LuxObject/2D/2DRenderSpace.h"
#include "LuxEngine/Engine/Input/KeyboardInput.h"
#include "LuxEngine/Engine/Input/Input.h"
#include "LuxEngine/Threads/ThreadPool.h"


//namespace lux::obj {
//	using h = Line2D;
//}


int h(const char* b, int hh) {
	printf("\n%s %d \n",b, hh);
	return 0;
}


lux::obj::Line2D lineTest;
int main() {
	LuxInit(false);
	
	lux::thr::sendToExecQueue(h, lux::thr::LUX_PRIORITY_MAX, "UwU EKUSUPUROSION", 12345);


	{
		lux::obj::RenderSpace2D renderSpace;

		renderSpace.spawnObject(&lineTest);
		*lineTest.col0 = vec4f32{ 1.0f, 0.1f, 0.0f, 1.0f };
		*lineTest.col1 = vec4f32{ 0.0f, 0.2f, 1.0f, 0.0f };
		*lineTest.wd0 = 100;
		*lineTest.wd1 = 200;
		*lineTest.p0 = vec2i32{ 2000, 500 };
		*lineTest.p1 = vec2i32{ 1700, 800 };


		lux::input::InputState ist{
			lux::input::KeySequence{{LUX_KEY_KP_ADD | LUX_PRESS }, [](lux::Array<uint16> keySequenceCode) { *lineTest.col0 -= vec4f32(0.0f, 0.0f, 0.0f, 0.05f); }},
			lux::input::KeySequence{{LUX_KEY_KP_SUBTRACT | LUX_PRESS }, [](lux::Array<uint16> keySequenceCode) { *lineTest.col0 += vec4f32(0.0f, 0.0f, 0.0f, 0.05f); }}
		};
		lux::input::setInputState(&ist);
	}



	while (engine.running) {
		sleep(5);
	}
	return 0;
}