
#pragma once
#include <math.h>
#include "LuxEngine/Types/Integers/Integers.hpp"
// #pragma warning ( disable:4146 ) //Unary minus operator applied to unsigned type
// #pragma warning ( disable:4804 ) //Unsafe use of type bool
// #pragma warning ( disable:4244 ) //Conversion from 'int' to 'T', possible loss of data

//TODO use intrinsic functions
template<class T> static inline constexpr T signNz(const T n) { return (n < 0) ? -1 : 1; }		//Returns -1 if the number is negative or +1 if it's positive
template<class T> static inline constexpr T __lp_sign(const T n) { return n / n; }				//  --  used as "sign" macro to prevent conflicts  --
template<class T> static inline constexpr T __lp_abs(const T n) { return (n < 0) ? -n : n; }	//  --  used as "abs" macro to prevent conflicts  --
#define sign(n) __lp_sign(n)																	//Returns -1 if the number is negative, +1 if it's positive or 0 if it's 0
// #define abs(n) __lp_abs(n)																	//Returns the absolute value of a number
#define lux_abs(n) __lp_abs(n)																	//Returns the absolute value of a number



//Returns the first multiple of m larger than or equal to n
//   e.g. multipleOf(21, 6) returns 24
//   e.g. multipleOf(15, 3) returns 15
template<class tn, class tm> static inline constexpr auto multipleOf(const tn n, const tm m) {
	return (n % m) ? (n - (n % m) + m) : n;
}
//Returns the largest multiple of m smaller than or equal to n
//   e.g. multipleOf(21, 6) returns 18
//   e.g. multipleOf(15, 3) returns 15
template<class tn, class tm>static  inline constexpr auto mmultipleOf(const tn n, const tm m) {
	return (n - (n % m));
}






//f | first value of the interval
//s | second value of the interval
//o | offset alignment
//e.g. alignedIntervalCut(10, 25, 4, ...)
//    a = 2  (from 10 to 12)
//    b = 12 (from 13 to 24)
//    c = 1  (25)
template<class t>
static inline constexpr auto alignedIntervalCut(const t f, const t s, const t o, t* const a, t* const b, t* const c){
	const auto fi = multipleOf(f, o);
	const auto si = mmultipleOf(s, o);
	*a = fi - f;
	//*b = mmultipleOf(s, o) - fi;
	*b = si - fi;
	//*c = s%o;
	*c = s - si;
}


//Returns the maximum number of variables with count s that fits in b bytes, with a minimum of 1 variable
//   e.g. fit(sizeof(int32), 15) returns 3
//   e.g. fit(sizeof(int32), 3) returns 1
static inline constexpr uint64 fit(const uint64 s, const uint64 b) {  return multipleOf(b, s) / s; }


// #pragma warning ( default:4146 ) //Unary minus operator applied to unsigned type
// #pragma warning ( default:4804 ) //Unsafe use of type bool
// #pragma warning ( default:4244 ) //Conversion from 'int' to 'T', possible loss of data