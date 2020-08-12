
#pragma once
#include <math.h>
#include "LuxEngine/Types/Integers/Integers.h"
#pragma warning ( disable:4146 ) //Unary minus operator applied to unsigned type
#pragma warning ( disable:4804 ) //Unsafe use of type bool
#pragma warning ( disable:4244 ) //Conversion from 'int' to 'T', possible loss of data

//TODO use intrinsic functions
template<class T> inline constexpr T signNz(const T n) { return (n < 0) ? -1 : 1; }		//Returns -1 if the number is negative or +1 if it's positive
template<class T> inline constexpr T __lp_sign(const T n) { return n / n; }				//  --  used as "sign" macro to prevent conflicts  --
template<class T> inline constexpr T __lp_abs(const T n) { return (n < 0) ? -n : n; }	//  --  used as "abs" macro to prevent conflicts  --
#define sign __lp_sign																	//Returns -1 if the number is negative, +1 if it's positive or 0 if it's 0
#define abs __lp_abs																	//Returns the absolute value of a number



//Returns the first multiple of m larger than or equal to n
//   e.g. multipleOf(21, 6) returns 24
//   e.g. multipleOf(15, 3) returns 15
template<class tn, class tm> inline constexpr auto multipleOf(const tn n, const tm m) { 
	return (n % m == 0) ? n : (n - (n % m) + m); 
}



//Returns the maximum number of variables with size s that fits in b bytes, with a minimum of 1 variable
//   e.g. fit(sizeof(int32), 15) returns 3
//   e.g. fit(sizeof(int32), 3) returns 1
inline constexpr uint64 fit(const uint64 s, const uint64 b) {  return multipleOf(b, s) / s; }


#pragma warning ( default:4146 ) //Unary minus operator applied to unsigned type
#pragma warning ( default:4804 ) //Unsafe use of type bool
#pragma warning ( default:4244 ) //Conversion from 'int' to 'T', possible loss of data