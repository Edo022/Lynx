#pragma once
#define LUX_H_VECTORS
#include "LuxEngine/Types/Integers/Integers.hpp"
#include "LuxEngine/Types/Vectors/Vec2.hpp"
#include "LuxEngine/Types/Vectors/Vec3.hpp"
#include "LuxEngine/Types/Vectors/Vec4.hpp"
#include <avx2intrin.h>
#include <type_traits>
#include <cmath>




using i8v2   = vec2_t<i8>;      //  Two-dimentional int8 vector
using i8v3   = vec3_t<i8>;      //Three-dimentional int8 vector
using i8v4   = vec4_t<i8>;      // Four-dimentional int8 vector
using int8v2 = vec2_t<i8>;      //  Two-dimentional int8 vector
using int8v3 = vec3_t<i8>;      //Three-dimentional int8 vector
using int8v4 = vec4_t<i8>;      // Four-dimentional int8 vector

using i16v2   = vec2_t<i16>;    //  Two-dimentional int16 vector
using i16v3   = vec3_t<i16>;    //Three-dimentional int16 vector
using i16v4   = vec4_t<i16>;    // Four-dimentional int16 vector
using int16v2 = vec2_t<i16>;    //  Two-dimentional int16 vector
using int16v3 = vec3_t<i16>;    //Three-dimentional int16 vector
using int16v4 = vec4_t<i16>;    // Four-dimentional int16 vector

using i32v2   = vec2_t<i32>;    //  Two-dimentional int32 vector
using i32v3   = vec3_t<i32>;    //Three-dimentional int32 vector
using i32v4   = vec4_t<i32>;    // Four-dimentional int32 vector
using int32v2 = vec2_t<i32>;    //  Two-dimentional int32 vector
using int32v3 = vec3_t<i32>;    //Three-dimentional int32 vector
using int32v4 = vec4_t<i32>;    // Four-dimentional int32 vector

using i64v2   = vec2_t<i64>;    //  Two-dimentional int64 vector
using i64v3   = vec3_t<i64>;    //Three-dimentional int64 vector
using i64v4   = vec4_t<i64>;    // Four-dimentional int64 vector
using int64v2 = vec2_t<i64>;    //  Two-dimentional int64 vector
using int64v3 = vec3_t<i64>;    //Three-dimentional int64 vector
using int64v4 = vec4_t<i64>;    // Four-dimentional int64 vector




using u8v2    = vec2_t<u8>;      //  Two-dimentional uint8 vector
using u8v3    = vec3_t<u8>;      //Three-dimentional uint8 vector
using u8v4    = vec4_t<u8>;      // Four-dimentional uint8 vector
using uint8v2 = vec2_t<u8>;      //  Two-dimentional uint8 vector
using uint8v3 = vec3_t<u8>;      //Three-dimentional uint8 vector
using uint8v4 = vec4_t<u8>;      // Four-dimentional uint8 vector

using u16v2    = vec2_t<u16>;    //  Two-dimentional uint16 vector
using u16v3    = vec3_t<u16>;    //Three-dimentional uint16 vector
using u16v4    = vec4_t<u16>;    // Four-dimentional uint16 vector
using uint16v2 = vec2_t<u16>;    //  Two-dimentional uint16 vector
using uint16v3 = vec3_t<u16>;    //Three-dimentional uint16 vector
using uint16v4 = vec4_t<u16>;    // Four-dimentional uint16 vector

using u32v2    = vec2_t<u32>;    //  Two-dimentional uint32 vector
using u32v3    = vec3_t<u32>;    //Three-dimentional uint32 vector
using u32v4    = vec4_t<u32>;    // Four-dimentional uint32 vector
using uint32v2 = vec2_t<u32>;    //  Two-dimentional uint32 vector
using uint32v3 = vec3_t<u32>;    //Three-dimentional uint32 vector
using uint32v4 = vec4_t<u32>;    // Four-dimentional uint32 vector

using u64v2    = vec2_t<u64>;    //  Two-dimentional uint64 vector
using u64v3    = vec3_t<u64>;    //Three-dimentional uint64 vector
using u64v4    = vec4_t<u64>;    // Four-dimentional uint64 vector
using uint64v2 = vec2_t<u64>;    //  Two-dimentional uint64 vector
using uint64v3 = vec3_t<u64>;    //Three-dimentional uint64 vector
using uint64v4 = vec4_t<u64>;    // Four-dimentional uint64 vector




using f32v2     = vec2_t<f32>;  //  Two-dimentional float32 vector
using f32v3     = vec3_t<f32>;  //Three-dimentional float32 vector
using f32v4     = vec4_t<f32>;  // Four-dimentional float32 vector
using float32v2 = vec2_t<f32>;  //  Two-dimentional float32 vector
using float32v3 = vec3_t<f32>;  //Three-dimentional float32 vector
using float32v4 = vec4_t<f32>;  // Four-dimentional float32 vector

