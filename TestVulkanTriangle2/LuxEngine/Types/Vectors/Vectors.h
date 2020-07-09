#pragma once


#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/macros.h"

#include "cmath"                                // for pow, sqrt
#include "corecrt_math.h"                       // for pow, sqrt


#pragma warning( disable: 26451 )
#pragma warning( disable: 4003 )	//No enough arguments for macro
//type: the type of the function and the vectors
//typen: the type but with the capital letter
//pass: & to bass by reference vectors larger than 3 byte for better performances
#define __lp_vec_def(type, typen, pass)																																			\
typedef _vec2<type> v2##typen, vec2##typen;																																					\
typedef _vec3<type> v3##typen, vec3##typen;																																					\
typedef _vec4<type> v4##typen, vec4##typen;																																					\
static inline type __vectorcall dist##typen(const _vec2<type>##pass a, const _vec2<type>##pass b) { return scast<type>(sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2))); }						\
static inline type __vectorcall dist##typen(const _vec3<type>##pass a, const _vec3<type>##pass b) { return scast<type>(sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2) + pow(b.z - a.z, 2))); }	\
static inline type __vectorcall dist##typen(const _vec4<type>& a, const _vec4<type>& b) { return scast<type>(sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2) + pow(b.z - a.z, 2) + pow(b.w - a.w, 2))); }

__lp_vec_def(int8, i8)			__lp_vec_def(uint8, u8)			__lp_vec_def(bool, b)				__lp_vec_def(char, c)
__lp_vec_def(int16, i16, &)		__lp_vec_def(uint16, u16, &)
__lp_vec_def(int32, i32, &)		__lp_vec_def(uint32, u32, &)	__lp_vec_def(float32, f32, &)
__lp_vec_def(int64, i64, &)		__lp_vec_def(uint64, u64, &)	__lp_vec_def(float64, f32, &)
#undef __lp_vec_def
#pragma warning( default: 26451 )
#pragma warning( default: 4003 )	//No enough arguments for macro
