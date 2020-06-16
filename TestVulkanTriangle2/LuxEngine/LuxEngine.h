

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
	for (int i = 0; i < 360000; i++) {
		#define j ((double)i * (3.1415926535897932 / 180)) / 1000
		*(__lp_sin + i) = sin(j);
		*(__lp_cos + i) = cos(j);
		*(__lp_tan + i) = tan(j);
		*(__lp_cot + i) = 1 / tan(j);
		*(__lp_sec + i) = 1 / cos(j);
		*(__lp_csc + i) = 1 / sin(j);

		*(__lp_sinh + i) = sinh(j);
		*(__lp_cosh + i) = cosh(j);
		*(__lp_tanh + i) = tanh(j);
		*(__lp_coth + i) = 1 / tanh(j);
		*(__lp_sech + i) = 1 / cosh(j);
		*(__lp_csch + i) = 1 / sinh(j);
		#undef j
	}
	while (!engine.__lp_initialized) sleep(10);
}