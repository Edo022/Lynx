

#include "GoniometricFunctions.h"

#ifdef LUX_CNF_USE_FFUNCTIONS
#define __lp_mallocFunc(n) (double*)malloc(sizeof(double) * (n))
double* __lp_sin = __lp_mallocFunc(LUX_CNF_FFUNCTIONS_PRECISION);
double* __lp_cos = __lp_mallocFunc(LUX_CNF_FFUNCTIONS_PRECISION);
double* __lp_tan = __lp_mallocFunc(LUX_CNF_FFUNCTIONS_PRECISION);
double* __lp_cot = __lp_mallocFunc(LUX_CNF_FFUNCTIONS_PRECISION);
double* __lp_sec = __lp_mallocFunc(LUX_CNF_FFUNCTIONS_PRECISION);
double* __lp_csc = __lp_mallocFunc(LUX_CNF_FFUNCTIONS_PRECISION);

double* __lp_asin = __lp_mallocFunc(LUX_CNF_FFUNCTIONS_PRECISION);
double* __lp_acos = __lp_mallocFunc(LUX_CNF_FFUNCTIONS_PRECISION);
double* __lp_atan = __lp_mallocFunc(LUX_CNF_FFUNCTIONS_PRECISION);
#undef __lp_mallocFunc
#endif
