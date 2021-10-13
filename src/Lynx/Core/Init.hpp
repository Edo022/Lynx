#pragma once
#include "Lynx/macros.hpp"
#include "Lynx/Types/Dummy.hpp"



// #define _pvt_uuid_cat2(a, b, c) a ## b ## c
// #define _pvt_uuid_cat(a, b, c) _pvt_uuid_cat2(a, b, c)
// #define _pvt_uuid_2(tokid) _pvt_uuid_cat(uuid, tokid, TU_UUID)
// #define UUID() _pvt_uuid_2(__COUNTER__) //TODO write condumentation

// #ifdef __INTELLISENSE__
// 	#define TU_UUID 0x0 //!Defined in compile time
// 	//TODO MOVE TO MacroUtils.hpp
// #endif



// /**
//  * @brief Concatenates up to 64 tokens
//  *     Extra tokens are ignored
//  */
// #define CAT(...) _pvt_cat64(
//     __VA_ARGS__,,,,,,,,,,,,,,,
//     ,,,,,,,,,,,,,,,,
//     ,,,,,,,,,,,,,,,,
//     ,,,,,,,,,,,,,,,,
// )
// #define _pvt_cat64(
//     _00, _01, _02, _03, _04, _05, _06, _07, _08, _09, _0a, _0b, _0c, _0d, _0e, _0f,
//     _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _1a, _1b, _1c, _1d, _1e, _1f,
//     _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _2a, _2b, _2c, _2d, _2e, _2f,
//     _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _3a, _3b, _3c, _3d, _3e, _3f,
// ...)
//     _00 ## _01 ## _02 ## _03 ## _04 ## _05 ## _06 ## _07 ## _08 ## _09 ## _0a ## _0b ## _0c ## _0d ## _0e ## _0f ##
//     _10 ## _11 ## _12 ## _13 ## _14 ## _15 ## _16 ## _17 ## _18 ## _19 ## _1a ## _1b ## _1c ## _1d ## _1e ## _1f ##
//     _20 ## _21 ## _22 ## _23 ## _24 ## _25 ## _26 ## _27 ## _28 ## _29 ## _2a ## _2b ## _2c ## _2d ## _2e ## _2f ##
//     _30 ## _31 ## _32 ## _33 ## _34 ## _35 ## _36 ## _37 ## _38 ## _39 ## _3a ## _3b ## _3c ## _3d ## _3e ## _3f
// //TODO MOVE TO MacroUtils.hpp




//TODO MOVE TO UTILS HEADER
#define DEL_P2(...) __VA_ARGS__
#define DEL_P(v) DEL_P2 v






/**
 * @brief Creates a global reference to a static variable which is initialized and constructed on its first use
 *     A _lnx_init_var_def in the translation unit is required to define and initialize the variable
 *     ! This macro is used to prevent the user from breaking the software by accessing uninitialized engine variables through classes which depend on them
 * @param type The type and cv qualifiers of the variable
 * @param name The name of the variable
 *
 *     e.g.
 *     _lnx_init_var_dec(f32v3, vector);
 */
#define _lnx_init_var_dec(      type, name) _lnx_init_var_dec2(      type, name, _lnx_init_var_##name)
#define _lnx_init_var_array_dec(type, name) _lnx_init_var_array_dec2(type, name, _lnx_init_var_##name)
#define _lnx_init_var_const_dec(type, name) _lnx_init_var_const_dec2(type, name, _lnx_init_var_##name)




