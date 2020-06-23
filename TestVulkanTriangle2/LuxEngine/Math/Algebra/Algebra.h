
#pragma once
#include <math.h>
#include "LuxEngine/Types/Integers/Integers.h"



template<class T> constexpr auto sign(T n) { return ((n) < 0) ? -1 : 1; }		//-1 if the number is negative, +1 if it's positive or 0; }

//Integer abs functions because math.h only has the floating point ones
//constexpr auto abs(int64 n) { return ((n) < 0) ? -n : n; }
//constexpr auto abs(int32 n) { return ((n) < 0) ? -n : n; }
//constexpr auto abs(int16 n) { return ((n) < 0) ? -n : n; }
//constexpr auto abs(int8 n) { return ((n) < 0) ? -n : n; }
//constexpr auto abs(uint64 n) { return ((n) < 0) ? -n : n; }
//constexpr auto abs(uint32 n) { return ((n) < 0) ? -n : n; }
//constexpr auto abs(uint16 n) { return ((n) < 0) ? -n : n; }
//constexpr auto abs(uint8 n) { return ((n) < 0) ? -n : n; }