
#pragma once
#include "LuxEngine/macros.h"
#include "Integers/Integers.h"
#include <thread>
#include <mutex>


//TODO update use thread pool fence system
//A struct used to synchronize threads
#pragma optimize( "g", off )	//Turn off global optimization. If the compiler optimizes the loop, it will be skipped and the program will probably freeze or crash
struct LuxFence {
private:
	char8 value = 0;

public:
	//Waits until the fence has the same value of v
	//A 2ms sleep every iteration uses way less CPU than iterating the loop every time, and the thread can do other things while waiting
	inline void wait(const char8 v) const {
		//Failure printf(" - Waiting %d from    %d", v, std::this_thread::get_id());
		while (value != v) { sleep(1); }
		//Failure printf(" - WaitED %d from     %d", v, std::this_thread::get_id());
	}

	//Same as wait(const char) but it waits until the value is the same as a or b
	inline void wait(const char8 a, const char8 b) { while (value != a && value != b) { sleep(2); } }

	//Returns true if the fence has the same value as v, false if not
	inline bool check(const char8 v) const {
		//Failure printf(" + Checked %d from    %d\t:%s", v, std::this_thread::get_id(), (v == value) ? "true" : "false");
		return value == v;
	}
	//Assigns a value to the fence
	inline void set(const char8 v) {
		//Failure printf(" + Setted %d from     %d", v, std::this_thread::get_id());
		value = v;
	}
};
#pragma optimize( "g", on )		//Turn on global optimization

