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

// namespace core::dvc{
// 	extern VkInstance	instance; //FIXME REMOVE
// }

	struct Window{
		GLFWwindow*	window = nullptr;		//Main engine window
		int32		width  = 1920 * 2;		//Size of the window //TODO
		int32 		height = 1080;			//Size of the window //TODO
		std::mutex	windowResizeFence;
		vram::ptr<int32, Ram,  Storage> gpuCellWindowSize     = nullptr;	//Size of the widow
		vram::ptr<int32, VRam, Storage> gpuCellWindowOutput   = nullptr;	//Color output of the window
		vram::ptr<int32, VRam, Storage> gpuCellWindowOutput_i = nullptr;	//Packed color output of the window
		vram::ptr<int32, VRam, Storage> gpuCellWindowZBuffer  = nullptr;	//TODO remove. use render space assembler

		VkSurfaceKHR surface;
		core::wnd::Swapchain swapchain;



		Window(){
			initWindow();
			swapchain.bindedWindow = this;
			swapchain.swapchainCreate();
		}

		void initWindow();
		void createInstance();

		~Window(){
			gpuCellWindowSize.free();
			gpuCellWindowOutput.free();
			gpuCellWindowOutput_i.free();
			gpuCellWindowZBuffer.free();
			glfwDestroyWindow(window);
			vkDestroySurfaceKHR(core::dvc::instance, surface, nullptr);
		}
	};
	extern Window window; //TODO REMOVE
}