#pragma once
#define LNX_H_WINDOW
#include <GLFW/glfw3.h>
#include <atomic>
#include "Lynx/Core/Render/Window/Swapchain.hpp"
#include "Lynx/Types/VPointer.hpp"
#include "Lynx/Types/FenceDE.hpp"
#include "Lynx/shaders/FloatToIntBuffer.hpp"
#include "Lynx/Core/Input/MouseInput.hpp"
#include "Lynx/Core/Render/Render.hpp" //TODO rename as RenderCore



namespace lnx::obj{
	struct RenderSpace2;
	struct Obj_bb;
	struct Obj2_bb;
}
namespace lnx{
	class Window{
	public:
	// private:
		GLFWwindow*	window;							//GLFW window object
		uint32		width;							//Width of the window
		uint32 		height;							//Heght of the window
		std::atomic<bool> running     = false;
		std::atomic<bool> initialized = false;


		core::RenderCore renderCore;


		RtArray<vk::Pipeline> pipelines;
		std::mutex addObject_m;
		void qSpawn(obj::Obj_bb* pObject);


		RtArray<obj::Obj_bb*> requests; //TODO USE RAARRAY
		std::mutex            requests_m;




		struct InputCallbackQueues{
			struct InputCallbackQueue{
				std::atomic<bool> queued = false;
				RaArray<obj::Obj2_bb*> list;
				std::mutex m;
				f32v2 pos;
				inline auto add(obj::Obj2_bb* pElm){
					m.lock();
					auto r = list.add(pElm);
					m.unlock();
					return r;
				}
				inline auto invalidate(f32v2 vPos){
					m.lock();
					queued = true;
					pos = vPos;
					m.unlock();
				}
			};
			InputCallbackQueue onClick, onEnter, onExit, onMove, onAxis;
			MouseButton lastMouseButton = MouseButton::eN1;
		} icQueues;








		vk::SurfaceKHR surface;

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
			.setBufferOffset      	(0)												//No buffer offset
			.setBufferRowLength     (0)												//dark magic
			.setBufferImageHeight   (0)					    						//dark magic
			.setImageSubresource    (vk::ImageSubresourceLayers()					//Create subresource object
				.setAspectMask          (vk::ImageAspectFlagBits::eColor)   			//Set aspect mask
				.setMipLevel            (0)												//No mipmap
				.setBaseArrayLayer      (0)												//Set base
				.setLayerCount          (1)						    					//No multi layer
			)
			.setImageOffset       (vk::Offset3D(0, 0, 0))							//No image offset
		;




		void run();

		void renderLoop();
		void draw(uint32& imageIndex);
		vk::Result present(uint32& imageIndex);
		void updateObjects();
		void sendInputCallbacks();

		void createDefaultCommandBuffers__();


		static void __attribute__((used)) resizeCallback(GLFWwindow* pWindow, int32 vWidth, int32 vHeight){
			((Window*)glfwGetWindowUserPointer(pWindow))->renderCore.swp.resized = true;
		}


	public:
		Window(uint32 vWidth = 800, uint32 vHeight = 600) : width{ vWidth }, height{ vHeight } {
			renderCore.t(*this, &Window::run);
			renderCore.t.detach();
			while(!initialized){ thr::self::yield(); }
		}
		Window(const Window&) = delete;
		Window(const Window&&) = delete;
		void operator=(const Window&) = delete;
		void operator=(const Window&&) = delete;

		void init();

		void clear();

		/**
		 * @brief Waits for the current frame to be rendered and closes the window
		 * Callable by: External threads only
		 * Complexity: O(1)
		 */
		void close();

		~Window(){ close(); }
	};
}




/*" //TODO
	On some platforms, a window move, resize or menu operation will cause event processing to block. T
	his is due to how event processing is designed on those platforms.
	You can use the window refresh callback to redraw the contents of your window when necessary during such operations.
"*/