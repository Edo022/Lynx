
#pragma once
#include "GLFW/glfw3.h"
#include "LuxEngine/Types/LuxFenceDE.h"
#include "LuxEngine/Types/EngineTypes.h"




namespace lux::core::g{
	extern GLFWwindow*		window;								//Main engine window
	extern int32			width, height;	//Size of the window //TODO
	extern FenceDE			windowResizeFence;
	extern LuxCell			gpuCellWindowSize;
	extern LuxCell			gpuCellWindowOutput;				//The buffer that contains the color output of the window
	extern LuxCell			gpuCellWindowOutput_i;				//The buffer that contains the color output of the window

	void initWindow();
	void createInstance();
}