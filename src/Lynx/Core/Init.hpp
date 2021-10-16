#pragma once
//####################################################################################'
// Those macros are used to prevent the user from breaking the software by accessing
// uninitialized engine variables through classes which depend on them
//####################################################################################\n'


#include "Lynx/Utils.hpp"
#include "Lynx/Types/Dummy.hpp"




#define _lnx_init_var_value_dec(      vType, vName) _lnx_init_var_dec2(vName, _lnx_init_var_##vName, (      DEL_P(vType)&), *)
#define _lnx_init_var_array_dec(      vType, vName) _lnx_init_var_dec2(vName, _lnx_init_var_##vName, (      DEL_P(vType)*),  )
#define _lnx_init_var_value_const_dec(vType, vName) _lnx_init_var_dec2(vName, _lnx_init_var_##vName, (const DEL_P(vType)&), *)
#define _lnx_init_var_array_const_dec(vType, vName) _lnx_init_var_dec2(vName, _lnx_init_var_##vName, (const DEL_P(vType)*),  )

#define _lnx_init_var_dec2(vName, vId, vEndType, vDeref)\
	_rls(used static alwaysInline DEL_P(vEndType) g_##vName(){ return vDeref _pvt::vId##_v; })\
	_dbg(used              inline DEL_P(vEndType) g_##vName())




#ifndef __LNX_PARSE_INITIALIZER_GENERATOR__
	#define _lnx_init_var_value_def(      vType, vName,         vFullNamespace) _lnx_init_var_def2(vType, vName,      1, _lnx_init_var_##vName, (      DEL_P(vType)&), *)
	#define _lnx_init_var_array_def(      vType, vName, vCount, vFullNamespace) _lnx_init_var_def2(vType, vName, vCount, _lnx_init_var_##vName, (      DEL_P(vType)*),  )
	#define _lnx_init_var_value_const_def(vType, vName,         vFullNamespace) _lnx_init_var_def2(vType, vName,      1, _lnx_init_var_##vName, (const DEL_P(vType)&), *)
	#define _lnx_init_var_array_const_def(vType, vName, vCount, vFullNamespace) _lnx_init_var_def2(vType, vName, vCount, _lnx_init_var_##vName, (const DEL_P(vType)*),  )
#else
	#define _lnx_init_var_value_def(      vType, vName,         vFullNamespace) _LNX_PARSE_INITIALIZER_GENERATOR_TYPE=DEL_P(vType),_LNX_PARSE_INITIALIZER_GENERATOR_NAME=vName,_LNX_PARSE_INITIALIZER_GENERATOR_FULLNS=vFullNamespace;
	#define _lnx_init_var_array_def(      vType, vName, vCount, vFullNamespace) _LNX_PARSE_INITIALIZER_GENERATOR_TYPE=DEL_P(vType),_LNX_PARSE_INITIALIZER_GENERATOR_NAME=vName,_LNX_PARSE_INITIALIZER_GENERATOR_FULLNS=vFullNamespace;
	#define _lnx_init_var_value_const_def(vType, vName,         vFullNamespace) _LNX_PARSE_INITIALIZER_GENERATOR_TYPE=DEL_P(vType),_LNX_PARSE_INITIALIZER_GENERATOR_NAME=vName,_LNX_PARSE_INITIALIZER_GENERATOR_FULLNS=vFullNamespace;
	#define _lnx_init_var_array_const_def(vType, vName, vCount, vFullNamespace) _LNX_PARSE_INITIALIZER_GENERATOR_TYPE=DEL_P(vType),_LNX_PARSE_INITIALIZER_GENERATOR_NAME=vName,_LNX_PARSE_INITIALIZER_GENERATOR_FULLNS=vFullNamespace;
#endif




#define _lnx_init_var_def2(vType, vName, vCount, vId, vEndType, vDeref)   \
	/*Initialization functions definition*/                               \
	namespace _pvt{                                                       \
		used DEL_P(vType)* vId##_v;                                       \
		used bool vId##_is_init;                                          \
		vId##_init_t_call_t::vId##_init_t_call_t(){                       \
			if(!vId##_is_init) {                                          \
				used static DEL_P(vType)* var = new DEL_P(vType)[vCount]; \
				set(var);                                                 \
				vId##_v = var;                                            \
				vId##_is_init = true;                                     \
			}                                                             \
		}                                                                 \
	\
	}                                                                     \
	/*Debug get function definition*/                                     \
	_dbg(used inline DEL_P(vEndType) g_##vName(){                         \
		lnx::dbg::assertCond(                                             \
			_pvt::vId##_is_init,                                          \
			"Global variable \"g_" #vName "\" used before initialization" \
		);                                                                \
		return vDeref _pvt::vId##_v;                                      \
	};)                                                                   \
	void _pvt::vId##_init_t_call_t::set(DEL_P(vType)* pVar)




#define _lnx_init_fun_dec(vTranslationUnit)                 _lnx_init_var_value_dec((Dummy), vTranslationUnit##_FUN)
#define _lnx_init_fun_def(vTranslationUnit, vFullNamespace) _lnx_init_var_value_def((Dummy), vTranslationUnit##_FUN, vFullNamespace)
