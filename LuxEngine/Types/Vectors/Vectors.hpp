#pragma once


#include "LuxEngine/Types/Vectors/Vec2.hpp"
#include "LuxEngine/Types/Vectors/Vec3.hpp"
#include "LuxEngine/Types/Vectors/Vec4.hpp"
#include "LuxEngine/macros.hpp"
#include "LuxEngine/Types/Integers/Integers.hpp"




#define __lp_vec_def(type, typen)				\
typedef vec2_t<type> typen##vec2, vec2##typen;	\
typedef vec3_t<type> typen##vec3, vec3##typen;	\
typedef vec4_t<type> typen##vec4, vec4##typen;	\

__lp_vec_def(int8, i8)			__lp_vec_def(uint8, u8)		__lp_vec_def(bool, b)			__lp_vec_def(wchar8, w8)	__lp_vec_def(char8, c8)
__lp_vec_def(int16, i16)		__lp_vec_def(uint16, u16)									__lp_vec_def(wchar16, w16)
__lp_vec_def(int32, i32)		__lp_vec_def(uint32, u32)	__lp_vec_def(float32, f32)		__lp_vec_def(wchar32, w32)
__lp_vec_def(int64, i64)		__lp_vec_def(uint64, u64)	__lp_vec_def(float64, f64)
#undef __lp_vec_def




//TODO use intrinsic functions whenever possible
//TODO explicit custom type conversion
template<class t> static inline constexpr auto dist(const vec2_t<t>& a, const vec2_t<t>& b) { return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2)); }										  //Returns the Euclidean distance between 2 vectors
template<class t> static inline constexpr auto dist(const vec3_t<t>& a, const vec3_t<t>& b) { return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2) + pow(b.z - a.z, 2)); }					  //Returns the Euclidean distance between 2 vectors
template<class t> static inline constexpr auto dist(const vec4_t<t>& a, const vec4_t<t>& b) { return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2) + pow(b.z - a.z, 2) + pow(b.w - a.w, 2)); } //Returns the Euclidean distance between 2 vectors

template<class t> static inline constexpr vec2_t<t>  signvNz(const vec2_t<t>& v) { return { signNz(v.x), signNz(v.y) }; }							//Returns a vector containing the sign of the elements of v (-1 if the element is negative, +1 if it's positive or 0)
template<class t> static inline constexpr vec3_t<t>  signvNz(const vec3_t<t>& v) { return { signNz(v.x), signNz(v.y), signNz(v.z) }; }				//Returns a vector containing the sign of the elements of v (-1 if the element is negative, +1 if it's positive or 0)
template<class t> static inline constexpr vec4_t<t>  signvNz(const vec4_t<t>& v) { return { signNz(v.x), signNz(v.y), signNz(v.z), signNz(v.w) }; }	//Returns a vector containing the sign of the elements of v (-1 if the element is negative, +1 if it's positive or 0)

template<class t> static inline constexpr vec2_t<t>  signv(const vec2_t<t>& v) { return { sign(v.x), sign(v.y) }; }									//Returns a vector containing the sign of the elements of v (-1 if the element is negative, +1 if it's positive, 0 if it's 0)
template<class t> static inline constexpr vec3_t<t>  signv(const vec3_t<t>& v) { return { sign(v.x), sign(v.y), sign(v.z) }; }						//Returns a vector containing the sign of the elements of v (-1 if the element is negative, +1 if it's positive, 0 if it's 0)
template<class t> static inline constexpr vec4_t<t>  signv(const vec4_t<t>& v) { return { sign(v.x), sign(v.y), sign(v.z), sign(v.w) }; }			//Returns a vector containing the sign of the elements of v (-1 if the element is negative, +1 if it's positive, 0 if it's 0)

template<class t> static inline constexpr vec2_t<t>  absv(const vec2_t<t>& v) { return { abs(v.x), abs(v.y) }; }									//Returns a vector containing the absolute value of the elements of v
template<class t> static inline constexpr vec3_t<t>  absv(const vec3_t<t>& v) { return { abs(v.x), abs(v.y), abs(v.z) }; }							//Returns a vector containing the absolute value of the elements of v
template<class t> static inline constexpr vec4_t<t>  absv(const vec4_t<t>& v) { return { abs(v.x), abs(v.y), abs(v.z), abs(v.w) }; }				//Returns a vector containing the absolute value of the elements of v

template<class t> static inline constexpr vec2_t<t>  dist2D(const vec2_t<t>& a, const vec2_t<t>& b) { return absv(a - b); }							//Returns a vector containing the distances between the values of v
template<class t> static inline constexpr vec3_t<t>  dist3D(const vec3_t<t>& a, const vec3_t<t>& b) { return absv(a - b); }							//Returns a vector containing the distances between the values of v
template<class t> static inline constexpr vec4_t<t>  dist4D(const vec4_t<t>& a, const vec4_t<t>& b) { return absv(a - b); }							//Returns a vector containing the distances between the values of v

template<class ta, class tb> static inline constexpr auto dot(const vec2_t<ta>& a, const vec2_t<tb>& b) { return (a.x * b.x) + (a.y * b.y); }									//Returns the dot product of 2 vectors
template<class ta, class tb> static inline constexpr auto dot(const vec3_t<ta>& a, const vec3_t<tb>& b) { return (a.x * b.x) + (a.y * b.y) + (a.z * b.z); }						//Returns the dot product of 2 vectors
template<class ta, class tb> static inline constexpr auto dot(const vec4_t<ta>& a, const vec4_t<tb>& b) { return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w); }		//Returns the dot product of 2 vectors
