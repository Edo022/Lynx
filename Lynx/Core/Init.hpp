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




/**
 * @brief Creates a global reference to a static variable which is initialized and constructed on its first use
 *     A _lnx_init_var_def in the translation unit is required to define and initialize the variable
 *     ! This macro is used to prevent the user from breaking the software by accessing uninitialized engine variables through classes which depend on them
 *     ! Using global variables is strongly discouraged and should be avoided whenever possible
 * @param type The type and cv qualifiers of the variable
 * @param name The name of the variable
 *
 *     e.g.
 *     _lnx_init_var_dec(f32v3, vector);
 */
#define _lnx_init_var_dec(type, name, ...)  \
	/*Get declaration*/                  \
	namespace __pvt {                    \
		type& _lnx_init_var_##name##_get(); \
	}                                    \
	\
	/*Reference definition*/             \
	static type& name = __pvt::_lnx_init_var_##name##_get() //! Put a semicolon after calling the macro



#define _lnx_init_var_array_dec(type, name, ...)\
	/*Get declaration*/                  \
	namespace __pvt {                    \
		type* _lnx_init_var_##name##_get(); \
	}                                    \
	\
	/*Reference definition*/             \
	static type* name = __pvt::_lnx_init_var_##name##_get() //! Put a semicolon after calling the macro





/**
 * @brief Definition and and initialization macro for varbiales declared with _lnx_init_var_dec
 *     This macro writes the function that creates the static variable, the variable definition and the signature of the function used to initialize it
 *
 *     The initializer function returns a reference to the type of the init variable and takes no arguments
 *     It will be automatically called when the variable is used for the first time or when the static initialization reaches its definition, whichever comes first
 *     The type must have a public copy constructor or a public move constructor
 *     To default construct the variable and initialize it manually, use _lnx_init_var_set_def //TODO rename macros
 *     init variables in the same translation unit are constructed in the order in which they are defined
 *
 *     e.g.
 *     _lnx_init_var_def(f32v3, vector){
 *         f32v4 v = { 0, 1, 2 }
 *         cin >> v[1];
 *         return v;
 *     }
 * @param type The type and cv qualifiers of the init variable. It must match the type and cv qualifiers used in its declaration
 * @param name The name of the init variable. It must match the name used in its declaration
 * @param fulln The namespace containing the variable
 */
#define _lnx_init_var_const_def(type, name)     \
	static_assert(std::is_const_v<type>, "Non-const variable defined with \"_lnx_init_var_const_def\"");\
	namespace __pvt{                         \
		/*Initializer function declaration*/ \
		type _lnx_init_var_##name##_init_f();   \
	\
		/*Get function definition*/          \
		type& _lnx_init_var_##name##_get(){     \
			static type* var = new (type)(_lnx_init_var_##name##_init_f());\
			return *var;                     \
		}                                    \
	}                                        \
	\
	/*Initializer function definition*/      \
	type __pvt::_lnx_init_var_##name##_init_f()





/**
 * @brief Definition and and initialization macro for varbiales declared with _lnx_init_var_dec
 *     This macro writes the function that creates the static variable, the variable definition and the signature of the function used to initialize it
 *
 *     The initializer function provides a variable called "pVar", which is a non const rvalue reference to the default constructed variable that can be used to initialize it
 *     The initializer function will be automatically called when the variable is used for the first time or when the static initialization reaches its definition, whichever comes first
 *     The type must have a public default constructor
 *     init variables in the same translation unit are constructed in the order in which they are defined
 *
 *     e.g.
 *     _lnx_init_var_def(f32v3, vector){
 *         pVar = { 0, 1, 2 }
 *         cin >> pVar[1];
 *     }
 * @param type The type and cv qualifiers of the init variable. It must match the type and cv qualifiers used in its declaration
 * @param name The name of the init variable. It must match the name used in its declaration
 * @param fulln The namespace containing the variable
 */
#define _lnx_init_var_set_def(type, name)          \
	namespace __pvt{                            \
		/*Initializer function declaration*/    \
		class _lnx_init_var_##name##_init_t{       \
			public:                             \
			_lnx_init_var_##name##_init_t(type& pVar); \
		};                                      \
	\
		/*Get function definition*/             \
		type& _lnx_init_var_##name##_get(){        \
			static type* var = new (type)();    \
			static _lnx_init_var_##name##_init_t init_v(*var); \
			return *var;                        \
		}                                       \
	}                                           \
	\
	/*Initializer function definition*/         \
	__pvt::_lnx_init_var_##name##_init_t::_lnx_init_var_##name##_init_t(type& pVar)




#define _lnx_init_var_array_def(type, name, count)     \
	namespace __pvt{                                \
		/*Initializer function declaration*/        \
		class _lnx_init_var_##name##_init_t{           \
			public:                                 \
			_lnx_init_var_##name##_init_t(type* pVar); \
		};                                          \
	\
		/*Get function definition*/                         \
		type* _lnx_init_var_##name##_get(){                    \
			static type* var = new type[count];             \
			static _lnx_init_var_##name##_init_t init_v(var);  \
			return var;                                     \
		}                                                   \
	}                                                       \
	\
	/*Initializer function definition*/                     \
	__pvt::_lnx_init_var_##name##_init_t::_lnx_init_var_##name##_init_t(type* pVar)
















// #define LnxAutoInit2(id)
#define LnxAutoInit(tu)                                                                  \
	namespace __pvt{                                                                     \
		struct _lnx_init_fun_##tu##_t{                                                   \
			__attribute__((used)) _lnx_init_fun_##tu##_t(); \
		};                                                                               \
	}                                                                                    \
	_lnx_init_var_set_def(__pvt::_lnx_init_fun_##tu##_t, tu){}                              \
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
	_lnx_init_var_dec(__pvt::_lnx_init_fun_##tu##_t, tu)