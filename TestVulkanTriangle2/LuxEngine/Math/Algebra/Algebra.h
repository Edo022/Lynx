
#pragma once
#include <math.h>
#include "LuxEngine/Types/Integers/Integers.h"



template<class T> constexpr auto signNz(T n) { return ((n) < 0) ? -1 : 1; }		//Returns -1 if the number is negative or +1 if it's positive
template<class T> constexpr auto __lp_sign(T n) { return n / n; }					//Returns -1 if the number is negative, +1 if it's positive or 0 if it's 0
template<class T> constexpr auto __lp_Abs(T n) { return n < 0 ? -n : n; }		//  --  used as "abs" macro to prevent conflicts  --
#define sign __lp_sign
#define abs __lp_Abs															//Returns the absolute value of a number
