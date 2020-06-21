
#pragma once
#include "LuxEngine/LuxEngine.config.h"
#include "LuxEngine/Math/Algebra/Algebra.h"
#include "LuxEngine/Math/Trigonometry/Trigonometry.h"
#include "LuxEngine/macros.h"


extern double* __lp_sin;
extern double* __lp_cos;
extern double* __lp_tan;
extern double* __lp_cot;
extern double* __lp_sec;
extern double* __lp_csc;

extern double* __lp_asin;
extern double* __lp_acos;
extern double* __lp_atan;






static void __lp_goniometric_functions_init() {
	for (int i = 0; i < FUNC_PRECISION; i++) {
		double rads = (revToRad(sc<double>(i)) / FUNC_PRECISION);
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
}





//TODO add
#define fsin(n) (sign(n) * __lp_sin[sc<uint32> (abs(n) * FUNC_PRECISION) % FUNC_PRECISION])		//Fast sin function that uses a pre-calculated values to improve the performance. Precision: 1/FUNC_PRECISION
#define fcos(n) (          __lp_cos[sc<uint32> (abs(n) * FUNC_PRECISION) % FUNC_PRECISION])		//Fast cos function that uses a pre-calculated values to improve the performance. Precision: 1/FUNC_PRECISION
#define ftan(n) (sign(n) * __lp_tan[sc<uint32> (abs(n) * FUNC_PRECISION) % FUNC_PRECISION])		//Fast tan function that uses a pre-calculated values to improve the performance. Precision: 1/FUNC_PRECISION
#define fcot(n) (sign(n) * __lp_cot[sc<uint32> (abs(n) * FUNC_PRECISION) % FUNC_PRECISION])		//Fast cot function that uses a pre-calculated values to improve the performance. Precision: 1/FUNC_PRECISION
#define fsec(n) (          __lp_sec[sc<uint32> (abs(n) * FUNC_PRECISION) % FUNC_PRECISION])		//Fast sec function that uses a pre-calculated values to improve the performance. Precision: 1/FUNC_PRECISION
#define fcsc(n) (sign(n) * __lp_csc[sc<uint32> (abs(n) * FUNC_PRECISION) % FUNC_PRECISION])		//Fast csc function that uses a pre-calculated values to improve the performance. Precision: 1/FUNC_PRECISION

#define fsin1(n) (sign(n) * __lp_sin[(abs(n) * FUNC_PRECISION) % FUNC_PRECISION])				//Faster version of fsin. This works only with values included in the function period. Precision: 1/FUNC_PRECISION
#define fcos1(n) (          __lp_cos[(abs(n) * FUNC_PRECISION) % FUNC_PRECISION])				//Faster version of fcos. This works only with values included in the function period. Precision: 1/FUNC_PRECISION
#define ftan1(n) (sign(n) * __lp_tan[(abs(n) * FUNC_PRECISION) % FUNC_PRECISION])				//Faster version of ftan. This works only with values included in the function period. Precision: 1/FUNC_PRECISION
#define fcot1(n) (sign(n) * __lp_cot[(abs(n) * FUNC_PRECISION) % FUNC_PRECISION])				//Faster version of fcot. This works only with values included in the function period. Precision: 1/FUNC_PRECISION
#define fsec1(n) (          __lp_sec[(abs(n) * FUNC_PRECISION) % FUNC_PRECISION])				//Faster version of fsec. This works only with values included in the function period. Precision: 1/FUNC_PRECISION
#define fcsc1(n) (sign(n) * __lp_csc[(abs(n) * FUNC_PRECISION) % FUNC_PRECISION])				//Faster version of fcsc. This works only with values included in the function period. Precision: 1/FUNC_PRECISION

#define fsin2(n) (sign(n) * __lp_sin[sc<uint32> (abs(n) * FUNC_PRECISION)])						//Faster version of fsin. This works only with integral values
#define fcos2(n) (          __lp_cos[sc<uint32> (abs(n) * FUNC_PRECISION)])						//Faster version of fcos. This works only with integral values
#define ftan2(n) (sign(n) * __lp_tan[sc<uint32> (abs(n) * FUNC_PRECISION)])						//Faster version of ftan. This works only with integral values
#define fcot2(n) (sign(n) * __lp_cot[sc<uint32> (abs(n) * FUNC_PRECISION)])						//Faster version of fcot. This works only with integral values
#define fsec2(n) (          __lp_sec[sc<uint32> (abs(n) * FUNC_PRECISION)])						//Faster version of fsec. This works only with integral values
#define fcsc2(n) (sign(n) * __lp_csc[sc<uint32> (abs(n) * FUNC_PRECISION)])						//Faster version of fcsc. This works only with integral values

#define ffsin(n) (sign(n) * __lp_sin[(abs(n) * FUNC_PRECISION)])								//Fastest version of fsin. This works only with integral values included in the function period
#define ffcos(n) (          __lp_cos[(abs(n) * FUNC_PRECISION)])								//Fastest version of fcos. This works only with integral values included in the function period
#define fftan(n) (sign(n) * __lp_tan[(abs(n) * FUNC_PRECISION)])								//Fastest version of ftan. This works only with integral values included in the function period
#define ffcot(n) (sign(n) * __lp_cot[(abs(n) * FUNC_PRECISION)])								//Fastest version of fcot. This works only with integral values included in the function period
#define ffsec(n) (          __lp_sec[(abs(n) * FUNC_PRECISION)])								//Fastest version of fsec. This works only with integral values included in the function period
#define ffcsc(n) (sign(n) * __lp_csc[(abs(n) * FUNC_PRECISION)])								//Fastest version of fcsc. This works only with integral values included in the function period