using f64v2     = vec2_t<f64>;  //  Two-dimentional float64 vector
using f64v3     = vec3_t<f64>;  //Three-dimentional float64 vector
using f64v4     = vec4_t<f64>;  // Four-dimentional float64 vector
using float64v2 = vec2_t<f64>;  //  Two-dimentional float64 vector
using float64v3 = vec3_t<f64>;  //Three-dimentional float64 vector
using float64v4 = vec4_t<f64>;  // Four-dimentional float64 vector




static inline constexpr auto sum(const vec2_t<auto>& v) noexcept { return v.x + v.y; }               //Returns the sum between the elements of the vector (x + y)
static inline constexpr auto sum(const vec3_t<auto>& v) noexcept { return v.x + v.y + v.z; }         //Returns the sum between the elements of the vector (x + y + z)
static inline constexpr auto sum(const vec4_t<auto>& v) noexcept { return v.x + v.y + v.z + v.w; }   //Returns the sum between the elements of the vector (x + y + z + w)

static inline constexpr auto sub(const vec2_t<auto>& v) noexcept { return v.x - v.y; }               //Returns the difference between the elements of the vector (x - y)
static inline constexpr auto sub(const vec3_t<auto>& v) noexcept { return v.x - v.y - v.z; }         //Returns the difference between the elements of the vector (x - y - z)
static inline constexpr auto sub(const vec4_t<auto>& v) noexcept { return v.x - v.y - v.z - v.w; }   //Returns the difference between the elements of the vector (x - y - z - w)

static inline constexpr vec2_t<auto> abs(const vec2_t<auto>& v) noexcept { return { abs(v.x), abs(v.y) }; }						    //Returns a vector containing the absolute values of the elements of v
static inline constexpr vec3_t<auto> abs(const vec3_t<auto>& v) noexcept { return { abs(v.x), abs(v.y), abs(v.z) }; }			    //Returns a vector containing the absolute values of the elements of v
static inline constexpr vec4_t<auto> abs(const vec4_t<auto>& v) noexcept { return { abs(v.x), abs(v.y), abs(v.z), abs(v.w) }; }	    //Returns a vector containing the absolute values of the elements of v

static inline constexpr vec2_t<auto> sqrt(const vec2_t<auto>& v) noexcept { return { sqrt(v.x), sqrt(v.y) }; }                       //Returns a vector containing the square roots of the elements of v
static inline constexpr vec3_t<auto> sqrt(const vec3_t<auto>& v) noexcept { return { sqrt(v.x), sqrt(v.y), sqrt(v.z) }; }            //Returns a vector containing the square roots of the elements of v
static inline constexpr vec4_t<auto> sqrt(const vec4_t<auto>& v) noexcept { return { sqrt(v.x), sqrt(v.y), sqrt(v.z), sqrt(v.w) }; } //Returns a vector containing the square roots of the elements of v

static inline constexpr i32v2 sign(const vec2_t<auto>& v) noexcept { return { sign(v.x), sign(v.y) }; }							//Returns a vector containing the sign of the elements of v (-1 if the element is negative, +1 if it's positive or 0)
static inline constexpr i32v3 sign(const vec3_t<auto>& v) noexcept { return { sign(v.x), sign(v.y), sign(v.z) }; }				//Returns a vector containing the sign of the elements of v (-1 if the element is negative, +1 if it's positive or 0)
static inline constexpr i32v4 sign(const vec4_t<auto>& v) noexcept { return { sign(v.x), sign(v.y), sign(v.z), sign(v.w) }; }	//Returns a vector containing the sign of the elements of v (-1 if the element is negative, +1 if it's positive or 0)

// static inline constexpr auto dist(const vec2_t<auto>& a, const vec2_t<auto>& b) { return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2)); }										  //Returns the Euclidean distance between 2 vectors
// static inline constexpr auto dist(const vec3_t<auto>& a, const vec3_t<auto>& b) { return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2) + pow(b.z - a.z, 2)); }					  //Returns the Euclidean distance between 2 vectors
// static inline constexpr auto dist(const vec4_t<auto>& a, const vec4_t<auto>& b) { return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2) + pow(b.z - a.z, 2) + pow(b.w - a.w, 2)); } //Returns the Euclidean distance between 2 vectors

template<class ta, class tb> static inline constexpr auto dist  (const ta& a, const tb& b) noexcept requires(std::is_base_of_v<vec_b, ta> && std::is_base_of_v<vec_b, tb>) { return sqrt(sum(pow(a - b, 2))); }  //Returns the Euclidean distance between a and b
template<class ta, class tb> static inline constexpr auto dist2D(const ta& a, const tb& b) noexcept requires(std::is_base_of_v<vec_b, ta> && std::is_base_of_v<vec_b, tb>) { return abs(a - b); }	            //Returns a vector containing the distances between the values of a and b
template<class ta, class tb> static inline constexpr auto dot   (const ta& a, const tb& b) noexcept requires(std::is_base_of_v<vec_b, ta> && std::is_base_of_v<vec_b, tb>) { return sum(a * b); }                //Returns the dot product between a and b