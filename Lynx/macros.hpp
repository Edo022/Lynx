#pragma once
#define LNX_H_MACROS


#ifdef __INTELLISENSE__
	#include ".engine/conf.hpp"
	//!The file is generated in the .engine directory of the user application when they change the build configuration
	//!This include is only used to allow Intellisense to correctly parse the code
#endif




#undef max
#undef min
//Functions
template<class ta, class tb>						static inline constexpr auto max(const ta a, const tb b) { return (a > b) ? a : b; } //Returns the minimum value
template<class ta, class tb>						static inline constexpr auto min(const ta a, const tb b) { return (a < b) ? a : b; } //Returns the maximum value
template<class ta, class tb, class tc, class ...tn> static inline constexpr auto max(const ta a, const tb b, const tc c, const tn... n) { return max(a, b, c, n...); }
template<class ta, class tb, class tc, class ...tn> static inline constexpr auto min(const ta a, const tb b, const tc c, const tn... n) { return min(a, b, c, n...); }




#ifdef LNX_DBG
#	define _dbg(...) __VA_ARGS__	//Executes a line of code only if in debug   mode
#	define _rls(...)				//Executes a line of code only if in release mode
#else
#	define _dbg(...)				//Executes a line of code only if in debug   mode
#	define _rls(...) __VA_ARGS__	//Executes a line of code only if in release mode
#endif




//Im lazy UwU
#define scast static_cast
#define rcast reinterpret_cast
#define noop  ((void)0)
#define alwaysInline __attribute__((__always_inline__,warning("function marked alwaysInline cannot be inlined"))) inline
#define neverInline  __attribute__((__noinline__))

#define alignVar(n) __attribute__((aligned(n )))
#define alignCache  __attribute__((aligned(64)))


#ifdef LNX_DBG
	#undef alwaysInline
	#define alwaysInline inline
#endif


//Time
#include <chrono>
#include <thread>
#define sleep(ms)					std::this_thread::sleep_for(std::chrono::milliseconds(ms))




#ifdef __GNUC__
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpmf-conversions"
	/**
	 * @brief Returns true if the object's class redefines a virtual member function of a base class
	 *     e.g. if(doesRedefine(derivedInstance, &Obj_bb::func)) //...do something
	 * @param object An instance of the derived class
	 * @param vVMFP The virtual member function pointer of the base class
	 */
	#define doesRedefine(vObj, vVMFP) ((void*)((vObj).*(vVMFP)) != (void*)(vVMFP))
	#pragma GCC diagnostic pop
#else
	static neverInline __attribute__((optimize("O0"), error("\"doesRedefine\" macro is only available in g++"))) bool doesRedefine(auto vObj, auto vVMFP){ return true; }
#endif




//Returns the relative path to the engine files
neverInline const char* getEnginePath();

#ifdef __INTELLISENSE__
	//FIXME USE EXTERN CONST CHAR INSTEAD OF MACRO AND FUNCTION
	//Path to the engine files. Generated during user app compilation
	//This macro may only be used by the user application
	//DO NOT use this macro in the engine code. Call getEnginePath() instead.
	#define enginePath "<generated>"
#endif
















//FIXME MOVE TO DEDICATED HEADER

/**
 * @brief Creates a global reference to a static variable which is initialized and constructed on its first use
 *     A __init_var_def in the translation unit is required to define and initialize the variable
 *     ! This macro is used to prevent the user from breaking the software by accessing uninitialized engine variables through classes which depend on them
 *     ! Using global variables is strongly discouraged and should be avoided whenever possible
 * @param type The type and cv qualifiers of the variable
 * @param name The name of the variable
 *
 *     e.g.
 *     __init_var_dec(f32v3, vector);
 */
#define __init_var_dec(type, name, ...)  \
	/*Get declaration*/                  \
	namespace __pvt {                    \
		type& __init_var_##name##_get(); \
	}                                    \
	\
	/*Reference definition*/             \
	static type& name = __pvt::__init_var_##name##_get() //! Put a semicolon after calling the macro



#define __init_var_array_dec(type, name, ...)\
	/*Get declaration*/                  \
	namespace __pvt {                    \
		type* __init_var_##name##_get(); \
	}                                    \
	\
	/*Reference definition*/             \
	static type* name = __pvt::__init_var_##name##_get() //! Put a semicolon after calling the macro





