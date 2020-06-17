
#pragma once



struct LuxFence {
	char value = 0;

	inline void wait(char v) { while (value != v) {} }
	inline void set(char v) { value = v; }
};