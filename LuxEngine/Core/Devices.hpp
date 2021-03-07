#pragma once
#include <GLFW/glfw3.h>
#include "LuxEngine/Types/Containers/LuxString.hpp"
#include "LuxEngine/Core/Devices_t.hpp"
#define LUX_H_DEVICES




namespace lux::core::dvc{
	extern graphicsDevice			graphics;			//Main graphics device
	extern computeDevice			compute;			//Main compute device
	extern RtArray<computeDevice>	secondary;			//Secondary compute devices

	extern VkInstance	instance;						//The only Vulkan instance. This is required in order to use Vulkan
	extern GLFWwindow*  dummyWindow;
	extern VkSurfaceKHR dummySurface;
	// extern VkSurfaceKHR	surface;						//FIXME MOVE TO WINDOW

	extern uint32		requiredDeviceExtensionsNum;	//A GLFW window used to create dummySurface
	extern const char**	requiredDeviceExtensions;		//A window surface used to initialize the Vulkan devices before the application creates the real windows

	#ifdef LUX_DEBUG
		extern VkDebugUtilsMessengerEXT	debugMessenger;
		extern uint32       validationLayersNum;
		extern const char**	validationLayers;
	#endif





	void preInit();
	void				getPhysical();
	void				createLogical(const _VkPhysicalDevice* pPD, VkDevice* pLD, RtArray<VkQueue>* pComputeQueues);
	int32		        rate(         const _VkPhysicalDevice* pDevice);
	bool				isSuitable(      const VkPhysicalDevice vDevice, String* pErrorText);
	bool				checkExtensions( const VkPhysicalDevice vDevice);
	QueueFamilyIndices	getQueueFamilies(const VkPhysicalDevice vDevice);
}
