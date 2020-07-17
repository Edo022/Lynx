
#pragma once
#include "LuxEngine/LuxEngine_config.h"
#include "LuxEngine/Math/Algebra/Algebra.h"
#include "LuxEngine/Math/Trigonometry/Trigonometry.h"
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/macros.h"
#include "corecrt_math.h"                              // for cos, sin, tan, acos, asin, atan

#if FUNC_PRECISION != 0
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






static void __lp_goniometric_functions_init() {
	#if FUNC_PRECISION != 0
	for (int i = 0; i < FUNC_PRECISION; ++i) {
		static double rads = (revToRad(scast<double>(i)) / FUNC_PRECISION);
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
#if FUNC_PRECISION != 0
template<class T> constexpr auto __vectorcall fsin(T n) { return signNz(n) * __lp_sin[scast<uint32>(abs(n) * FUNC_PRECISION) % FUNC_PRECISION]; }	//Fast sine function that uses a precomputed values to improve performance. Precision: 1/FUNC_PRECISION
template<class T> constexpr auto __vectorcall fcos(T n) { return           __lp_cos[scast<uint32>(abs(n) * FUNC_PRECISION) % FUNC_PRECISION]; }		//Fast cosine function that uses a precomputed values to improve performance. Precision: 1/FUNC_PRECISION
template<class T> constexpr auto __vectorcall ftan(T n) { return signNz(n) * __lp_tan[scast<uint32>(abs(n) * FUNC_PRECISION) % FUNC_PRECISION]; }	//Fast tangent function that uses a precomputed values to improve performance. Precision: 1/FUNC_PRECISION
template<class T> constexpr auto __vectorcall fcot(T n) { return signNz(n) * __lp_cot[scast<uint32>(abs(n) * FUNC_PRECISION) % FUNC_PRECISION]; }	//Fast cotangent function that uses a precomputed values to improve performance. Precision: 1/FUNC_PRECISION
template<class T> constexpr auto __vectorcall fsec(T n) { return           __lp_sec[scast<uint32>(abs(n) * FUNC_PRECISION) % FUNC_PRECISION]; }		//Fast secant function that uses a precomputed values to improve performance. Precision: 1/FUNC_PRECISION
template<class T> constexpr auto __vectorcall fcsc(T n) { return signNz(n) * __lp_csc[scast<uint32>(abs(n) * FUNC_PRECISION) % FUNC_PRECISION]; }	//Fast cosecant function that uses a precomputed values to improve performance. Precision: 1/FUNC_PRECISION

template<class T> constexpr auto __vectorcall fsin1(T n) { return signNz(n) * __lp_sin[(abs(n) * FUNC_PRECISION) % FUNC_PRECISION]; }				//Faster version of fsin. This works only with values included in the function period. Precision: 1/FUNC_PRECISION
template<class T> constexpr auto __vectorcall fcos1(T n) { return           __lp_cos[(abs(n) * FUNC_PRECISION) % FUNC_PRECISION]; }					//Faster version of fcos. This works only with values included in the function period. Precision: 1/FUNC_PRECISION
template<class T> constexpr auto __vectorcall ftan1(T n) { return signNz(n) * __lp_tan[(abs(n) * FUNC_PRECISION) % FUNC_PRECISION]; }				//Faster version of ftan. This works only with values included in the function period. Precision: 1/FUNC_PRECISION
template<class T> constexpr auto __vectorcall fcot1(T n) { return signNz(n) * __lp_cot[(abs(n) * FUNC_PRECISION) % FUNC_PRECISION]; }				//Faster version of fcot. This works only with values included in the function period. Precision: 1/FUNC_PRECISION
template<class T> constexpr auto __vectorcall fsec1(T n) { return           __lp_sec[(abs(n) * FUNC_PRECISION) % FUNC_PRECISION]; }					//Faster version of fsec. This works only with values included in the function period. Precision: 1/FUNC_PRECISION
template<class T> constexpr auto __vectorcall fcsc1(T n) { return signNz(n) * __lp_csc[(abs(n) * FUNC_PRECISION) % FUNC_PRECISION]; }				//Faster version of fcsc. This works only with values included in the function period. Precision: 1/FUNC_PRECISION

template<class T> constexpr auto __vectorcall fsin2(T n) { return signNz(n) * __lp_sin[scast<uint32>(abs(n) * FUNC_PRECISION)]; }					//Faster version of fsin. This works only with integral values
template<class T> constexpr auto __vectorcall fcos2(T n) { return           __lp_cos[scast<uint32>(abs(n) * FUNC_PRECISION)]; }						//Faster version of fcos. This works only with integral values
template<class T> constexpr auto __vectorcall ftan2(T n) { return signNz(n) * __lp_tan[scast<uint32>(abs(n) * FUNC_PRECISION)]; }					//Faster version of ftan. This works only with integral values
template<class T> constexpr auto __vectorcall fcot2(T n) { return signNz(n) * __lp_cot[scast<uint32>(abs(n) * FUNC_PRECISION)]; }					//Faster version of fcot. This works only with integral values
template<class T> constexpr auto __vectorcall fsec2(T n) { return           __lp_sec[scast<uint32>(abs(n) * FUNC_PRECISION)]; }						//Faster version of fsec. This works only with integral values
template<class T> constexpr auto __vectorcall fcsc2(T n) { return signNz(n) * __lp_csc[scast<uint32>(abs(n) * FUNC_PRECISION)]; }					//Faster version of fcsc. This works only with integral values

template<class T> constexpr auto __vectorcall ffsin(T n) { return signNz(n) * __lp_sin[(abs(n) * FUNC_PRECISION)]; }								//Fastest version of fsin. This works only with integral values included in the function period
template<class T> constexpr auto __vectorcall ffcos(T n) { return           __lp_cos[(abs(n) * FUNC_PRECISION)]; }									//Fastest version of fcos. This works only with integral values included in the function period
template<class T> constexpr auto __vectorcall fftan(T n) { return signNz(n) * __lp_tan[(abs(n) * FUNC_PRECISION)]; }								//Fastest version of ftan. This works only with integral values included in the function period
template<class T> constexpr auto __vectorcall ffcot(T n) { return signNz(n) * __lp_cot[(abs(n) * FUNC_PRECISION)]; }								//Fastest version of fcot. This works only with integral values included in the function period
template<class T> constexpr auto __vectorcall ffsec(T n) { return           __lp_sec[(abs(n) * FUNC_PRECISION)]; }									//Fastest version of fsec. This works only with integral values included in the function period
template<class T> constexpr auto __vectorcall ffcsc(T n) { return signNz(n) * __lp_csc[(abs(n) * FUNC_PRECISION)]; }								//Fastest version of fcsc. This works only with integral values included in the function period
#endif