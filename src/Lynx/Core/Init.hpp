#pragma once
//####################################################################################'
// Those macros are used to prevent the user from breaking the software by accessing
// uninitialized engine variables through classes which depend on them
//####################################################################################\n'


#ifndef __LNX_PARSE_NO_INCLUDES__
	#include "Lynx/Utils.hpp"
	#include "Lynx/Types/Dummy.hpp"
#endif








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





#define _lnx_init_fun_dec(tu) _lnx_init_var_dec((Dummy), tu##_FUN)
#define _lnx_init_fun_def(tu, fullNs) _lnx_init_var_value_def((Dummy), tu##_FUN, fullNs)

#define _lnx_init_var_dec(      type, name) _lnx_init_var_dec2(      type, name, _lnx_init_var_##name)
#define _lnx_init_var_array_dec(type, name) _lnx_init_var_array_dec2(type, name, _lnx_init_var_##name)
#define _lnx_init_var_const_dec(type, name) _lnx_init_var_const_dec2(type, name, _lnx_init_var_##name)



#define _lnx_init_var_dec2(type, name, id)\
	_rls(used static alwaysInline DEL_P(type)& g_##name(){ return *_pvt::id##_v; });\
	_dbg(used              inline DEL_P(type)& g_##name())

#define _lnx_init_var_array_dec2(type, name, id)\
	_rls(used static alwaysInline DEL_P(type)* g_##name(){ return _pvt::id##_v; });\
	_dbg(used              inline DEL_P(type)* g_##name())

#define _lnx_init_var_const_dec2(type, name, id)\
	_rls(used static alwaysInline const DEL_P(type)& g_##name(){ return *_pvt::id##_v; });\
	_dbg(used              inline const DEL_P(type)& g_##name())





#ifndef __LNX_PARSE_INITIALIZER_GENERATOR__
	#define _lnx_init_var_value_def(type, name, fullNs)        _lnx_init_var_value_def2(type, name,     1, _lnx_init_var_##name)
	#define _lnx_init_var_array_def(type, name, count, fullNs) _lnx_init_var_array_def2(type, name, count, _lnx_init_var_##name)
	#define _lnx_init_var_const_def(type, name, fullNs)        _lnx_init_var_const_def2(type, name,     1, _lnx_init_var_##name)
#else
	#define _lnx_init_var_value_def(type, name, fullNs)        _LNX_INITIALIZER_GENERATOR_TYPE=DEL_P(type),_LNX_INITIALIZER_GENERATOR_NAME=name,_LNX_INITIALIZER_GENERATOR_FULLNS=fullNs;
	#define _lnx_init_var_array_def(type, name, count, fullNs) _LNX_INITIALIZER_GENERATOR_TYPE=DEL_P(type),_LNX_INITIALIZER_GENERATOR_NAME=name,_LNX_INITIALIZER_GENERATOR_FULLNS=fullNs;
	#define _lnx_init_var_const_def(type, name, fullNs)        _LNX_INITIALIZER_GENERATOR_TYPE=DEL_P(type),_LNX_INITIALIZER_GENERATOR_NAME=name,_LNX_INITIALIZER_GENERATOR_FULLNS=fullNs;
#endif




#define _lnx_init_var_array_def2(type, name, count, id)     \
	_lnx_init_var_call_t_definition(type, name, count, id)\
	\
	/*Debug getter function*/\
	_dbg(used inline DEL_P(type)* g_##name(){)\
	_dbg(	static_assert(!std::is_const_v<DEL_P(type)>, "Use _lnx_init_var_const_def to define const variables");)\
	_dbg(	lnx::dbg::assertCond(_pvt::id##_is_init, "Global variable \"g_" #name "\" used before initialization");)\
	_dbg(	return _pvt::id##_v;)\
	_dbg(};)\
	void _pvt::id##_init_t_call_t::set(DEL_P(type)* pVar)




#define _lnx_init_var_value_def2(type, name, count, id)     \
	_lnx_init_var_call_t_definition(type, name, count, id)\
	\
	/*Debug getter function*/\
	_dbg(used inline DEL_P(type)& g_##name(){)\
	_dbg(	static_assert(!std::is_const_v<DEL_P(type)>, "Use _lnx_init_var_const_def to define const variables");)\
	_dbg(	lnx::dbg::assertCond(_pvt::id##_is_init, "Global variable \"g_" #name "\" used before initialization");)\
	_dbg(	return *_pvt::id##_v;)\
	_dbg(};)\
	void _pvt::id##_init_t_call_t::set(DEL_P(type)* pVar)




#define _lnx_init_var_const_def2(type, name, count, id)     \
	_lnx_init_var_call_t_definition(type, name, count, id)\
	\
	/*Debug getter function*/\
	_dbg(used inline const DEL_P(type)& g_##name(){)\
	_dbg(	static_assert(!std::is_const_v<DEL_P(type)>, "Const type used in const definition");)\
	_dbg(	lnx::dbg::assertCond(_pvt::id##_is_init, "Global variable \"g_" #name "\" used before initialization");)\
	_dbg(	return *_pvt::id##_v;)\
	_dbg(};)\
	void _pvt::id##_init_t_call_t::set(DEL_P(type)* pVar)








#define _lnx_init_var_call_t_definition(type, name, count, id)\
	namespace _pvt{                                \
		used DEL_P(type)* id##_v;\
		used bool id##_is_init;   \
		id##_init_t_call_t::id##_init_t_call_t(){\
			if(!id##_is_init) {\
				used static DEL_P(type)* var = new DEL_P(type)[count];             \
				set(var);\
				id##_v = var;\
				id##_is_init = true;\
			}\
		}\
		\
	}










//FIXME PASS THE POINTER WITH THE TYPE WHEN USING ARRAYS

