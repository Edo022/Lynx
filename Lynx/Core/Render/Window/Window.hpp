#pragma once
#define LNX_H_WINDOW
#include <GLFW/glfw3.h>
#include <atomic>
#include "Lynx/Core/Render/Window/Swapchain.hpp"
#include "Lynx/Types/VPointer.hpp"
#include "Lynx/Types/FenceDE.hpp"
#include "Lynx/shaders/FloatToIntBuffer.hpp"






namespace lnx::obj{
	struct RenderSpace2;
	struct Base;
}
namespace lnx{
	class Window{
	// private:
		GLFWwindow*	window;							//GLFW window object
		int32		width;							//Width of the window
		int32 		height;							//Heght of the window
		std::atomic<bool> running     = false;
		std::atomic<bool> initialized = false;
		Thread t;									//Main render thread of the window

		vram::ptr<u32,   VRam,  Storage> wSize_g;	//Size of the widow
		vram::ptr<f32v4, VRam, Storage> fOut_g ;	//Color output of the window
		vram::ptr<u32,   VRam, Storage> iOut_g ;	//Packed color output of the window
		vram::ptr<u32,   VRam, Storage> zBuff_g;	//TODO remove. use render space assembler



		// RtArray<LnxShaderLayout_t>	CShadersLayouts;
		vk::Pipeline pipelines[ShaderLayout::LNX_DEF_SHADER_NUM];
		vk::CommandPool commandPool;
		vk::CommandPool copyCommandPool;
		RtArray<vk::CommandBuffer> copyCommandBuffers;
		shd::FloatToIntBuffer sh_clear;
		std::mutex addObject_m;

		RaArray<lnx::obj::RenderSpace2*> CRenderSpaces;
		void spawn(obj::RenderSpace2* pRenderSpace);
		RtArray<obj::Base*>	objUpdates;
		std::mutex          objUpdates_m;


		struct InputCallbackQueues{
			struct InputCallbackQueue{
				inline auto add(obj::Base* pElm){
					m.lock();
					auto r = list.add(pElm);
					m.unlock();
					return r;
				}
			private:
				RaArray<obj::Base*> list;
				std::mutex m;
			};
			InputCallbackQueue onClick, onHover, onEnter, onExit, onAxis;
		} icQueues;








		vk::SurfaceKHR surface;
		core::wnd::Swapchain swp;

        vk::ImageMemoryBarrier readToWriteBarrier = vk::ImageMemoryBarrier()	//Create memory barrier object
			.setSrcAccessMask       (vk::AccessFlagBits::eMemoryRead)				//Set source access mask
			.setDstAccessMask       (vk::AccessFlagBits::eTransferWrite)			//Set destination access mask. It must be writable in order to copy the buffer in it
			.setOldLayout           (vk::ImageLayout::eUndefined)					//Set old layout. Swapchain images are in undefined layout after being acquired
			.setNewLayout           (vk::ImageLayout::eTransferDstOptimal)			//Set new layout. Destination optimal allows the image to be used as a transfer destination
			.setSrcQueueFamilyIndex (VK_QUEUE_FAMILY_IGNORED)						//Queue families unset
			.setDstQueueFamilyIndex (VK_QUEUE_FAMILY_IGNORED)						//Queue families unset
			.setSubresourceRange    (vk::ImageSubresourceRange()					//Create subresource object
				.setAspectMask          (vk::ImageAspectFlagBits::eColor)				//Set the aspect mask
				.setBaseMipLevel        (0)												//No mipmap
				.setLevelCount          (1)												//No multi leve images
				.setBaseArrayLayer      (0)												//Set base layer
				.setLayerCount          (1)												//No multi layer
			)
		;
		vk::ImageMemoryBarrier writeToReadBarrier = vk::ImageMemoryBarrier()	//Create memory barrier object
			.setSrcAccessMask       (vk::AccessFlagBits::eTransferWrite)			//Set source access mask
			.setDstAccessMask       (vk::AccessFlagBits::eMemoryRead)				//Set destination access mask. It must be readable to be displayed
			.setOldLayout           (vk::ImageLayout::eTransferDstOptimal)			//Set old layout. Swapchain images is in dst optimal layout after being written
			.setNewLayout           (vk::ImageLayout::ePresentSrcKHR)				//Set new layout. Swapchain images must be in this format to be displayed on screen
			.setSrcQueueFamilyIndex (VK_QUEUE_FAMILY_IGNORED)						//Queue families unset
			.setDstQueueFamilyIndex (VK_QUEUE_FAMILY_IGNORED)						//Queue families unset
			.setSubresourceRange    (vk::ImageSubresourceRange()					//Create subresource object
				.setAspectMask          (vk::ImageAspectFlagBits::eColor)				//Set the aspect mask
				.setBaseMipLevel        (0)												//No mipmap
				.setLevelCount          (1)												//No multi leve images
				.setBaseArrayLayer      (0)												//Set base layer
				.setLayerCount          (1)												//No multi layer
			)
		;
		vk::BufferImageCopy copyRegion = vk::BufferImageCopy() 					//Create bufferImageCopy region to copy the buffer into the image
			.setBufferOffset      (0)												//No buffer offset
			.setBufferRowLength   (0)												//dark magic
			.setBufferImageHeight (0)					    						//dark magic
			.setImageSubresource     (vk::ImageSubresourceLayers()					//Create subresource object
				.setAspectMask        (vk::ImageAspectFlagBits::eColor)   				//Set aspect mask
				.setMipLevel          (0)												//No mipmap
				.setBaseArrayLayer    (0)												//Set base
				.setLayerCount        (1)						    					//No multi layer
			)
			.setImageOffset       (vk::Offset3D(0, 0, 0))							//No image offset
		;




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
		Window(const Window&) = delete;
		Window(const Window&&) = delete;
		void operator=(const Window&) = delete;
		void operator=(const Window&&) = delete;

		void init();

		void close();
		~Window(){ close(); }
	};
}




/*" //TODO
	On some platforms, a window move, resize or menu operation will cause event processing to block. T
	his is due to how event processing is designed on those platforms.
	You can use the window refresh callback to redraw the contents of your window when necessary during such operations.
"*/