#pragma once
#define LNX_H_RENDER_CORE
#include "Lynx/Core/IncludeVulkan.hpp"
#include "Lynx/Types/FenceDE.hpp"
#include "Lynx/Types/Containers/RtArray.hpp"
#include "Lynx/Types/VPointer.hpp"
#include "Lynx/Types/Vectors/Vectors.hpp"








namespace lnx{
	class Window;

	namespace obj{
		struct RenderSpace2;
		struct Obj_bb;
		struct Obj2_bb;
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
						Failure printf("\nVulkan error: ");
					}
					else if((messageSeverity | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
						Warning printf("\nVulkan warning: ");
					}
					else { Normal printf("Vulkan info: "); }
					printf("%s", pCallbackData->pMessage); NormalNoNl;
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
			void init();	//Initializes the render core.              This function must be called from lnx::Window::init()  only
			void clear();	//Stops the render and frees its resources. This function must be called from lnx::Window::clear() only

			void recSpawn  (obj::Obj_bb* pObj, Window& pWindow);
			void recUpdateg(obj::Obj_bb* pObj, vk::CommandBuffer pCB);
			void recLimit  (obj::Obj_bb* pObj);

		public:
			Window* w;									//Address of the window that owns the render core. Initialized in the init function of the window
			Thread t;									//Main render thread.                              Initialized in the init function of the window
			vram::ptr<u32v2, eVRam, eStorage> wSize_g;	//Size of the window
			vram::ptr<f32v4, eVRam, eStorage> fOut_g ;	//Color output
			vram::ptr<u32,   eVRam, eStorage> iOut_g ;	//Packed color output
			vram::ptr<u32,   eVRam, eStorage> zBuff_g;	//TODO remove. use render space assembler
		};
	}
}