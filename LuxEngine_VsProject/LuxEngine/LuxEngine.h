
#pragma once




//UwU
	//This is the header to include in order to use the LuxEngine
	//Call LuxInit() to initialize and run the engine
	//It will open a default graphic window
	//You can configure the engine in the LuxEngine_config.h file






//Names
	//variableName			//functionName		//macroName		//basicTypeName
	//ComplexTypeName		//FileName
	//CONFIG_MACRO_NAME		//ENUM_MEMBER_NAME
	//Any variable, macro or function whose name begins with "__lp_" is private, but it's in the public section for performance reasons
	//Don't use them if you don't know what you're doing
	//Files ending with _t only contains the structures needed by the file with the same name
	//Files ending with _e only contains the extern variables and static members needed by the file with the same name
	//Function parameters passed by value have a name that starts with a lowercase v
	//Function parameters passed by reference or pointers have a name that starts with a lowercase p
	//Some types are just indices of the instanciated objects, like LuxShader or LuxCell. The actual type has the same name, but it ends with _t


//Performance
	//Class operators and size() and data() functions are inlined, so the performance is the same as using the __lp_ variables directly
	//The luxDebug()   macro executes a line of code only if LUX_DEBUG is defined
	//The luxRelease() macro executes a line of code only if LUX_DEBUG is NOT defined
	//luxDebug and luxRelease macros do not affect performance or code generation. They just remove a line of code
	//LUX_DEBUG also enables Vulkan validation layers, when available. They'll affect performance
	//LUX_DEBUG must be define BEFORE including this header


//System
	//Supported operating systems:
		//Windows 10
		//TODO
	//Supported compilers:
		//Visual studio 2019 C++
		//?
	//Max (GPU + shared) memory:
		//51200MB
		//This value can be increased by using larger GPU buffers. See GPU_STATIC_BUFFER_SIZE in LuxEngine_config.h
		//In that case, the new maximum memory will be (GPU_STATIC_BUFFER_SIZE * 1024)






#include "LuxEngine/Engine/Engine.h"

//Initializes the Engine
//Objects will automatically call this function when created
//    They need the engine to be initialized to allocate their data
static void LuxInit(bool useVSync = true) { lux::engine( ).init(useVSync); }



