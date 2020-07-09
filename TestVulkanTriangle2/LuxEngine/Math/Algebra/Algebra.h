
#pragma once
#include <math.h>
#include "LuxEngine/Types/Integers/Integers.h"


//TODO use intrinsic functions
template<class T> constexpr T signNz(const T n) { return ((n) < 0) ? -1 : 1; }	//Returns -1 if the number is negative or +1 if it's positive
template<class T> constexpr T __lp_sign(const T n) { return n / n; }			//  --  used as "sign" macro to prevent conflicts  --
template<class T> constexpr T __lp_Abs(const T n) { return n < 0 ? -n : n; }	//  --  used as "abs" macro to prevent conflicts  --
#define sign __lp_sign															//Returns -1 if the number is negative, +1 if it's positive or 0 if it's 0
#define abs __lp_Abs															//Returns the absolute value of a number
