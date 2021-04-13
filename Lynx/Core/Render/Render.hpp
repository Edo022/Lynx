#pragma once
#define LNX_H_GRAPHICS
#include "Lynx/Core/IncludeVulkan.hpp"
#include "Lynx/Types/FenceDE.hpp"
#include "Lynx/Types/Object/Obj_b.hpp"








namespace lnx::core::render{
	extern std::mutex graphicsQueueSubmit_m;
	extern std::mutex presentQueueSubmit_m;


	void cleanup();
	#ifdef LNX_DEBUG
		_dbg(void createDebugMessenger());
	#endif


	vk::Format findSupportedFormat(const RtArray<vk::Format>* pCandidates, const vk::ImageTiling vTiling, const vk::FormatFeatureFlags vFeatures);
	uint32   findMemoryType(const uint32 vTypeFilter, const vk::MemoryPropertyFlags vProperties);
	_dbg(static VKAPI_ATTR VkBool32 VKAPI_CALL vulkanOutputCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
		if((messageSeverity | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
			Failure printf("\nVulkan error: ");
		}
		else if((messageSeverity | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
			Warning printf("\nVulkan warning: ");
		}
		else { Normal printf("Vulkan info: "); }
		printf("%s", pCallbackData->pMessage); NormalNoNl;
		return VK_FALSE;
	})
}