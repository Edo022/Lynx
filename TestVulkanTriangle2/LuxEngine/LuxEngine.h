

#pragma once

//This is the main include file for the Lux Engine
//Use LuxInit() to initialize and run the engine

//Any variable, macro or function whose name begins with "__lp_" is private, but it's in the public section for performance reasons
//Don't use them if you don't know what you are doing
//Class operators and size() and data() functions are inlined, so the performance is the same as using the __lp_ variables directly

//The LuxDebug() macro executes a line of code only if LUX_DEBUG is defined
//LUX_DEBUG also enables Vulkan validation layers, when available
//The LuxRelease() macro executes a line of code only if LUX_DEBUG is NOT defined
//Those macros does not affect performance or code creation. LUX_DEBUG must be define BEFORE including this header
#include "LuxEngine/Engine/Engine.h"

#include "LuxEngine/Engine/Input/Input.h"





//This function initializes the Lux Engine. Call it only once
static void LuxInit(bool useVSync = true) {
	static Engine engine;
	__lp_luxInit(&engine, useVSync);
	while (!engine.__lp_initialized) sleep(10);
}