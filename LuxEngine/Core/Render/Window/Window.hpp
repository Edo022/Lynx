#pragma once
#define LUX_H_WINDOW
#include <GLFW/glfw3.h>
#include "LuxEngine/Core/Render/Window/Swapchain.hpp"
#include "LuxEngine/Types/VPointer.hpp"
#include "LuxEngine/Types/LuxFenceDE.hpp"
// #include "LuxEngine/Types/LuxObject/LuxObject.hpp"
// #include "LuxEngine/Types/Dummy.hpp"

typedef uint32 LuxShader;


namespace lux::obj{
	struct RenderSpace2D;
}
namespace lux{
	struct Window{
		GLFWwindow*	window = nullptr;		//Main engine window
		int32		width  = 1920;		//Size of the window //TODO
		int32 		height = 1080;			//Size of the window //TODO
		// std::mutex	windowResizeFence;
		vram::ptr<int32, Ram,  Storage> wSize_g = nullptr;	//Size of the widow
		vram::ptr<int32, VRam, Storage> fOut_G  = nullptr;	//Color output of the window
		vram::ptr<int32, VRam, Storage> iOut_g  = nullptr;	//Packed color output of the window
		vram::ptr<int32, VRam, Storage> zBuff_g = nullptr;	//TODO remove. use render space assembler

		RaArray<lux::obj::RenderSpace2D*> CRenderSpaces; //FIXME MAKE WINDOW-LOCAL
		void addRenderSpace(obj::RenderSpace2D* pRenderSpace);


		VkSurfaceKHR surface;
		core::wnd::Swapchain swapchain;

        VkImageMemoryBarrier readToWriteBarrier{ 			        //Create memory barrier object
		.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,	//Set structure type
		.srcAccessMask       = VK_ACCESS_MEMORY_READ_BIT,				//Set source access mask
		.dstAccessMask       = VK_ACCESS_TRANSFER_WRITE_BIT,			//Set destination access mask. It must be writable in order to copy the buffer in it
		.oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED,				//Set old layout. Swapchain images are in undefined layout after being acquired
		.newLayout           = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,	//Set new layout. Destination optimal allows the image to be used as a transfer destination
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,			    	//Queue families unset
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,				    //Queue families unset
		.subresourceRange{												//Create subresource object
			.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,					//Set the aspect mask
			.baseMipLevel   = 0,											//No mipmap
			.levelCount     = 1,											//No multi leve images
			.baseArrayLayer = 0,											//Set base layer
			.layerCount     = 1												//No multi layer
		}
	};




    VkImageMemoryBarrier writeToReadBarrier{				       	//Create memory barrier object
        .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,	//Set structure type
        .srcAccessMask       = VK_ACCESS_TRANSFER_WRITE_BIT,			//Set source access mask
        .dstAccessMask       = VK_ACCESS_MEMORY_READ_BIT,				//Set destination access mask. It must be readable to be displayed
        .oldLayout           = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,	//Set old layout. Swapchain images is in dst optimal layout after being written
        .newLayout           = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,			//Set new layout. Swapchain images must be in this format to be displayed on screen
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,			    	//Queue families unset
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,			    	//Queue families unset
        .subresourceRange{												//Create subresource object
            .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,					//Set the aspect mask
            .baseMipLevel   = 0,											//No mipmap
            .levelCount     = 1,											//No multi leve images
            .baseArrayLayer = 0,											//Set base layer
            .layerCount     = 1												//No multi layer
        }
    };




    VkBufferImageCopy copyRegion{ 				//Create bufferImageCopy region to copy the buffer into the image
        .bufferOffset = 0,							//No buffer offset
        .bufferRowLength = 0,						//dark magic
        .bufferImageHeight = 0,					    //dark magic
        .imageSubresource{							//Create subresource object
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,   	//Set aspect mask
            .mipLevel = 0,								//No mipmap
            .baseArrayLayer = 0,						//Set base
            .layerCount = 1						    	//No multi layer
        },
        .imageOffset{ 0, 0, 0 }										//No image offset
    };


	 VkCommandPool				commandPool = nullptr;
		VkCommandPool            copyCommandPool    = nullptr;
		RtArray<VkCommandBuffer> copyCommandBuffers;
		VkCommandBuffer          clearCommandBuffer = nullptr;
		LuxShader					clearShader = 0;
	 std::mutex							addShaderFence;		//A fence that synchronizes the creation of a new object's shader and the frame render


		void loop();
		bool running = true;
		bool initialized = false;
Thread t;
		Window(){

			t(*this, &Window::loop);
	while(!initialized){}
			// swapchain.bindedWindow = this;
			// swapchain.swapchainCreate();
		}

		void initWindow();
		void createInstance();
		void createDefaultCommandBuffers__();

		//FIXME MOVE DESTRUCTOR TO .JOIN()
		//FIXME or don't. if the data gets destroyed, the window can't be used
		~Window(){
			running = false;
			t.join();
			wSize_g.free();
			fOut_G.free();
			iOut_g.free();
			zBuff_g.free();
			vkDestroySurfaceKHR(core::dvc::instance, surface, nullptr);
			glfwDestroyWindow(window);
		}
		//TODO ADD JOIN FUNCTION TO WAIT FOR THE WINDOW TO GET CLOSED




		static void __attribute__((used)) resizeCallback(GLFWwindow* pWindow, int32 vWidth, int32 vHeight){
			((Window*)glfwGetWindowUserPointer(pWindow))->resize(vWidth, vHeight);
		}
		void resize(int32 vWidth, int32 vHeight);
	};
}