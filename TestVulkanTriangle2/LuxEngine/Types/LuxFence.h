
#pragma once
#include <thread>
#include "LuxEngine/macros.h"


//A struct used to synchronize threads
struct LuxFence {
	char value = 0;

	#pragma optimize( "g", off )									//Turn off global optimization. If the compiler optimizes the loop, it will be skipped and the program will probably freeze or crash
	inline void wait(const char v) const {							//Waits until the fence has the same value of v
		while (value != v) { sleep(2); }							//A 2ms sleep every iteration uses way less CPU than iterating the loop every time, and the thread can do other things while waiting
	}
	#pragma optimize( "g", on )										//Turn on global optimization

	inline bool check(const char v) const { return value == v; }	//Returns true if the fence has the same value as v, false if not
	inline void set(const char v) { value = v; }					//Assigns a value to the fence
};