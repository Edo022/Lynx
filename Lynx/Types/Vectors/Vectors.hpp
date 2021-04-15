#pragma once
#define LNX_H_VECTORS
#include "Lynx/Types/Integers/Integers.hpp"
#include "Lynx/Types/Vectors/Vec2.hpp"
#include "Lynx/Types/Vectors/Vec3.hpp"
#include "Lynx/Types/Vectors/Vec4.hpp"
#include <avx2intrin.h>
#include <type_traits>
#include <cmath>








namespace lnxc{
    typedef vec2_t<i8> i8v2, int8v2;        //  Two-dimentional int8 vector
    typedef vec3_t<i8> i8v3, int8v3;        //Three-dimentional int8 vector
    typedef vec4_t<i8> i8v4, int8v4;        // Four-dimentional int8 vector

    typedef vec2_t<i16> i16v2, int16v2;     //  Two-dimentional int16 vector
    typedef vec3_t<i16> i16v3, int16v3;     //Three-dimentional int16 vector
    typedef vec4_t<i16> i16v4, int16v4;     // Four-dimentional int16 vector

    typedef vec2_t<i32> i32v2, int32v2;     //  Two-dimentional int32 vector
    typedef vec3_t<i32> i32v3, int32v3;     //Three-dimentional int32 vector
    typedef vec4_t<i32> i32v4, int32v4;     // Four-dimentional int32 vector

    typedef vec2_t<i64> i64v2, int64v2;     //  Two-dimentional int64 vector
    typedef vec3_t<i64> i64v3, int64v3;     //Three-dimentional int64 vector
    typedef vec4_t<i64> i64v4, int64v4;     // Four-dimentional int64 vector




    typedef vec2_t<u8> u8v2, uint8v2;       //  Two-dimentional uint8 vector
    typedef vec3_t<u8> u8v3, uint8v3;       //Three-dimentional uint8 vector
    typedef vec4_t<u8> u8v4, uint8v4;       // Four-dimentional uint8 vector

    typedef vec2_t<u16> u16v2, uint16v2;    //  Two-dimentional uint16 vector
    typedef vec3_t<u16> u16v3, uint16v3;    //Three-dimentional uint16 vector
    typedef vec4_t<u16> u16v4, uint16v4;    // Four-dimentional uint16 vector

    typedef vec2_t<u32> u32v2, uint32v2;    //  Two-dimentional uint32 vector
    typedef vec3_t<u32> u32v3, uint32v3;    //Three-dimentional uint32 vector
    typedef vec4_t<u32> u32v4, uint32v4;    // Four-dimentional uint32 vector

    typedef vec2_t<u64> u64v2, uint64v2;    //  Two-dimentional uint64 vector
    typedef vec3_t<u64> u64v3, uint64v3;    //Three-dimentional uint64 vector
    typedef vec4_t<u64> u64v4, uint64v4;    // Four-dimentional uint64 vector




    typedef vec2_t<bool> bv2, boolv2;        //  Two-dimentional bool vector
    typedef vec3_t<bool> bv3, boolv3;        //Three-dimentional bool vector
    typedef vec4_t<bool> bv4, boolv4;        // Four-dimentional bool vector

    typedef vec2_t<f32> f32v2, float32v2;   //  Two-dimentional float32 vector
    typedef vec3_t<f32> f32v3, float32v3;   //Three-dimentional float32 vector
    typedef vec4_t<f32> f32v4, float32v4;   // Four-dimentional float32 vector

    typedef vec2_t<f64> f64v2, float64v2;   //  Two-dimentional float64 vector
    typedef vec3_t<f64> f64v3, float64v3;   //Three-dimentional float64 vector
    typedef vec4_t<f64> f64v4, float64v4;   // Four-dimentional float64 vector




    static inline constexpr auto sum(const vec2_t<auto>& v) noexcept { return v.x + v.y; }               //Returns the sum between the elements of the vector (x + y)
    static inline constexpr auto sum(const vec3_t<auto>& v) noexcept { return v.x + v.y + v.z; }         //Returns the sum between the elements of the vector (x + y + z)
    static inline constexpr auto sum(const vec4_t<auto>& v) noexcept { return v.x + v.y + v.z + v.w; }   //Returns the sum between the elements of the vector (x + y + z + w)

    static inline constexpr auto sub(const vec2_t<auto>& v) noexcept { return v.x - v.y; }               //Returns the difference between the elements of the vector (x - y)
    static inline constexpr auto sub(const vec3_t<auto>& v) noexcept { return v.x - v.y - v.z; }         //Returns the difference between the elements of the vector (x - y - z)
    static inline constexpr auto sub(const vec4_t<auto>& v) noexcept { return v.x - v.y - v.z - v.w; }   //Returns the difference between the elements of the vector (x - y - z - w)




    #define vec_fun(fun)                                                                                                                \
        static inline constexpr vec2_t<auto> fun(const vec2_t<auto>& v) noexcept { return { ::fun(v.x), ::fun(v.y) }; }                 \
        static inline constexpr vec3_t<auto> fun(const vec3_t<auto>& v) noexcept { return { ::fun(v.x), ::fun(v.y), ::fun(v.z) }; }     \
        static inline constexpr vec4_t<auto> fun(const vec4_t<auto>& v) noexcept { return { ::fun(v.x), ::fun(v.y), ::fun(v.z), ::fun(v.w) }; }

        vec_fun(abs)    vec_fun(sqrt)   vec_fun(sign)
        vec_fun(sin)    vec_fun(cos)    vec_fun(tan)    vec_fun(sec)    vec_fun(cosec)
        vec_fun(asin)   vec_fun(acos)   vec_fun(atan)   vec_fun(asec)   vec_fun(acosec)
        vec_fun(sinh)   vec_fun(cosh)   vec_fun(tanh)   vec_fun(sech)   vec_fun(cosech)
        vec_fun(asinh)  vec_fun(acosh)  vec_fun(atanh)  vec_fun(asech)  vec_fun(acosech)

        static alwaysInline constexpr auto csc  (const auto& v) noexcept { return   cosec(v); };     //cosec   alias
        static alwaysInline constexpr auto acsc (const auto& v) noexcept { return  acosec(v); };     //acosec  alias
        static alwaysInline constexpr auto csch (const auto& v) noexcept { return  cosech(v); };     //cosech  alias
        static alwaysInline constexpr auto acsch(const auto& v) noexcept { return acosech(v); };     //acosech alias
    #undef vec_fun




    static inline constexpr auto dist (const std::derived_from<vec_b> auto& a, const std::derived_from<vec_b> auto& b) noexcept { return sqrt(sum(pow(a - b, 2))); }    //Returns the Euclidean distance between a and b
    static inline constexpr auto sdist(const std::derived_from<vec_b> auto& a, const std::derived_from<vec_b> auto& b) noexcept { return      sum(pow(a - b, 2));  }    //Returns the squared Euclidean distance between a and b
    static inline constexpr auto adist(const std::derived_from<vec_b> auto& a, const std::derived_from<vec_b> auto& b) noexcept { return abs(a - b); }	                //Returns a vector containing the distances between the values of a and b
    static inline constexpr auto dot  (const std::derived_from<vec_b> auto& a, const std::derived_from<vec_b> auto& b) noexcept { return sum(a * b); }                  //Returns the dot product between a and b
}
#ifndef LNX_NO_GLOBAL_NAMESPACE
	using namespace lnxc;
#endif