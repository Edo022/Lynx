
#pragma once
#include "LuxEngine/LuxEngine_config.hpp"
#include "LuxEngine/Math/Algebra/Algebra.hpp"
#include "LuxEngine/Math/Trigonometry/Trigonometry.hpp"
#include "LuxEngine/Types/Integers/Integers.hpp"
#include "LuxEngine/macros.hpp"
// #include "corecrt_mathcorecrt_math.h"                              // for cos, sin, tan, acos, asin, atan
#include <math.h>

#ifdef LUX_CNF_USE_FFUNCTIONS
extern double* __lp_sin;
extern double* __lp_cos;
extern double* __lp_tan;
extern double* __lp_cot;
extern double* __lp_sec;
extern double* __lp_csc;

extern double* __lp_asin;
extern double* __lp_acos;
extern double* __lp_atan;
#endif






static void __lp_goniometric_functions_init( ) {
	#ifdef LUX_CNF_USE_FFUNCTIONS

	#define __lp_mallocFunc(n) (double*)malloc(sizeof(double) * (n))
	__lp_sin = __lp_mallocFunc(LUX_CNF_FFUNCTIONS_PRECISION);
	__lp_cos = __lp_mallocFunc(LUX_CNF_FFUNCTIONS_PRECISION);
	__lp_tan = __lp_mallocFunc(LUX_CNF_FFUNCTIONS_PRECISION);
	__lp_cot = __lp_mallocFunc(LUX_CNF_FFUNCTIONS_PRECISION);
	__lp_sec = __lp_mallocFunc(LUX_CNF_FFUNCTIONS_PRECISION);
	__lp_csc = __lp_mallocFunc(LUX_CNF_FFUNCTIONS_PRECISION);

	__lp_asin = __lp_mallocFunc(LUX_CNF_FFUNCTIONS_PRECISION);
	__lp_acos = __lp_mallocFunc(LUX_CNF_FFUNCTIONS_PRECISION);
	__lp_atan = __lp_mallocFunc(LUX_CNF_FFUNCTIONS_PRECISION);
	#undef __lp_mallocFunc


	for(int i = 0; i < LUX_CNF_FFUNCTIONS_PRECISION; ++i) {
		static double rads = (revToRad(scast<double>(i)) / LUX_CNF_FFUNCTIONS_PRECISION);
		__lp_sin[i] = sin(rads);
		__lp_cos[i] = cos(rads);
		__lp_tan[i] = tan(rads);
		__lp_cot[i] = 1 / tan(rads);
		__lp_sec[i] = 1 / cos(rads);
		__lp_csc[i] = 1 / sin(rads);

		__lp_asin[i] = asin(rads);
		__lp_acos[i] = acos(rads);
		__lp_atan[i] = atan(rads);
		//__lp_acot[i] = 1 / atan(rads);
		//__lp_asec[i] = 1 / acos(rads);
		//__lp_acsc[i] = 1 / asin(rads);

		//*(__lp_sinh + i) = sinh(j);
		//*(__lp_cosh + i) = cosh(j);
		//*(__lp_tanh + i) = tanh(j);
		//*(__lp_coth + i) = 1 / tanh(j);
		//*(__lp_sech + i) = 1 / cosh(j);
		//*(__lp_csch + i) = 1 / sinh(j);
	}
	#endif
}





//TODO add
#ifdef LUX_CNF_USE_FFUNCTIONS
template<class T> constexpr auto fsin(T n) { return signNz(n) * __lp_sin[scast<uint32>(abs(n) * LUX_CNF_FFUNCTIONS_PRECISION) % LUX_CNF_FFUNCTIONS_PRECISION]; }	//Fast sine function that uses a precomputed values to improve performance. Precision: 1/LUX_CNF_FFUNCTIONS_PRECISION
template<class T> constexpr auto fcos(T n) { return           __lp_cos[scast<uint32>(abs(n) * LUX_CNF_FFUNCTIONS_PRECISION) % LUX_CNF_FFUNCTIONS_PRECISION]; }		//Fast cosine function that uses a precomputed values to improve performance. Precision: 1/LUX_CNF_FFUNCTIONS_PRECISION
template<class T> constexpr auto ftan(T n) { return signNz(n) * __lp_tan[scast<uint32>(abs(n) * LUX_CNF_FFUNCTIONS_PRECISION) % LUX_CNF_FFUNCTIONS_PRECISION]; }	//Fast tangent function that uses a precomputed values to improve performance. Precision: 1/LUX_CNF_FFUNCTIONS_PRECISION
template<class T> constexpr auto fcot(T n) { return signNz(n) * __lp_cot[scast<uint32>(abs(n) * LUX_CNF_FFUNCTIONS_PRECISION) % LUX_CNF_FFUNCTIONS_PRECISION]; }	//Fast cotangent function that uses a precomputed values to improve performance. Precision: 1/LUX_CNF_FFUNCTIONS_PRECISION
template<class T> constexpr auto fsec(T n) { return           __lp_sec[scast<uint32>(abs(n) * LUX_CNF_FFUNCTIONS_PRECISION) % LUX_CNF_FFUNCTIONS_PRECISION]; }		//Fast secant function that uses a precomputed values to improve performance. Precision: 1/LUX_CNF_FFUNCTIONS_PRECISION
template<class T> constexpr auto fcsc(T n) { return signNz(n) * __lp_csc[scast<uint32>(abs(n) * LUX_CNF_FFUNCTIONS_PRECISION) % LUX_CNF_FFUNCTIONS_PRECISION]; }	//Fast cosecant function that uses a precomputed values to improve performance. Precision: 1/LUX_CNF_FFUNCTIONS_PRECISION

