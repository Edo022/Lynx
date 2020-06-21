

#pragma once

//This is the main include file for the Lux Engine
//Use LuxInit() to initialize and run the engine

//Any variable, macro or function whose name begins with "__lp_" is private, but it's in the public section for performance reasons
//Don't use them if you don't know what you are doing
//Class operators and size() and data() functions are inlined, so the performance is the same as using the __lp_ variables directly

//The luxDebug() macro executes a line of code only if LUX_DEBUG is defined
//LUX_DEBUG also enables Vulkan validation layers, when available
//The luxRelease() macro executes a line of code only if LUX_DEBUG is NOT defined
//Those macros does not affect performance or code creation. LUX_DEBUG must be define BEFORE including this header
#include "LuxEngine/Engine/Engine.h"

#include "LuxEngine/Engine/Input/Input.h"




//This function initializes the Lux Engine. Call it only once
static void LuxInit(bool useVSync = true) {
	__lp_luxInit(useVSync);
	for (int i = 0; i < precision; i++) {
		double rads = (revToRad(sc<double>(i)) / precision);
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

	while (!engine.__lp_initialized) sleep(10);
}