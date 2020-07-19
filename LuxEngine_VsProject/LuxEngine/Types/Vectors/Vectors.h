#pragma once


#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/macros.h"
#include "LuxEngine/Math/Algebra/Algebra.h"
#include "math.h"                                // for pow, sqrt




#pragma warning( disable: 26451 )
#pragma warning( disable: 4003 )	//No enough arguments for macro
//type: the type of the function and the vectors
//typen: the type but with the capital letter
#define __lp_vec_def(type, typen)				\
typedef vec2_t<type> v2##typen, vec2##typen;	\
typedef vec3_t<type> v3##typen, vec3##typen;	\
typedef vec4_t<type> v4##typen, vec4##typen;	\

__lp_vec_def(int8, i8)			__lp_vec_def(uint8, u8)		__lp_vec_def(bool, b)			__lp_vec_def(char8, c8)
__lp_vec_def(int16, i16)		__lp_vec_def(uint16, u16)									__lp_vec_def(char16, c16)
__lp_vec_def(int32, i32)		__lp_vec_def(uint32, u32)	__lp_vec_def(float32, f32)		__lp_vec_def(char32, c32)
__lp_vec_def(int64, i64)		__lp_vec_def(uint64, u64)	__lp_vec_def(float64, f64)
#undef __lp_vec_def
#pragma warning( default: 26451 )
#pragma warning( default: 4003 )	//No enough arguments for macro




//TODO use intrinsic functions whenever possible
//TODO explicit custom type convertion
template<class t> static inline constexpr auto	__vectorcall dist(const vec2_t<t>& a, const vec2_t<t>& b) { return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2)); }
template<class t> static inline constexpr auto	__vectorcall dist(const vec3_t<t>& a, const vec3_t<t>& b) { return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2) + pow(b.z - a.z, 2)); }
template<class t> static inline constexpr auto	__vectorcall dist(const vec4_t<t>& a, const vec4_t<t>& b) { return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2) + pow(b.z - a.z, 2) + pow(b.w - a.w, 2)); }

template<class t> static inline constexpr vec2_t<t> __vectorcall signNz(const vec2_t<t>& v) { return { signNz(v.x), signNz(v.y) }; }
template<class t> static inline constexpr vec3_t<t> __vectorcall signNz(const vec3_t<t>& v) { return { signNz(v.x), signNz(v.y), signNz(v.z) }; }
template<class t> static inline constexpr vec4_t<t> __vectorcall signNz(const vec4_t<t>& v) { return { signNz(v.x), signNz(v.y), signNz(v.z), signNz(v.w) }; }

template<class t> static inline constexpr vec2_t<t> __vectorcall __lp_sign(const vec2_t<t>& v) { return { sign(v.x), sign(v.y) }; }
template<class t> static inline constexpr vec3_t<t> __vectorcall __lp_sign(const vec3_t<t>& v) { return { sign(v.x), sign(v.y), sign(v.z) }; }
template<class t> static inline constexpr vec4_t<t> __vectorcall __lp_sign(const vec4_t<t>& v) { return { sign(v.x), sign(v.y), sign(v.z), sign(v.w) }; }

template<class t> static inline constexpr vec2_t<t> __vectorcall __lp_abs(const vec2_t<t>& v) { return { abs(v.x), abs(v.y) }; }
template<class t> static inline constexpr vec3_t<t> __vectorcall __lp_abs(const vec3_t<t>& v) { return { abs(v.x), abs(v.y), abs(v.z) }; }
template<class t> static inline constexpr vec4_t<t> __vectorcall __lp_abs(const vec4_t<t>& v) { return { abs(v.x), abs(v.y), abs(v.z), abs(v.w) }; }

template<class t> static inline constexpr vec2_t<t> __vectorcall dist2D(const vec2_t<t>& a, const vec2_t<t>& b) { return abs(a - b); }
template<class t> static inline constexpr vec3_t<t> __vectorcall dist3D(const vec3_t<t>& a, const vec3_t<t>& b) { return abs(a - b); }
template<class t> static inline constexpr vec4_t<t> __vectorcall dist4D(const vec4_t<t>& a, const vec4_t<t>& b) { return abs(a - b); }

template<class t> static inline constexpr auto	__vectorcall length(const vec2_t<t>& v) { return sqrt(v.x * v.x + v.y * v.y); }
template<class t> static inline constexpr auto	__vectorcall length(const vec3_t<t>& v) { return sqrt(v.x * v.x + v.y * v.y + v.z * v.z); }
template<class t> static inline constexpr auto	__vectorcall length(const vec4_t<t>& v) { return sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w); }
