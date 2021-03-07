#pragma once
#define LUX_H_WINDOW
#include "GLFW/glfw3.h"
#include "LuxEngine/Types/LuxFenceDE.hpp"
#include "LuxEngine/Types/VPointer.hpp"
#include "LuxEngine/Core/Render/Window/Swapchain.hpp"
#include "LuxEngine/Types/Dummy.hpp"




namespace lux{
	struct Window{
		GLFWwindow*	window = nullptr;		//Main engine window
		int32		width  = 1920 * 2;		//Size of the window //TODO
		int32 		height = 1080;			//Size of the window //TODO
		std::mutex	windowResizeFence;
		vram::ptr<int32, Ram,  Storage> wSize_g = nullptr;	//Size of the widow
		vram::ptr<int32, VRam, Storage> fOut_G  = nullptr;	//Color output of the window
		vram::ptr<int32, VRam, Storage> iOut_g  = nullptr;	//Packed color output of the window
		vram::ptr<int32, VRam, Storage> zBuff_g = nullptr;	//TODO remove. use render space assembler

		VkSurfaceKHR surface;
		core::wnd::Swapchain swapchain;



		Window(){
			initWindow();
			swapchain.bindedWindow = this;
			swapchain.swapchainCreate();
		}

		void initWindow();
		void createInstance();

		//FIXME MOVE DESTRUCTOR TO .JOIN()
		//FIXME or don't. if the data gets destroyed, the window can't be used
		~Window(){
			wSize_g.free();
			fOut_G.free();
			iOut_g.free();
			zBuff_g.free();
			vkDestroySurfaceKHR(core::dvc::instance, surface, nullptr);
			glfwDestroyWindow(window);
		}
		//TODO ADD JOIN FUNCTION TO WAIT FOR THE WINDOW TO GET CLOSED
	};
	extern Window window; //TODO REMOVE
}