#pragma once
#define LUX_H_WINDOW
#include <GLFW/glfw3.h>
#include <atomic>
#include "LuxEngine/Core/Render/Window/Swapchain.hpp"
#include "LuxEngine/Types/VPointer.hpp"
#include "LuxEngine/Types/LuxFenceDE.hpp"
// #include "LuxEngine/Types/LuxObject/LuxObject.hpp"







typedef uint32 LuxShader;
namespace lux::obj{
	struct RenderSpace2D;
	struct Base;
}
namespace lux{
	struct Window{
	// private:
		GLFWwindow*	window;		//GLFW window object
		int32		width;		//Width of the window
		int32 		height;		//Heght of the window

		vram::ptr<int32, Ram,  Storage> wSize_g;	//Size of the widow
		vram::ptr<int32, VRam, Storage> fOut_g ;	//Color output of the window
		vram::ptr<int32, VRam, Storage> iOut_g ;	//Packed color output of the window
		vram::ptr<int32, VRam, Storage> zBuff_g;	//TODO remove. use render space assembler



		RtArray<LuxShaderLayout_t>	CShadersLayouts;
		VkCommandPool commandPool;
		VkCommandPool copyCommandPool;
		RtArray<VkCommandBuffer> copyCommandBuffers;
		LuxShader clearShader;
		std::mutex addShaderFence;



		VkSurfaceKHR surface;
		core::wnd::Swapchain swp;

        VkImageMemoryBarrier readToWriteBarrier{ 			        	//Create memory barrier object
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
			.imageOffset{ 0, 0, 0 }						//No image offset
		};







		std::atomic<bool> running     = false;
		std::atomic<bool> initialized = false;
		Thread t;



		void run();
		void draw();
		void createDefaultCommandBuffers__();




		static void __attribute__((used)) resizeCallback(GLFWwindow* pWindow, int32 vWidth, int32 vHeight){
			((Window*)glfwGetWindowUserPointer(pWindow))->swp.resized = true;
		}




	public:
		Window(uint32 vWidth = 800, uint32 vHeight = 600) : width{ (int32)vWidth }, height{ (int32)vHeight } {
			t(*this, &Window::run);
			while(!initialized){ thr::self::yield(); }
		}

		void init();
		RaArray<lux::obj::RenderSpace2D*> CRenderSpaces;
		void add(obj::RenderSpace2D* pRenderSpace);
		RtArray<obj::Base*>	objUpdates2D;
		std::mutex          objUpdates2D_f;

		void close();
		~Window(){ close(); }
	};
}




/*" //TODO
	On some platforms, a window move, resize or menu operation will cause event processing to block. T
	his is due to how event processing is designed on those platforms.
	You can use the window refresh callback to redraw the contents of your window when necessary during such operations.
"*/