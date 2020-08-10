

//regex per contare le linee di codice non vuote
//^(?!(\s*\*))(?!(\s*\-\-\>))(?!(\s*\<\!\-\-))(?!(\s*\n))(?!(\s*\*\/))(?!(\s*\/\*))(?!(\s*\/\/\/))(?!(\s*\/\/))(?!(\s(using))).*$
//stessa cosa ma comprendendo i commenti
//^(/)|(?!(\s*\*))(?!(\s*\-\-\>))(?!(\s*\<\!\-\-))(?!(\s*\n))(?!(\s*\*\/))(?!(\s*\/\*))(?!(\s*\/\/\/))(?!(\s*\/\/))(?!(\s(using))).*$
//TODO squared distance aa*bb
//TODO add memory pool



#include <time.h>
#include "LuxEngine/macros.h"
#include "LuxEngine/Core/Input/Input.h"
#include "LuxEngine/LuxEngine.h"
#include "LuxEngine/Types/LuxObject/2D/2DLines.h"
#include "LuxEngine/Types/LuxObject/2D/2DBorder.h"
#include "LuxEngine/Types/LuxObject/2D/2DRenderSpace.h"
#include "LuxEngine/Threads/ThreadPool.h"








int h(const char* b, int hh) {
	printf("\n%s %d \n",b, hh);
	return 0;
}

void hg(lux::Array<uint16>){
	printf("\nKEYYYY");
}




int main( ) {
	LuxInit(false);


	{
		lux::obj::Line2D* lineTest3;
		lux::obj::Line2D lineTest{
			vec2f32{ 2000, 500 }, vec2f32{ 1700, 800 },
			vec4f32{ 1.0f, 0.1f, 0.0f, 1.0f }, vec4f32{ 0.0f, 0.2f, 1.0f, 1.0f },
			100, 200
		};

		lux::obj::Line2D lineTest2{
			vec2f32{ 2000, 500 }, vec2f32{ 1700, 800 },
			vec4f32{ 1.0f, 0.1f, 0.0f, 1.0f }, vec4f32{ 0.0f, 0.2f, 1.0f, 1.0f },
			100, 200
		};



		lux::obj::RenderSpace2D renderSpace(lux::obj::AlignmentType::FixedHorizontal, 2);
		renderSpace.addChild(&lineTest);
		renderSpace.addChild(&lineTest2);
		//TODO automatize object search
		//lineTest.debugBorder->update( );
		//lineTest2.debugBorder->update( );
		//renderSpace.addChild(lineTest.debugBorder);
		//renderSpace.addChild(lineTest2.debugBorder);

		lux::obj::addRenderSpace(&renderSpace);
	}


	lux::input::InputState inputStateTest({
		lux::input::KeySequence{ LUX_KEY_W | lux::input::LuxKeyState::LUX_PRESS, hg },
		lux::input::KeySequence{ LUX_KEY_A | lux::input::LuxKeyState::LUX_PRESS, hg }
		});
	lux::input::setInputState(&inputStateTest);


	srand(time(NULL));
	lux::thr::sendToExecQueue(h, lux::thr::LUX_PRIORITY_MAX, "UwU EKUSUPUROSION", 12345);
	lux::thr::sendToExecQueue(h, lux::thr::LUX_PRIORITY_MAX, "UwU EKUSUPUROSION", 12345);
	while(lux::core::running) {
		//lux::thr::sendToExecQueue(h, lux::thr::LUX_PRIORITY_MAX, "UwU EKUSUPUROSION", 12345);

		sleep(10);
	}
	return 0;
}


//TODO use suspend and resume thread functions in fences



//#undef max
//#undef min
 //
//static inline auto __vectorcall distToPoint(const vec2f32& a, const vec2f32& b, const vec2f32& p){
//	float32 l2 = (pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
//	if(l2 == 0) return dist(p, a);
//	return dist(p, (a + ((b - a) * max(0, min(1, dot(p - a, b - a) / l2)))));
//}
//
	//static inline auto __vectorcall distToPoint2(const vec2f32& a, const vec2f32& b, const vec2f32& p){
//	if(pow(dist(a, b), 2) == 0) return dist(p, a);
//	return dist(p, (a + ((b - a) * max(0, min(1, dot(p - a, b - a) / (pow(b.x - a.x, 2) + pow(b.y - a.y, 2)))))));
//}
//
//
//
//
//static inline auto __vectorcall distToPoint2D(const vec2f32& a, const vec2f32& b, const vec2f32& p){
//	return dist(p, ((a == b) ? a : (a + ((b - a) * max(0, min(1, (dot(p - a, b - a) / (pow(b.x - a.x, 2) + pow(b.y - a.y, 2)))))))));
//}
////Distnce to point for lines with non zero length
//static inline auto __vectorcall distToPoint2D_nz(const vec2f32& a, const vec2f32& b, const vec2f32& p){
//	return dist(p, (a + ((b - a) * max(0, min(1, (dot(p - a, b - a) / (pow(b.x - a.x, 2) + pow(b.y - a.y, 2))))))));
//	//return dist(p, (a + ((b - a) * max(0, min(1, (dot(p - a, b - a) / pow(dist(a, b), 2)))))));
//}

////TODO separate local and host memory in gpu memory pool
