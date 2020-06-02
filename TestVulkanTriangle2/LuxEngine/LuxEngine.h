

#pragma once

//This is the main include file for the Lux Engine
//Use luxInit() to initialize and run the engine

//Any variable, macro or function whose name begins with "__lp_" is private, but it's in the public section for performance reasons
//Don't use them if you don't know what you are doing
//Class methods such as size(), data(), operators or similar are inlined, so the performance is the same using the variable directly


#include "LuxEngine/Engine/Engine.h"
#include "LuxEngine/Engine/Input/Input.h"

static void luxInit(bool useVSync = true) {
	static Engine engine;
	__lp_luxInit(&engine, useVSync);
	__lp_input_state_set_engine_ptr(&engine);
}