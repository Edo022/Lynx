#pragma once
#define LNX_H_RENDER_CORE
#include "Lynx/Core/IncludeVulkan.hpp"
#include "Lynx/Types/FenceDE.hpp"
#include "Lynx/Types/Containers/RtArray.hpp"






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




		class RenderCore{
			friend class lnx::Window;

			void init();
			void cleanup();

			void recSpawn  (obj::Obj_bb* pObj, Window& pWindow);
			void recUpdateg(obj::Obj_bb* pObj, vk::CommandBuffer pCB);
			void recLimit  (obj::Obj_bb* pObj);

		public:
			Window* w;
		};
	}
}