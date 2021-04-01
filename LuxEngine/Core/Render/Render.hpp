#pragma once
#define LUX_H_GRAPHICS
#include <vulkan/vulkan.hpp>
#include "LuxEngine/Types/LuxFenceDE.hpp"
#include "LuxEngine/Types/LuxObject/Obj_b.hpp"








namespace lux::core::render{
	extern std::mutex graphicsQueueSubmit_m;
	extern std::mutex presentQueueSubmit_m;


	void cleanup();
	#ifdef LUX_DEBUG
		_dbg(void createDebugMessenger());
	#endif


	vk::Format findSupportedFormat(const RtArray<vk::Format>* pCandidates, const vk::ImageTiling vTiling, const vk::FormatFeatureFlags vFeatures);
	uint32   findMemoryType(const uint32 vTypeFilter, const vk::MemoryPropertyFlags vProperties);
	_dbg(static VKAPI_ATTR VkBool32 VKAPI_CALL vulkanOutputCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
		if((messageSeverity | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
			Failure printf("\nValidation layer error: ");
		}
		else if((messageSeverity | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
			Warning printf("\nValidation layer warning: ");
		}
		else { Normal printf("Validation layer info: "); }
		printf("%s", pCallbackData->pMessage); NormalNoNl;
		return VK_FALSE;
	})
}