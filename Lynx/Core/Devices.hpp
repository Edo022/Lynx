#pragma once
#define LNX_H_DEVICES
#include <GLFW/glfw3.h>
#include "Lynx/Core/Devices_t.hpp"
#include "Lynx/Types/Containers/String.hpp"




namespace lnx::core::dvc{
	__init_var_dec(Device,          graphics);		//Main graphics device
	__init_var_dec(RtArray<Device>, secondary);		//Secondary compute devices

	__init_var_dec(vk::Instance,   instance);		//The only Vulkan instance. This is required in order to use Vulkan
	__init_var_dec(GLFWwindow*,    dummyWindow);	//An invisible window used to create dummySurface
	__init_var_dec(vk::SurfaceKHR, dummySurface);	//A window surface used to initialize the Vulkan devices before the application creates the real windows

	__init_var_dec(uint32,       requiredDeviceExtensionsNum);
	__init_var_dec(const char**, requiredDeviceExtensions);

	#ifdef LNX_DBG
		__init_var_dec(VkDebugUtilsMessengerEXT, debugMessenger);
		__init_var_dec(uint32,       validationLayersNum);
		__init_var_dec(const char**, validationLayers);
	#endif

	_lnx_init_fun_dec(LNX_H_DEVICES);




	void getPhysicalDevices();
	void createLogicalDevices(const _VkPhysicalDevice& pPDevice, Device& pDevice);
	u32  rate(const _VkPhysicalDevice& pPDevice);
	bool isSuitable     (const vk::PhysicalDevice vDevice, const bool vAllowLLVM, String* pErrorText);
	bool checkExtensions(const vk::PhysicalDevice vDevice);
	QueueFamilyIndices getQueueFamilies(const vk::PhysicalDevice vDevice);
}
