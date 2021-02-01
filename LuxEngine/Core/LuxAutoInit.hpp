
//Creates a function that will be called during the global variable initialization
//This macro can only be used in the global scope of .cpp files
#define luxAutoInit(tu)                            		\
	namespace __pvt{									\
		void __lainit##tu##_f();						\
		struct __lainit##tu##_t{                   		\
			__lainit##tu##_t() { __lainit##tu##_f(); }	\
		};												\
		__lainit##tu##_t __lainit##tu##_v;				\
	}													\
	void __pvt::__lainit##tu##_f() //{ ... } 			//Implementation
