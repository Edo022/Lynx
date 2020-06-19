
#pragma once
#include <thread>
#include "LuxEngine/macros.h"


struct LuxFence {
	char value = 0;

	#pragma optimize( "g", off )									//Turn off global optimization. If the compiler optimizes the loop, it will be skipped and the program will freeze when resizing a window
	inline void wait(char v) { while (value != v) { sleep(2); } }	//A 2ms sleep every iteration uses way less CPU than iterating the loop every time, and the thread can do other things while waiting
	#pragma optimize( "g", on )										//Turn on global optimization
	inline bool check(char v) { return value == v; }
	inline void set(char v) { value = v; }
};