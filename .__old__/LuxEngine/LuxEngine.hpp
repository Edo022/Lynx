
#pragma once
//TODO
#define GLM_FORCE_RADIANS					//Use radiants intead of degrees
#define GLM_FORCE_DEPTH_ZERO_TO_ONE			//0 to 1 depth instead of OpenGL -1 to 1




//UwU
	//TODO
	//The engine is initialized before anything else
	//The "LuxEngine_config.hpp" file contains configuration macros to change the program behaviour






//Names
	//Names of function parameters passed by value start with a lowercase 'v'
	//Names of function parameters passed by reference or pointers start with a lowercase 'p'



//System
	//Supported operating systems:
		//Windows 10
		//Linux
	//Supported compilers:
		//gcc
	//Max (GPU + shared) memory:
		//51200MB
		//This value can be increased by using larger GPU buffers (LUX_CNF_GPU_STATIC_BUFFER_SIZE)
		//Maximum memory will = (LUX_CNF_GPU_STATIC_BUFFER_SIZE * 1024)
