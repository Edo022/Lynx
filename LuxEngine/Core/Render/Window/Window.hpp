#pragma once
#define LUX_H_WINDOW
#include "GLFW/glfw3.h"
#include "LuxEngine/Types/LuxFenceDE.hpp"
#include "LuxEngine/Types/VPointer.hpp"

#include "LuxEngine/Types/Dummy.hpp"
#include "LuxEngine/Types/LuxBool.hpp"

#include "LuxEngine/Core/Render/Window/Swapchain.hpp"



//TODO use ptrs instead of cells
namespace lux{
	#ifdef LUX_DEBUG
		namespace wnd{
			extern uint32       validationLayersNum;
			extern const char**	validationLayers;
		}
	#endif



	struct Window{
		GLFWwindow*	window = nullptr;		//Main engine window
		int32		width  = 1920 * 2;		//Size of the window //TODO
		int32 		height = 1080;			//Size of the window //TODO
		std::mutex	windowResizeFence;
		vram::ptr<int32, Ram,  Storage> gpuCellWindowSize     = nullptr;	//Size of the widow
		vram::ptr<int32, VRam, Storage> gpuCellWindowOutput   = nullptr;	//Color output of the window
		vram::ptr<int32, VRam, Storage> gpuCellWindowOutput_i = nullptr;	//Packed color output of the window
		vram::ptr<int32, VRam, Storage> gpuCellWindowZBuffer  = nullptr;	//TODO remove. use render space assembler

		core::wnd::Swapchain swapchain;



		Window(){ initWindow(); }

		void initWindow();
		void createInstance();
	};
	extern Window window; //TODO REMOVE
}