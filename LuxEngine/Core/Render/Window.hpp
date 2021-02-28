#pragma once
#define LUX_H_WINDOW
#include "GLFW/glfw3.h"
#include "LuxEngine/Types/LuxFenceDE.hpp"
#include "LuxEngine/Types/VPointer.hpp"

#include "LuxEngine/Types/Dummy.hpp"
#include "LuxEngine/Types/LuxBool.hpp"




//TODO use ptrs instead of cells
namespace lux::core::render::wnd{
	extern GLFWwindow*	window;					//Main engine window
	extern int32		width;					//Size of the window //TODO
	extern int32 		height;					//Size of the window //TODO
	extern std::mutex	windowResizeFence;
	// extern vram::Cell	gpuCellWindowSize;		//Size of the widow
	// extern vram::Cell	gpuCellWindowOutput;	//Color output of the window
	// extern vram::Cell	gpuCellWindowOutput_i;	//Packed color output of the window
	// extern vram::Cell	gpuCellWindowZBuffer;	//TODO remove. use render space assembler
	extern vram::ptr<int32, Ram,  Storage> gpuCellWindowSize;		//Size of the widow
	extern vram::ptr<int32, VRam, Storage> gpuCellWindowOutput;	//Color output of the window
	extern vram::ptr<int32, VRam, Storage> gpuCellWindowOutput_i;	//Packed color output of the window
	extern vram::ptr<int32, VRam, Storage> gpuCellWindowZBuffer;	//TODO remove. use render space assembler



	void preInit( );
	void initWindow( );
	void createInstance( );
}