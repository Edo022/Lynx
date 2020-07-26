

//regex per contare le linee di codice non vuote
//^(?!(\s*\*))(?!(\s*\-\-\>))(?!(\s*\<\!\-\-))(?!(\s*\n))(?!(\s*\*\/))(?!(\s*\/\*))(?!(\s*\/\/\/))(?!(\s*\/\/))(?!(\s(using))).*$
//TODO squared distance aa*bb


//TODO add memory pool

#include <time.h>
#include "LuxEngine/LuxEngine.h"
#include "LuxEngine/Engine/Input/Input.h"
#include "LuxEngine/macros.h"


//namespace lux::obj {
//	using h = Line2D;
//}


int h(const char* b, int hh) {
	printf("\n%s %d \n",b, hh);
	return 0;
}


void hg(lux::Array<uint16>){
	printf("\nKEYYYY");
}


int main() {
	LuxInit(false);
	//sleep(1000);


	vec2i32 hhh = {10, 9 };
	hhh *= 0.5;


	{
		lux::obj::Line2D lineTest;
		lineTest.col0 = vec4f32{ 1.0f, 0.1f, 0.0f, 1.0f };
		lineTest.col1 = vec4f32{ 0.0f, 0.2f, 1.0f, 1.0f };
		lineTest.wd0 = 100;
		lineTest.wd1 = 200;
		lineTest.p0 = vec2i32{ 2000, 500 };
		lineTest.p1 = vec2i32{ 1700, 800 };

		lux::obj::Line2D lineTest2;
		lineTest2.col0 = vec4f32{ 1.0f, 0.1f, 0.0f, 1.0f };
		lineTest2.col1 = vec4f32{ 0.0f, 0.2f, 1.0f, 1.0f };
		lineTest2.wd0 = 100;
		lineTest2.wd1 = 200;
		lineTest2.p0 = vec2i32{ 2000, 500 };
		lineTest2.p1 = vec2i32{ 1700, 800 };


		lux::obj::RenderSpace2D renderSpace(lux::obj::AlignmentType::FixedHorizontal, 2);
		renderSpace.addChild(&lineTest);
		renderSpace.addChild(&lineTest2);

		//TODO
		Engine& engine_ = lux::getEngine( );
		engine_.cshaderNew({ engine_.gpuCellWindowOutput, engine_.gpuCellWindowSize, lineTest.gpuCell }, LUX_DEF_SHADER_LINE_2D, lineTest.shaderName, 4, 1, 1);
		engine_.cshaderNew({ engine_.gpuCellWindowOutput, engine_.gpuCellWindowSize, lineTest2.gpuCell }, LUX_DEF_SHADER_LINE_2D, lineTest.shaderName, 4, 1, 1);
		//engine_.cshaderNew(lux::Array<LuxCell>{ engine_.gpuCellWindowOutput, engine_.gpuCellWindowOutput_i, engine_.gpuCellWindowSize}, "FloatToIntBuffer", engine_.swapchainExtent.width / 32, engine_.swapchainExtent.height / 32, 1);
		engine_.cshaderNew({ engine_.gpuCellWindowOutput, engine_.gpuCellWindowOutput_i, engine_.gpuCellWindowSize}, LUX_DEF_SHADER_COPY, "FloatToIntBuffer", engine_.width / 32 + 1, engine_.height / 32 + 1, 1);
		lux::obj::addRenderSpace(&renderSpace);
	}


	lux::input::InputState is({
		lux::input::KeySequence{ LUX_KEY_W | lux::input::LuxKeyState::LUX_PRESS, hg },
		lux::input::KeySequence{ LUX_KEY_A | lux::input::LuxKeyState::LUX_PRESS, hg }
		});
	lux::input::setInputState(&is);


	srand(time(NULL));
	lux::thr::sendToExecQueue(h, lux::thr::LUX_PRIORITY_MAX, "UwU EKUSUPUROSION", 12345);
	lux::thr::sendToExecQueue(h, lux::thr::LUX_PRIORITY_MAX, "UwU EKUSUPUROSION", 12345);
	while(lux::getEngine( ).running) {
		//lux::thr::sendToExecQueue(h, lux::thr::LUX_PRIORITY_MAX, "UwU EKUSUPUROSION", 12345);
		//sleep(rand( ) % 20);
		sleep(10);
	}
	return 0;
}






#undef max
#undef min



static inline auto __vectorcall distToPoint(const vec2f32& a, const vec2f32& b, const vec2f32& p){
	float32 l2 = (pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
	if(l2 == 0) return dist(p, a);
	return dist(p, (a + ((b - a) * max(0, min(1, dot(p - a, b - a) / l2)))));
}

static inline auto __vectorcall distToPoint2(const vec2f32& a, const vec2f32& b, const vec2f32& p){
	if(pow(dist(a, b), 2) == 0) return dist(p, a);
	return dist(p, (a + ((b - a) * max(0, min(1, dot(p - a, b - a) / (pow(b.x - a.x, 2) + pow(b.y - a.y, 2)))))));
}




static inline auto __vectorcall distToPoint2D(const vec2f32& a, const vec2f32& b, const vec2f32& p){
	return dist(p, ((a == b) ? a : (a + ((b - a) * max(0, min(1, (dot(p - a, b - a) / (pow(b.x - a.x, 2) + pow(b.y - a.y, 2)))))))));
}
//Distnce to point for lines with non zero length
static inline auto __vectorcall distToPoint2D_nz(const vec2f32& a, const vec2f32& b, const vec2f32& p){
	return dist(p, (a + ((b - a) * max(0, min(1, (dot(p - a, b - a) / (pow(b.x - a.x, 2) + pow(b.y - a.y, 2))))))));
	//return dist(p, (a + ((b - a) * max(0, min(1, (dot(p - a, b - a) / pow(dist(a, b), 2)))))));
}


//TODO ! la memoria pre allocata dei buffer della GPU non distingue memoria locale e condivisa