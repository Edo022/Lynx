#pragma once
#define LNX_H_DEVICES
#include <GLFW/glfw3.h>
#include "Lynx/Core/Devices_t.hpp"
#include "Lynx/Types/Containers/String.hpp"




namespace lnx::core::dvc{
	extern Device graphics;	//Main graphics device
	extern RtArray<Device> secondary;	//Secondary compute devices

	extern vk::Instance	instance;				//The only Vulkan instance. This is required in order to use Vulkan
	extern GLFWwindow*  dummyWindow;			//An invisible window used to create dummySurface
	extern vk::SurfaceKHR dummySurface;			//A window surface used to initialize the Vulkan devices before the application creates the real windows

	extern uint32		requiredDeviceExtensionsNum;
	extern const char**	requiredDeviceExtensions;

	#ifdef LNX_DEBUG
		extern VkDebugUtilsMessengerEXT	debugMessenger;
		extern uint32       validationLayersNum;
		extern const char**	validationLayers;
	#endif




	bool checkDriver(const char* vDriverName);
	void chooseDriver();
	void getPhysical();
	void createLogical(const _VkPhysicalDevice& pPDevice, Device& pDevice);
	u32  rate         (const _VkPhysicalDevice& pPDevice);
	bool isSuitable     (const vk::PhysicalDevice vDevice, String* pErrorText);
	bool checkExtensions(const vk::PhysicalDevice vDevice);
	QueueFamilyIndices	getQueueFamilies(const vk::PhysicalDevice vDevice);
}
