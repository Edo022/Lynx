
//Creates a function that will be called during the global variable initialization
//This macro can only be used in the global scope of .cpp files
#define luxAutoInit(tu)                            		\
	namespace luxAutoInitFunc{							\
		void tu##_AutoInit_f();							\
		struct tu##_AutoInit_t{                   		\
			tu##_AutoInit_t() { tu##_AutoInit_f(); }	\
		};												\
		tu##_AutoInit_t tu##_Initializer;				\
	}													\
	void luxAutoInitFunc::tu##_AutoInit_f() //{ ... } 	//Implementation