/**
 * @brief Definition and and initialization macro for varbiales declared with __init_var_dec
 *     This macro writes the function that creates the static variable, the variable definition and the signature of the function used to initialize it
 *
 *     The initializer function returns a reference to the type of the init variable and takes no arguments
 *     It will be automatically called when the variable is used for the first time or when the static initialization reaches its definition, whichever comes first
 *     The type must have a public copy constructor or a public move constructor
 *     To default construct the variable and initialize it manually, use __init_var_set_def //TODO rename macros
 *     init variables in the same translation unit are constructed in the order in which they are defined
 *
 *     e.g.
 *     __init_var_def(f32v3, vector){
 *         f32v4 v = { 0, 1, 2 }
 *         cin >> v[1];
 *         return v;
 *     }
 * @param type The type and cv qualifiers of the init variable. It must match the type and cv qualifiers used in its declaration
 * @param name The name of the init variable. It must match the name used in its declaration
 * @param fulln The namespace containing the variable
 */
#define __init_var_const_def(type, name)     \
	static_assert(std::is_const_v<type>, "Non-const variable defined with \"__init_var_const_def\"");\
	namespace __pvt{                         \
		/*Initializer function declaration*/ \
		type __init_var_##name##_init_f();   \
	\
		/*Get function definition*/          \
		type& __init_var_##name##_get(){     \
			static type* var = new (type)(__init_var_##name##_init_f());\
			return *var;                     \
		}                                    \
	}                                        \
	\
	/*Initializer function definition*/      \
	type __pvt::__init_var_##name##_init_f()





/**
 * @brief Definition and and initialization macro for varbiales declared with __init_var_dec
 *     This macro writes the function that creates the static variable, the variable definition and the signature of the function used to initialize it
 *
 *     The initializer function provides a variable called "pVar", which is a non const rvalue reference to the default constructed variable that can be used to initialize it
 *     The initializer function will be automatically called when the variable is used for the first time or when the static initialization reaches its definition, whichever comes first
 *     The type must have a public default constructor
 *     init variables in the same translation unit are constructed in the order in which they are defined
 *
 *     e.g.
 *     __init_var_def(f32v3, vector){
 *         pVar = { 0, 1, 2 }
 *         cin >> pVar[1];
 *     }
 * @param type The type and cv qualifiers of the init variable. It must match the type and cv qualifiers used in its declaration
 * @param name The name of the init variable. It must match the name used in its declaration
 * @param fulln The namespace containing the variable
 */
#define __init_var_set_def(type, name)          \
	namespace __pvt{                            \
		/*Initializer function declaration*/    \
		class __init_var_##name##_init_t{       \
			public:                             \
			__init_var_##name##_init_t(type& pVar); \
		};                                      \
	\
		/*Get function definition*/             \
		type& __init_var_##name##_get(){        \
			static type* var = new (type)();    \
			static __init_var_##name##_init_t init_v(*var); \
			return *var;                        \
		}                                       \
	}                                           \
	\
	/*Initializer function definition*/         \
	__pvt::__init_var_##name##_init_t::__init_var_##name##_init_t(type& pVar)




#define __init_var_array_def(type, name, count)     \
	namespace __pvt{                                \
		/*Initializer function declaration*/        \
		class __init_var_##name##_init_t{           \
			public:                                 \
			__init_var_##name##_init_t(type* pVar); \
		};                                          \
	\
		/*Get function definition*/                         \
		type* __init_var_##name##_get(){                    \
			static type* var = new type[count];             \
			static __init_var_##name##_init_t init_v(var);  \
			return var;                                     \
		}                                                   \
	}                                                       \
	\
	/*Initializer function definition*/                     \
	__pvt::__init_var_##name##_init_t::__init_var_##name##_init_t(type* pVar)





//TODO MOVE AND RENAME _lnx_init_var_...












// //Calculates the square root of a float number
// static float __s(float n) {
// 	static int32_t i = 0x5F3759DF - (*(int32_t*)&n >> 1);
// 	static float n2 = *(float*)&i;
// 	return scast<float>(1 / (n2 * (1.5 - (n * 0.5 * n2 * n2))));
// }

// //Calculates the result of b to the power of e. Way faster than math.h pow function
// static int64_t pow___(int64_t b, int64_t e) {
// 	int64_t r = 1;
// 	while (e > 0) {
// 		if (e & 1) r *= b % 1000000007;
// 		b *= b % 1000000007;
// 		e >>= 1;
// 	}
// 	return r;
// }