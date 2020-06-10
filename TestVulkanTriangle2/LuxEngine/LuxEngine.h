

#pragma once

//This is the main include file for the Lux Engine
//Use LuxInit() to initialize and run the engine

//Any variable, macro or function whose name begins with "__lp_" is private, but it's in the public section for performance reasons
//Don't use them if you don't know what you are doing
//Class operators and size() and data() functions are inlined, so the performance is the same as using the __lp_ variables directly
#include "LuxEngine/Engine/Engine.h"

#include "LuxEngine/Engine/Input/Input.h"





//This function initializes the Lux Engine. Call it only once
static void LuxInit(bool useVSync = true) {
	static Engine engine;
	__lp_luxInit(&engine, useVSync);
	while (!engine.initialized) sleep(10);
}