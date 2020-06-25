#pragma once


#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "LuxEngine/Types/Integers/Integers.h"

#include <cinttypes>
#include "LuxEngine/Math/Algebra/Algebra.h"


#pragma warning( disable: 26451 )
#pragma warning( disable: 4003 )	//No enough arguments for macro
//type: the type of the function and the vectors
//typen: the type but with the capital letter
//pass: & to bass by reference vectors larger than 3 byte for better performances
#define __lp_vec_def(type, typen, pass)																																			\
typedef _vec2<type> vec2##type;																																					\
typedef _vec3<type> vec3##type;																																					\
typedef _vec4<type> vec4##type;																																					\
static inline type __vectorcall dist##typen(const _vec2<type>##pass a, const _vec2<type>##pass b) { return scast<type>(sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2))); }						\
static inline type __vectorcall dist##typen(const _vec3<type>##pass a, const _vec3<type>##pass b) { return scast<type>(sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2) + pow(b.z - a.z, 2))); }	\
static inline type __vectorcall dist##typen(const _vec4<type>& a, const _vec4<type>& b) { return scast<type>(sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2) + pow(b.z - a.z, 2) + pow(b.w - a.w, 2))); }

__lp_vec_def(int8, Int8)			__lp_vec_def(uint8, Uint8)			__lp_vec_def(bool, Bool)		__lp_vec_def(char, Char)
__lp_vec_def(int16, Int16, &)		__lp_vec_def(uint16, Uint16, &)	
__lp_vec_def(int32, Int32, &)		__lp_vec_def(uint32, Uint32, &)		__lp_vec_def(float, Float, &)
__lp_vec_def(int64, Int64, &)		__lp_vec_def(uint64, Uint64, &)		__lp_vec_def(double, Double, &)
#undef __lp_vec_def
#pragma warning( default: 26451 )
#pragma warning( default: 4003 )	//No enough arguments for macro

