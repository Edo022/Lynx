#pragma once
#define LUX_H_GRAPHICS
#include <vulkan/vulkan.h>
#include "LuxEngine/Types/LuxFenceDE.hpp"
#include "LuxEngine/Types/LuxObject/LuxObject.hpp"








namespace lux::core::render{
	extern RtArray<obj::Base*>	objUpdates2D;
	extern std::mutex				objUpdates2D_f;
	extern std::mutex graphicsQueueSubmit_m;
	extern std::mutex presentQueueSubmit_m;


	void cleanup();
	#ifdef LUX_DEBUG
		_dbg(void createDebugMessenger());
	#endif


	VkFormat findSupportedFormat(const RtArray<VkFormat>* pCandidates, const VkImageTiling vTiling, const VkFormatFeatureFlags vFeatures);
	uint32   findMemoryType(const uint32 vTypeFilter, const VkMemoryPropertyFlags vProperties);
	_dbg(static VKAPI_ATTR VkBool32 VKAPI_CALL vulkanOutputCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
		if((messageSeverity | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
			Failure printf("Validation layer error: ");
		}
		else if((messageSeverity | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
			Warning printf("Validation layer warning: ");
		}
		else { Normal printf("Validation layer info: "); }
		printf("%s", pCallbackData->pMessage); NormalNoNl;
		return VK_FALSE;
	})
}