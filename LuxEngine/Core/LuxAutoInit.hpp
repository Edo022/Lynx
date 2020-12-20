
//Creates a function that will be called during the global variable initialization
//This macro can only be used in the global scope of .cpp files
#define luxAutoInit(tu)                            		\
	namespace __pvt{									\
		void tu##_AutoInit_f();							\
		struct tu##_AutoInit_t{                   		\
			tu##_AutoInit_t() { tu##_AutoInit_f(); }	\
		};												\
		tu##_AutoInit_t tu##_Initializer;				\
	}													\
	void __pvt::tu##_AutoInit_f() //{ ... } 			//Implementation
