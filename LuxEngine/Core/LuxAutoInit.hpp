//An initializer function automatically called after the default initialization of the translation unit's variables
//This function cannot be used in headers
#define AutoInit(tu)                             	\
	void tu##_AutoInit();							\
	struct tu##_AutoInitializer{                   	\
		tu##_AutoInitializer() { tu##_AutoInit(); }	\
	};												\
	tu##_AutoInitializer tu##_Initializer;			\
	void tu##_AutoInit()
