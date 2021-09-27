#pragma once
#define LNX_H_RENDER_CORE
#include "Lynx/Core/IncludeVulkan.hpp"
#include "Lynx/Types/FenceDE.hpp"
#include "Lynx/Types/Containers/RtArray.hpp"
#include "Lynx/Types/VPointer.hpp"
#include "Lynx/Types/Vectors/Vectors.hpp"
#include "Lynx/Types/Object/Obj_b.hpp"
#include "Generated/Shaders/FloatToIntBuffer.gsi.hpp"
#include "Lynx/Core/Render/Window/Swapchain.hpp"





namespace lnx{
	class Window;

	namespace obj{
		struct RenderSpace2;
		struct obj_bb;
		struct obj2_b;
	}




	namespace core{
		namespace render{
			extern std::mutex graphicsQueueSubmit_m;
			extern std::mutex presentQueueSubmit_m;

			vk::Format findSupportedFormat(const RtArray<vk::Format>* pCandidates, const vk::ImageTiling vTiling, const vk::FormatFeatureFlags vFeatures);
			uint32     findMemoryType(const uint32 vTypeFilter, const vk::MemoryPropertyFlags vProperties);
			void cleanup();



			#ifdef LNX_DEBUG
				void createDebugMessenger();

				static VKAPI_ATTR VkBool32 VKAPI_CALL vulkanOutputCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
					if((messageSeverity | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
						dbg::logError("Vulkan error: %s", pCallbackData->pMessage);
					}
					else if((messageSeverity | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
						dbg::logWarn("Vulkan warning: %s", pCallbackData->pMessage);
					}
					else {
						dbg::logInfo("Vulkan info: %s", pCallbackData->pMessage);
					}
					return VK_FALSE;
				}
			#endif
		}




		/**
		 * @brief A class that manages the render of the objects owned by a window
		 * This class is only used in lnx::Window
		 */
		class RenderCore{
			friend class lnx::Window;
			friend class lnx::core::wnd::Swapchain;

			void init();
			void run();
			void clear();

			void recSpawn  (obj::obj_bb* pObj);
			void recFlush(obj::obj_bb* pObj, vk::CommandBuffer pCB);
			void recLimit  (obj::obj_bb* pObj);

			void renderLoop();
			void draw(uint32& imageIndex);
			vk::Result present(uint32& imageIndex);

			void updateObjects();
			void sendInputCallbacks();
			void createDefaultCommandBuffers__();

		public:
			Window* w;									//Address of the window that owns the render core. Initialized in the init function of the window
			Thread t;									//Main render thread.                              Initialized in the init function of the window
			vram::ptr<u32v2, eVRam, eStorage> wSize_g;	//Size of the window
			vram::ptr<f32v4, eVRam, eStorage> fOut_g ;	//Color output
			vram::ptr<u32,   eVRam, eStorage> iOut_g ;	//Packed color output
			vram::ptr<u32,   eVRam, eStorage> zBuff_g;	//TODO remove. use render space assembler

			core::wnd::Swapchain swp;
			RtArray<vk::Pipeline> pipelines;

			vk::CommandPool commandPool;
			vk::CommandPool copyCommandPool;
			RtArray<vk::CommandBuffer> copyCommandBuffers;
			shd::gsi::FloatToIntBuffer sh_clear;

			std::mutex addObject_m;
			RtArray<obj::obj_bb*> requests; //TODO USE RAARRAY
			std::mutex            requests_m;

		private:
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
		};
	}
}