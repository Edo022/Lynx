

//regex per contare le linee di codice non vuote
//^(?!(\s*\*))(?!(\s*\-\-\>))(?!(\s*\<\!\-\-))(?!(\s*\n))(?!(\s*\*\/))(?!(\s*\/\*))(?!(\s*\/\/\/))(?!(\s*\/\/))(?!(\s(using))).*$
//stessa cosa ma comprendendo i commenti
//^(/)|(?!(\s*\*))(?!(\s*\-\-\>))(?!(\s*\<\!\-\-))(?!(\s*\n))(?!(\s*\*\/))(?!(\s*\/\*))(?!(\s*\/\/\/))(?!(\s*\/\/))(?!(\s(using))).*$
//TODO squared distance aa*bb
//TODO merge node to merge aligned objects in a unique cloud/image
//TODO use intrinsic functions with vector and matrices types
//TODO limit vector types to float, double, int32, uint32

#include <time.h>
#include "LuxEngine/macros.hpp"
#include "LuxEngine/Core/Input/Input.hpp"
#include "LuxEngine/LuxEngine.hpp"
#include "LuxEngine/Types/LuxObject/2D/2DLines.hpp"
#include "LuxEngine/Types/LuxObject/2D/2DBorder.hpp"
#include "LuxEngine/Types/LuxObject/2D/2DRenderSpace.hpp"
#include "LuxEngine/Threads/ThreadPool.hpp"
#include "LuxEngine/Memory/Ram/Memory.hpp"
#include "LuxEngine/Memory/Gpu/VMemory.hpp"






#define LUH_H_TEST_TU
// PostInitializer(LUH_H_TEST_TU);






//TODO LIMIT FORCED PREINITIALIZATION TO MEMORY POOL ONLY

int h(const char* b, int hh) {
	printf("\n%s %d \n",b, hh);
	return 0;
}

void hg(lux::RTArray<uint16>){
	printf("\nKEYYYY");
}

//0000017baf9ef070
//1100100100

//TODO
//TODO object cache
//TODO object secondary buffers
//TODO

//TODO break memory pool placeholder pointers if the the memory is full
//TODO align memory allocation and sizes to 32 bytes

//TODO static objects have ordered vertices
//TODO dynamic objects are static if not modified for a certain time

//TODO fix map linked free list. Use it even with one element
//TODO add capacity function to dynamic containers




void runArrayTest();

int main( ) {
	// //TODO remove debug junk ..
		runArrayTest();
		// return 0;
	// //TODO remove debug junk ^^



	//BUG THOSE VARIABLES ARE PROBABLY THE CAUSE OF ALL THE BUGS
	//BUG THEY GET DESTROYED WHEN THEY GO OUT OF SCOPE BUT THE POINTER IN THE HEAP REMAINS
	{
		//TODO automatize object search //TODO wtf does this mean
		lux::obj::RenderSpace2D renderSpace(lux::obj::AlignmentType::FixedHorizontal, 2);
		lux::obj::Border2D testBorder({0, 0}, {1, 1});
		lux::obj::Line2D lineTest{
			vec2f32{ .4, .5 }, vec2f32{ .8, .8 },
			vec4f32{ 1.0f, 0.1f, 0.0f, 1.0f }, vec4f32{ 0.0f, 0.2f, 1.0f, 1.0f },
			100, 200
		};
		// usleep(1000*1000);
		renderSpace.addChild(&lineTest);
		lux::obj::Line2D lineTest2{
			vec2f32{ .4, .5 }, vec2f32{ .8, .8 },
			vec4f32{ 1.0f, 0.1f, 0.0f, 1.0f }, vec4f32{ 0.0f, 0.2f, 1.0f, 1.0f },
			100, 200
		};
		renderSpace.addChild(&lineTest2);
		//TODO FIX OR ADD WARNING WHEN THE VARIABLE GETS DESTROYED
		lux::obj::addRenderSpace(new lux::obj::RenderSpace2D(renderSpace));
	}

	// sleep(5000);

	lux::input::InputState inputStateTest({
		lux::input::KeySequence{ LUX_KEY_W | lux::input::LuxKeyState::LUX_PRESS, hg },
		lux::input::KeySequence{ LUX_KEY_A | lux::input::LuxKeyState::LUX_PRESS, hg }
		});
	lux::input::setInputState(&inputStateTest);


	srand(time(NULL));
	//lux::thr::sendToExecQueue(h, lux::thr::LUX_PRIORITY_MAX, nullptr, "UwU EKUSUPUROSION", 12345);
	//lux::thr::sendToExecQueue(h, lux::thr::LUX_PRIORITY_MAX, nullptr, "UwU EKUSUPUROSION", 12345);
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
//static inline auto distToPoint(const vec2f32& a, const vec2f32& b, const vec2f32& p){
//	float32 l2 = (pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
//	if(l2 == 0) return dist(p, a);
//	return dist(p, (a + ((b - a) * max(0, min(1, dot(p - a, b - a) / l2)))));
//}
//
	//static inline auto distToPoint2(const vec2f32& a, const vec2f32& b, const vec2f32& p){
//	if(pow(dist(a, b), 2) == 0) return dist(p, a);
//	return dist(p, (a + ((b - a) * max(0, min(1, dot(p - a, b - a) / (pow(b.x - a.x, 2) + pow(b.y - a.y, 2)))))));
//}
//
//
//
//
//static inline auto distToPoint2D(const vec2f32& a, const vec2f32& b, const vec2f32& p){
//	return dist(p, ((a == b) ? a : (a + ((b - a) * max(0, min(1, (dot(p - a, b - a) / (pow(b.x - a.x, 2) + pow(b.y - a.y, 2)))))))));
//}
////Distnce to point for lines with non zero length
//static inline auto distToPoint2D_nz(const vec2f32& a, const vec2f32& b, const vec2f32& p){
//	return dist(p, (a + ((b - a) * max(0, min(1, (dot(p - a, b - a) / (pow(b.x - a.x, 2) + pow(b.y - a.y, 2))))))));
//	//return dist(p, (a + ((b - a) * max(0, min(1, (dot(p - a, b - a) / pow(dist(a, b), 2)))))));
//}

////TODO separate local and host memory in gpu memory pool















void runArrayTest(){
	lux::ram::ptr<int> a = lux::ram::allocBck(10);
	lux::ram::ptr<int> b = lux::ram::allocBck(32);
	lux::ram::ptr<int> c = lux::ram::allocBck(256);
	lux::ram::ptr<int> d = lux::ram::allocBck(256);
	lux::ram::ptr<int> e = lux::ram::allocBck(256);
	int h = 0;
}