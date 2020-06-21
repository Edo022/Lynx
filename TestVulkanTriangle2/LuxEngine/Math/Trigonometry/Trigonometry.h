
#pragma once
#include "LuxEngine/Math/Constants.h"


template<class t>
static inline t __vectorcall revToRad(t n) { return n * PI2; }
template<class t>
static inline t __vectorcall radToRev(t n) { return n / PI2; }

