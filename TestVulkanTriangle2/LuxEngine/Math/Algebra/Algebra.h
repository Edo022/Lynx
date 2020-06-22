
#pragma once
#include <math.h>
#include "LuxEngine/Types/Integers/Integers.h"


template<class T>
constexpr auto sign(T n) { return (((n) < 0) ? -1 : 1); }		//-1 if the number is negative, +1 if it's positive or 0; }


//constexpr auto abs(uint64 n) { return ((n) < 0) ? -(n) : (n); }