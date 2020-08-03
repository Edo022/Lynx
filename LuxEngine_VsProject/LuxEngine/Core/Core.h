
#pragma once
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Core/Graphics/Graphics.h"
#include "vulkan/vulkan.h"


namespace lux::core{
	extern double	FPS;
	extern float	FOV;
	extern bool		running;
	extern bool		useVSync;
	extern bool		initialized;
	extern uint32	frames;

	extern VkInstance				instance;
	extern VkDebugUtilsMessengerEXT	debugMessenger;
	extern VkSurfaceKHR				surface;

	extern Array<const char*, uint32> validationLayers;
	extern Array<const char*, uint32> requiredDeviceExtensions;




	void init(bool useVSync);
	void run(bool vUseVSync, float vFOV);
	void mainLoop( );
	void runFPSCounterThr( );
	void runRenderThr( );
}





namespace lux::_engine {
	//It's dark magic, idk why or how it works, but it does
	inline VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if(func != nullptr) return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		else return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
	inline void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if(func != nullptr) func(instance, debugMessenger, pAllocator);
	}


	//More dark magic
	static constexpr inline void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
		createInfo = { };
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = lux::core::g::graphicsDebugCallback;
	}
}
