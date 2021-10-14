#pragma once
#include <GLFW/glfw3.h>
#include "Lynx/Core/Devices_t.hpp"
#include "Lynx/Types/Containers/String.hpp"




namespace lnx::core::dvc{
	_lnx_init_var_value_dec((Device),                  graphics);		//Main graphics device
	_lnx_init_var_value_dec((RtArray<Device, uint32>), secondary);	//Secondary compute devices

	_lnx_init_var_value_dec((vk::Instance),   instance);		//The only Vulkan instance. This is required in order to use Vulkan
	_lnx_init_var_value_dec((GLFWwindow*),    dummyWindow);	//An invisible window used to create dummySurface
	_lnx_init_var_value_dec((vk::SurfaceKHR), dummySurface);	//A window surface used to initialize the Vulkan devices before the application creates the real windows

	_lnx_init_var_value_dec((uint32),       requiredDeviceExtensionsNum);
	_lnx_init_var_value_dec((const char**), requiredDeviceExtensions);

	#ifdef LNX_DBG
		_lnx_init_var_value_dec((VkDebugUtilsMessengerEXT), debugMessenger);
		_lnx_init_var_value_dec((uint32),       validationLayersNum);
		_lnx_init_var_value_dec((const char**), validationLayers);
	#endif

	_lnx_init_fun_dec(LNX_H_DEVICES);




	void getPhysicalDevices();
	void createLogicalDevices(const _VkPhysicalDevice& pPDevice, Device& pDevice);
	u32  rate(const _VkPhysicalDevice& pPDevice);
	bool isSuitable     (const vk::PhysicalDevice vDevice, const bool vAllowLLVM, String* pErrorText);
	bool checkExtensions(const vk::PhysicalDevice vDevice);
	QueueFamilyIndices getQueueFamilies(const vk::PhysicalDevice vDevice);
}