// #define _lnx_init_var_dec2(type, name, id) \
	/*Get declaration*/                    \
	// namespace _pvt {                      \
	// 	used        type&          id##_get();                  \
	// 	used static type& id##_v = id##_get();  \
	// 	extern bool id##_is_init;/*Zero initialized before global constructors*/\
	// }                                      \
	\
	/*Reference definition*/               \
	_rls(used static alwaysInline type& g_##name(){ return _pvt::_lnx_init_var_##name##_v; })\\
	_dbg(used              inline type& g_##name())



// #define _lnx_init_var_array_dec2(type, name, id)
#define _lnx_init_var_dec2(type, name, id)\
	_rls(used static alwaysInline DEL_P(type)& g_##name(){ return *_pvt::id##_v; });\
	_dbg(used              inline DEL_P(type)& g_##name())

#define _lnx_init_var_array_dec2(type, name, id)\
	_rls(used static alwaysInline DEL_P(type)* g_##name(){ return _pvt::id##_v; });\
	_dbg(used              inline DEL_P(type)* g_##name())

#define _lnx_init_var_const_dec2(type, name, id)\
	_rls(used static alwaysInline const DEL_P(type)& g_##name(){ return _pvt::id##_v; });\
	_dbg(used              inline const DEL_P(type)& g_##name())


	/*Get declaration*/               \
	// namespace _pvt {                 \
	// 	used        DEL_P(type)          id##_get();             \
	// 	used static DEL_P(type) id##_v = id##_get(); \
	// 	extern bool id##_is_init;/*Zero initialized before global constructors*/\
	// }                                 \
	//\
	/*Reference definition*/          \
	// _rls(used static alwaysInline DEL_P(type) g_##name(){ return _pvt::_lnx_init_var_##name##_v; });\
	// _dbg(used              inline DEL_P(type) g_##name())







// #define _lnx_init_fun_def(tu, fullNs) _lnx_init_fun_def2(tu##_FUN, _lnx_init_fun_##tu##_FUN, fullNs)


#ifndef __LNX_INITIALIZER_GENERATOR__
	// #include <type_traits>
	// #define _lnx_init_var_const_def(type, name,        fullNs) _lnx_init_var_const_def2(type, name,        _lnx_init_var_##name)
	// #define _lnx_init_var_value_def(type, name,        fullNs) _lnx_init_var_value_def2(type, name,        _lnx_init_var_##name)
	// #define _lnx_init_var_array_def(type, name, count, fullNs) _lnx_init_var_array_def2(type, name, count, _lnx_init_var_##name)
	#define _lnx_init_var_redirect_value_def(type, name, fullNs) 				_lnx_init_var_redirect_value_def2(type, name, 1, _lnx_init_var_##name)
	#define _lnx_init_var_redirect_array_def(type, name, count, fullNs) _lnx_init_var_redirect_array_def2(type, name, count, _lnx_init_var_##name)
	#define _lnx_init_var_redirect_const_def(type, name, fullNs) _lnx_init_var_redirect_const_def2(type, name, 1, _lnx_init_var_##name)
#else
	// #define _lnx_init_var_const_def(type, name,        fullNs) _LNX_INITIALIZER_GENERATOR_TYPE=DEL_P(type),_LNX_INITIALIZER_GENERATOR_NAME=name,_LNX_INITIALIZER_GENERATOR_FULLNS=fullNs;
	// #define _lnx_init_var_value_def(type, name,        fullNs) _LNX_INITIALIZER_GENERATOR_TYPE=DEL_P(type),_LNX_INITIALIZER_GENERATOR_NAME=name,_LNX_INITIALIZER_GENERATOR_FULLNS=fullNs;
	// #define _lnx_init_var_array_def(type, name, count, fullNs) _LNX_INITIALIZER_GENERATOR_TYPE=DEL_P(type),_LNX_INITIALIZER_GENERATOR_NAME=name,_LNX_INITIALIZER_GENERATOR_FULLNS=fullNs;
	#define _lnx_init_var_redirect_value_def(type, name, fullNs)              _LNX_INITIALIZER_GENERATOR_TYPE=DEL_P(type),_LNX_INITIALIZER_GENERATOR_NAME=name,_LNX_INITIALIZER_GENERATOR_FULLNS=fullNs;
	#define _lnx_init_var_redirect_array_def(type, name, count, fullNs) _LNX_INITIALIZER_GENERATOR_TYPE=DEL_P(type),_LNX_INITIALIZER_GENERATOR_NAME=name,_LNX_INITIALIZER_GENERATOR_FULLNS=fullNs;
	#define _lnx_init_var_redirect_const_def(type, name, fullNs) _LNX_INITIALIZER_GENERATOR_TYPE=DEL_P(type),_LNX_INITIALIZER_GENERATOR_NAME=name,_LNX_INITIALIZER_GENERATOR_FULLNS=fullNs;
#endif









// /**
//  * @brief Definition and and initialization macro for varbiales declared with _lnx_init_var_dec
//  *     This macro writes the function that creates the static variable, the variable definition and the signature of the function used to initialize it
//  *
//  *     The initializer function returns a reference to the type of the init variable and takes no arguments
//  *     It will be automatically called when the variable is used for the first time or when the static initialization reaches its definition, whichever comes first
//  *     The type must have a public copy constructor or a public move constructor
//  *     To default construct the variable and initialize it manually, use _lnx_init_var_value_def //TODO rename macros
//  *     init variables in the same translation unit are constructed in the order in which they are defined
//  *
//  *     e.g.
//  *     _lnx_init_var_def(f32v3, vector){
//  *         f32v4 v = { 0, 1, 2 }
//  *         cin >> v[1];
//  *         return v;
//  *     }
//  * @param type The type and cv qualifiers of the init variable. It must match the type and cv qualifiers used in its declaration
//  * @param name The name of the init variable. It must match the name used in its declaration
//  * @param fulln The namespace containing the variable
//  */
// #define _lnx_init_var_const_def2(type, name, id)     \
// 	static_assert(std::is_const_v<DEL_P(type)>, "Non-const variable defined with \"_lnx_init_var_const_def\"");\
// 	namespace _pvt{                         \
// 		/*Initializer function declaration*/ \
// 		used DEL_P(type) id##_init_f();   \
// 	\
// 		/*is_init definition*/ \
// 		used bool id##_is_init;   \
// 	\
// 		/*Get function definition*/          \
// 		used DEL_P(type)* id##_get(){     \
// 			lnx::dbg::assertCond(!_pvt::id##_is_init, "Global initializer \"" #id "_get\" called twice");\
// 			used static DEL_P(type)* var = new (DEL_P(type))(id##_init_f());\
// 			return var;                     \
// 		}                                    \
// 	\
// 		/*Pointer definition*/\
// 		used DEL_P(type)* id##_v;\
// 	}                                        \
// 	\
// 	/*Debug getter definition*/\
// 	_dbg(used inline DEL_P(type)& g_##name(){)\
// 	_dbg(	lnx::dbg::assertCond(_pvt::id##_is_init, "Global variable \"g_" #name "\" used before initialization");)\
// 	_dbg(	return *_pvt::id##_v;)\
// 	_dbg(};)\
// 	\
// 	/*Initializer function definition*/      \
// 	used DEL_P(type) _pvt::id##_init_f()




// //TODO add used attribute
// /**
//  * @brief Definition and and initialization macro for varbiales declared with _lnx_init_var_dec
//  *     This macro writes the function that creates the static variable, the variable definition and the signature of the function used to initialize it
//  *
//  *     The initializer function provides a variable called "pVar", which is a non const rvalue reference to the default constructed variable that can be used to initialize it
//  *     The initializer function will be automatically called when the variable is used for the first time or when the static initialization reaches its definition, whichever comes first
//  *     The type must have a public default constructor
//  *     init variables in the same translation unit are constructed in the order in which they are defined
//  *
//  *     e.g.
//  *     _lnx_init_var_def(f32v3, vector){
//  *         pVar = { 0, 1, 2 }
//  *         cin >> pVar[1];
//  *     }
//  * @param type The type and cv qualifiers of the init variable. It must match the type and cv qualifiers used in its declaration
//  * @param name The name of the init variable. It must match the name used in its declaration
//  * @param fulln The namespace containing the variable
//  */
// #define _lnx_init_var_value_def2(type, name, id)          \
// 	namespace _pvt{                            \
// 		/*Initializer type definition*/    \
// 		struct id##_init_t{       \
// 			used id##_init_t(DEL_P(type)& pVar); \
// 		};                                      \
// 	\
// 		/*is_init definition*/ \
// 		used bool id##_is_init;   \
// 	\
// 		/*Get function definition*/             \
// 		used DEL_P(type)* id##_get(){        \
// 			lnx::dbg::assertCond(!_pvt::id##_is_init, "Global initializer \"" #id "_get\" called twice");\
// 			used static DEL_P(type)* var = new (DEL_P(type))();    \
// 			used static id##_init_t init_v(*var); \
// 			return var;                        \
// 		}                                       \
// 	\
// 		/*Pointer definition*/\
// 		used DEL_P(type)* id##_v;\
// 	}                                           \
// 	\
// 	/*Debug getter function*/\
// 	_dbg(used inline DEL_P(type)& g_##name(){)\
// 	_dbg(	lnx::dbg::assertCond(_pvt::id##_is_init, "Global variable \"g_" #name "\" used before initialization");)\
// 	_dbg(	return *_pvt::id##_v;)\
// 	_dbg(};)\
// 	\
// 	/*Initializer function definition*/         \
// 	used _pvt::id##_init_t::id##_init_t(DEL_P(type)& pVar)




// #define _lnx_init_var_array_def2(type, name, count, id)
#define _lnx_init_var_redirect_array_def2(type, name, count, id)     \
	namespace _pvt{                                \
		/*is_init definition*/ \
		used DEL_P(type)* id##_v;\
		used bool id##_is_init;   \
		id##_init_t_call_t::id##_init_t_call_t(){\
			if(!id##_is_init) {\
				/*id##_v = id##_get();*/\
				used static DEL_P(type)* var = new DEL_P(type)[count];             \
				set(var);\
				id##_v = var;\
				id##_is_init = true;\
			}\
		}\
		\
	}                                                       \
	\
	/*Debug getter function*/\
	_dbg(used inline DEL_P(type)* g_##name(){)\
	_dbg(	static_assert(!std::is_const_v<DEL_P(type)>, "Use _lnx_init_var_redirect_const_def to define const variables");)\
	_dbg(	lnx::dbg::assertCond(_pvt::id##_is_init, "Global variable \"g_" #name "\" used before initialization");)\
	_dbg(	return _pvt::id##_v;)\
	_dbg(};)\
	void _pvt::id##_init_t_call_t::set(DEL_P(type)* pVar)

	// static _rls(alwaysInline) _dbg(inline) type* g_##name(){
	// 	return _pvt::_lnx_init_var_##name##_v;
	// }







// #define _lnx_init_var_array_def2(type, name, count, id)
#define _lnx_init_var_redirect_value_def2(type, name, count, id)     \
	namespace _pvt{                                \
		/*is_init definition*/ \
		used DEL_P(type)* id##_v;\
		used bool id##_is_init;   \
		id##_init_t_call_t::id##_init_t_call_t(){\
			if(!id##_is_init) {\
				/*id##_v = id##_get();*/\
				used static DEL_P(type)* var = new DEL_P(type)[count];             \
				set(var);\
				id##_v = var;\
				id##_is_init = true;\
			}\
		}\
		\
	}                                                       \
	\
	/*Debug getter function*/\
	_dbg(used inline DEL_P(type)& g_##name(){)\
	_dbg(	static_assert(!std::is_const_v<DEL_P(type)>, "Use _lnx_init_var_redirect_const_def to define const variables");)\
	_dbg(	lnx::dbg::assertCond(_pvt::id##_is_init, "Global variable \"g_" #name "\" used before initialization");)\
	_dbg(	return *_pvt::id##_v;)\
	_dbg(};)\
	void _pvt::id##_init_t_call_t::set(DEL_P(type)* pVar)




// #define _lnx_init_var_array_def2(type, name, count, id)
#define _lnx_init_var_redirect_const_def2(type, name, count, id)     \
	namespace _pvt{                                \
		/*is_init definition*/ \
		used DEL_P(type)* id##_v;\
		used bool id##_is_init;   \
		id##_init_t_call_t::id##_init_t_call_t(){\
			if(!id##_is_init) {\
				/*id##_v = id##_get();*/\
				used static DEL_P(type)* var = new DEL_P(type)[count];             \
				set(var);\
				id##_v = var;\
				id##_is_init = true;\
			}\
		}\
		\
	}                                                       \
	\
	/*Debug getter function*/\
	_dbg(used inline const DEL_P(type)& g_##name(){)\
	_dbg(	static_assert(!std::is_const_v<DEL_P(type)>, "Const type used in const definition");)\
	_dbg(	lnx::dbg::assertCond(_pvt::id##_is_init, "Global variable \"g_" #name "\" used before initialization");)\
	_dbg(	return *_pvt::id##_v;)\
	_dbg(};)\
	void _pvt::id##_init_t_call_t::set(DEL_P(type)* pVar)

	// static _rls(alwaysInline) _dbg(inline) type* g_##name(){
	// 	return _pvt::_lnx_init_var_##name##_v;
	// }










//FIXME MERGE DEFINITIONS

//FIXME PASS THE POINTER WITH THE TYPE WHEN USING ARRAYS



// // file://./main.cpp
// // #define _lnx_init_fun_dec2(id)
// #define _lnx_init_fun_def2(tu, id, fullNs)                                                                  \
// 	namespace _pvt{                                                                     \
// 		struct id##_t{                                                   \
// 			used id##_t(); \
// 		};                                                                               \
// 	}                                                                                    \
// 	_lnx_init_var_value_def((_pvt::id##_t), tu, fullNs){}                              \
// 	used _pvt::id##_t::id##_t() //{
// 		//Implementation
// 		//...
// 	//}

// //Creates a function that will be called during the static initialization
// //This macro can only be used in the global scope of .cpp files
// // #define _lnx_init_fun_def() _lnx_init_fun_dec2(UUID())


// #define _lnx_init_fun_dec(tu) _lnx_init_fun_dec2(tu##_FUN, _lnx_init_fun_##tu##_FUN)
// #define _lnx_init_fun_dec2(tu, id)          \
// 	namespace _pvt {                  \
// 		struct id##_t; \
// 	}                                  \
// 	_lnx_init_var_dec((_pvt::id##_t), tu)

#define _lnx_init_fun_dec(tu) _lnx_init_var_dec((Dummy), tu##_FUN)
#define _lnx_init_fun_def(tu, fullNs) _lnx_init_var_redirect_value_def((Dummy), tu##_FUN, fullNs)
