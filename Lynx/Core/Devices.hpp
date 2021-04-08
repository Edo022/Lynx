#pragma once
#define LUX_H_DEVICES
#include <GLFW/glfw3.h>
#include "Lynx/Core/Devices_t.hpp"
#include "Lynx/Types/Containers/LuxString.hpp"




namespace lux::core::dvc{
	extern graphicsDevice			graphics;	//Main graphics device
	extern computeDevice			compute;	//Main compute device
	extern RtArray<computeDevice>	secondary;	//Secondary compute devices

	extern vk::Instance	instance;				//The only Vulkan instance. This is required in order to use Vulkan
	extern GLFWwindow*  dummyWindow;			//An invisible window used to create dummySurface
	extern vk::SurfaceKHR dummySurface;			//A window surface used to initialize the Vulkan devices before the application creates the real windows

	extern uint32		requiredDeviceExtensionsNum;
	extern const char**	requiredDeviceExtensions;

	#ifdef LUX_DEBUG
		extern VkDebugUtilsMessengerEXT	debugMessenger;
		extern uint32       validationLayersNum;
		extern const char**	validationLayers;
	#endif





	void				getPhysical();
	void				createLogical(const _VkPhysicalDevice* pPD, vk::Device* pLD, RtArray<vk::Queue>* pComputeQueues);
	uint32		        rate(const _VkPhysicalDevice* pDevice);
	bool				isSuitable(      const vk::PhysicalDevice vDevice, String* pErrorText);
	bool				checkExtensions( const vk::PhysicalDevice vDevice);
	QueueFamilyIndices	getQueueFamilies(const vk::PhysicalDevice vDevice);
}
