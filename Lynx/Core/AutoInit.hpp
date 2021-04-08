#pragma once


//Creates a function that will be called during the global variable initialization
//This macro can only be used in the global scope of .cpp files
#define LnxAutoInit(tu)															\
	namespace __pvt{															\
		void __lainit_##tu##_f();												\
		struct __lainit_##tu##_t{												\
			__attribute__((used)) __lainit_##tu##_t() { __lainit_##tu##_f(); }	\
		};																		\
		__attribute__((used)) __lainit_##tu##_t __lainit_##tu##_v;				\
	}																			\
	void __pvt::__lainit_##tu##_f() //{
		//Implementation
		//...
	//}


