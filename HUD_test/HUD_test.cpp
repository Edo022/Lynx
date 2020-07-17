

//regex per contare le linee di codice non vuote
//^(?!(\s*\*))(?!(\s*\-\-\>))(?!(\s*\<\!\-\-))(?!(\s*\n))(?!(\s*\*\/))(?!(\s*\/\*))(?!(\s*\/\/\/))(?!(\s*\/\/))(?!(\s(using))).*$


//TODO add memory pool

#include <time.h>

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


int main() {
	sleep(10);
	LuxInit(false);
	//sleep(1000);
	

	{
		lux::obj::Line2D* lineTest = new lux::obj::Line2D( );
		*lineTest->col0 = vec4f32{ 1.0f, 0.1f, 0.0f, 1.0f };
		*lineTest->col1 = vec4f32{ 0.0f, 0.2f, 1.0f, 0.0f };
		*lineTest->wd0 = 100;
		*lineTest->wd1 = 200;
		*lineTest->p0 = vec2i32{ 2000, 500 };
		*lineTest->p1 = vec2i32{ 1700, 800 };

		lux::obj::RenderSpace2D renderSpace;
		renderSpace.addChild(lineTest);

		//TODO
		lux::obj::addRenderSpace(&renderSpace);
		Engine& engine_ = lux::engine( );
		engine_.cshaderNew(lux::Array<LuxCell>{ engine_.gpuCellWindowOutput, engine_.gpuCellWindowSize, lineTest->gpuCell }, (engine_.shaderPath + lineTest->shaderName + ".comp.spv").begin( ), 4, 1, 1);
	}


	srand(time(NULL));
	lux::thr::sendToExecQueue(h, lux::thr::LUX_PRIORITY_MAX, "UwU EKUSUPUROSION", 12345);
	lux::thr::sendToExecQueue(h, lux::thr::LUX_PRIORITY_MAX, "UwU EKUSUPUROSION", 12345);
	while(lux::engine( ).running) {
		//lux::thr::sendToExecQueue(h, lux::thr::LUX_PRIORITY_MAX, "UwU EKUSUPUROSION", 12345);
		//sleep(rand( ) % 20);
		sleep(10);
	}
	return 0;
}


//TODO ! la memoria pre allocata dei buffer della GPU non distingue memoria locale e condivisa