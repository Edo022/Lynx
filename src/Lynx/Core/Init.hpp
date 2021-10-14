#pragma once
//####################################################################################'
// Those macros are used to prevent the user from breaking the software by accessing
// uninitialized engine variables through classes which depend on them
//####################################################################################\n'


#include "Lynx/Utils.hpp"
#include "Lynx/Types/Dummy.hpp"




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

