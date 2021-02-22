#pragma once
#define LUX_H_WINDOW
#include "GLFW/glfw3.h"
#include "LuxEngine/Types/LuxFenceDE.hpp"
// #include "LuxEngine/Core/Memory/Gpu/VMemory.hpp""
#include "LuxEngine/Core/Memory/Gpu/VMemory.hpp"




//TODO use ptrs instead of cells
namespace lux::core::render::wnd{
	extern GLFWwindow*	window;					padExt(window)	//Main engine window
	extern int32		width;					padExt(width)	//Size of the window //TODO
	extern int32 		height;					padExt(width)	//Size of the window //TODO
	extern std::mutex	windowResizeFence;
	extern rem::Cell	gpuCellWindowSize;		padExt(gpuCellWindowSize)		//Size of the widow
	extern rem::Cell	gpuCellWindowOutput;	padExt(gpuCellWindowOutput)		//Color output of the window
	extern rem::Cell	gpuCellWindowOutput_i;	padExt(gpuCellWindowOutput_i)	//Packed color output of the window
	extern rem::Cell	gpuCellWindowZBuffer;	padExt(gpuCellWindowZBuffer)	//TODO remove. use render space assembler



	void preInit( );
	void initWindow( );
	void createInstance( );
}