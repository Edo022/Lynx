#pragma once


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


#include "Lynx/macros.hpp"




// #define LnxAutoInit2(id)
#define LnxAutoInit(tu)                                                                  \
	namespace __pvt{                                                                     \
		struct _lnx_init_fun_##tu##_t{                                                   \
			__attribute__((used)) _lnx_init_fun_##tu##_t(); \
		};                                                                               \
	}                                                                                    \
	__init_var_set_def(__pvt::_lnx_init_fun_##tu##_t, tu){}                              \
	__pvt::_lnx_init_fun_##tu##_t::_lnx_init_fun_##tu##_t() //{
		//Implementation
		//...
	//}

//Creates a function that will be called during the static initialization
//This macro can only be used in the global scope of .cpp files
// #define LnxAutoInit() LnxAutoInit2(UUID())



#define _lnx_init_fun_dec(tu)          \
	namespace __pvt {                  \
		struct _lnx_init_fun_##tu##_t; \
	}                                  \
	__init_var_dec(__pvt::_lnx_init_fun_##tu##_t, tu)