template<class T> constexpr auto fsin1(T n) { return signNz(n) * __lp_sin[(abs(n) * LUX_CNF_FFUNCTIONS_PRECISION) % LUX_CNF_FFUNCTIONS_PRECISION]; }	//Faster version of fsin. This works only with values included in the function period. Precision: 1/LUX_CNF_FFUNCTIONS_PRECISION
template<class T> constexpr auto fcos1(T n) { return           __lp_cos[(abs(n) * LUX_CNF_FFUNCTIONS_PRECISION) % LUX_CNF_FFUNCTIONS_PRECISION]; }		//Faster version of fcos. This works only with values included in the function period. Precision: 1/LUX_CNF_FFUNCTIONS_PRECISION
template<class T> constexpr auto ftan1(T n) { return signNz(n) * __lp_tan[(abs(n) * LUX_CNF_FFUNCTIONS_PRECISION) % LUX_CNF_FFUNCTIONS_PRECISION]; }	//Faster version of ftan. This works only with values included in the function period. Precision: 1/LUX_CNF_FFUNCTIONS_PRECISION
template<class T> constexpr auto fcot1(T n) { return signNz(n) * __lp_cot[(abs(n) * LUX_CNF_FFUNCTIONS_PRECISION) % LUX_CNF_FFUNCTIONS_PRECISION]; }	//Faster version of fcot. This works only with values included in the function period. Precision: 1/LUX_CNF_FFUNCTIONS_PRECISION
template<class T> constexpr auto fsec1(T n) { return           __lp_sec[(abs(n) * LUX_CNF_FFUNCTIONS_PRECISION) % LUX_CNF_FFUNCTIONS_PRECISION]; }		//Faster version of fsec. This works only with values included in the function period. Precision: 1/LUX_CNF_FFUNCTIONS_PRECISION
template<class T> constexpr auto fcsc1(T n) { return signNz(n) * __lp_csc[(abs(n) * LUX_CNF_FFUNCTIONS_PRECISION) % LUX_CNF_FFUNCTIONS_PRECISION]; }	//Faster version of fcsc. This works only with values included in the function period. Precision: 1/LUX_CNF_FFUNCTIONS_PRECISION

template<class T> constexpr auto fsin2(T n) { return signNz(n) * __lp_sin[scast<uint32>(abs(n) * LUX_CNF_FFUNCTIONS_PRECISION)]; }		//Faster version of fsin. This works only with integral values
template<class T> constexpr auto fcos2(T n) { return           __lp_cos[scast<uint32>(abs(n) * LUX_CNF_FFUNCTIONS_PRECISION)]; }		//Faster version of fcos. This works only with integral values
template<class T> constexpr auto ftan2(T n) { return signNz(n) * __lp_tan[scast<uint32>(abs(n) * LUX_CNF_FFUNCTIONS_PRECISION)]; }		//Faster version of ftan. This works only with integral values
template<class T> constexpr auto fcot2(T n) { return signNz(n) * __lp_cot[scast<uint32>(abs(n) * LUX_CNF_FFUNCTIONS_PRECISION)]; }		//Faster version of fcot. This works only with integral values
template<class T> constexpr auto fsec2(T n) { return           __lp_sec[scast<uint32>(abs(n) * LUX_CNF_FFUNCTIONS_PRECISION)]; }		//Faster version of fsec. This works only with integral values
template<class T> constexpr auto fcsc2(T n) { return signNz(n) * __lp_csc[scast<uint32>(abs(n) * LUX_CNF_FFUNCTIONS_PRECISION)]; }		//Faster version of fcsc. This works only with integral values

template<class T> constexpr auto ffsin(T n) { return signNz(n) * __lp_sin[(abs(n) * LUX_CNF_FFUNCTIONS_PRECISION)]; }		//Fastest version of fsin. This works only with integral values included in the function period
template<class T> constexpr auto ffcos(T n) { return           __lp_cos[(abs(n) * LUX_CNF_FFUNCTIONS_PRECISION)]; }		//Fastest version of fcos. This works only with integral values included in the function period
template<class T> constexpr auto fftan(T n) { return signNz(n) * __lp_tan[(abs(n) * LUX_CNF_FFUNCTIONS_PRECISION)]; }		//Fastest version of ftan. This works only with integral values included in the function period
template<class T> constexpr auto ffcot(T n) { return signNz(n) * __lp_cot[(abs(n) * LUX_CNF_FFUNCTIONS_PRECISION)]; }		//Fastest version of fcot. This works only with integral values included in the function period
template<class T> constexpr auto ffsec(T n) { return           __lp_sec[(abs(n) * LUX_CNF_FFUNCTIONS_PRECISION)]; }		//Fastest version of fsec. This works only with integral values included in the function period
template<class T> constexpr auto ffcsc(T n) { return signNz(n) * __lp_csc[(abs(n) * LUX_CNF_FFUNCTIONS_PRECISION)]; }		//Fastest version of fcsc. This works only with integral values included in the function period
#endif