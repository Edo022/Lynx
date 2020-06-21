#pragma once


#define pi  3.141592653589793239
#define pi2 6.283185307179586478
#define e   0.577215664901532861

template<class t>
static inline t __vectorcall revToRad(t n) { return n * pi2; }