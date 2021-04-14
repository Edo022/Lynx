#pragma once


//Creates a function that will be called during the global variable initialization
//This macro can only be used in the global scope of .cpp files

#define __catm(a, b, c) a ## b ## c
#define __cat(a, b, c) __catm(a, b, c)

#define LnxAutoInit2(tui, idx)														\
	namespace __pvt{																\
		void __cat(tui, idx, _f)();													\
		struct __cat(tui, idx, _t){													\
			__attribute__((used)) __cat(tui, idx, _t)() { __cat(tui, idx, _f)(); }	\
		};																			\
		__attribute__((used)) __cat(tui, idx, _t) __cat(tui, idx, _v);				\
	}																				\
	void __pvt:: __cat(tui, idx, _f)() //{
		//Implementation
		//...
	//}


#define LnxAutoInit(tui) LnxAutoInit2(__LYNX_AUTO_INIT_##tui##_, __COUNTER__)