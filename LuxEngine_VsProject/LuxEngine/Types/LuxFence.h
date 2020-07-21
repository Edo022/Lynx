


#pragma once
#include "LuxEngine/macros.h"
#include "Integers/Integers.h"
#include <thread>
#include <mutex>



#pragma optimize( "g", off )	//Turn off global optimization. If the compiler optimizes the loop, it will be skipped and the program will probably freeze or crash
//Deprecated. Use lux::FenceDE
//TODO remove
struct LuxFence {
private:
	char8 value = 0;

public:
	//Deprecated. Use lux::FenceDE
	//Waits until the fence has the same value of v
	//A 2ms sleep every iteration uses way less CPU than iterating the loop every time, and the thread can do other things while waiting
	inline void wait(const char8 v) const { while(value != v) { sleep(1); } }

	//Deprecated. Use lux::FenceDE
	//Same as wait(const char) but it waits until the value is the same as a or b
	inline void wait(const char8 a, const char8 b) { while(value != a && value != b) { sleep(2); } }

	//Deprecated. Use lux::FenceDE
	//Returns true if the fence has the same value as v, false if not
	inline bool check(const char8 v) const { return value == v; }

	//Deprecated. Use lux::FenceDE
	//Assigns a value to the fence
	inline void set(const char8 v) { value = v; }
};
#pragma optimize( "g", on )		//Turn on global optimization
