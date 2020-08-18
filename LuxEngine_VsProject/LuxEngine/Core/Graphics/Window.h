
#pragma once
#include "GLFW/glfw3.h"
#include "LuxEngine/Types/LuxFenceDE.h"
#include "LuxEngine/Memory/Gpu/VMemory.h"



namespace lux::core::g::wnd{
	extern GLFWwindow*		window;					//Main engine window
	extern int32			width, height;			//Size of the window //TODO
	extern FenceDE			windowResizeFence;
	extern ram::Cell		gpuCellWindowSize;		//Size of the widow
	extern ram::Cell		gpuCellWindowOutput;	//Color output of the window
	extern ram::Cell		gpuCellWindowOutput_i;	//Packed color output of the window
	extern ram::Cell		gpuCellWindowZBuffer;	//TODO remove. use render space assembler

	void initWindow( );
	void createInstance( );
}