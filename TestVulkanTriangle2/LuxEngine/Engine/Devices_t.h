#pragma once
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Types/Containers/LuxMap.h"
#include "vulkan/vulkan_core.h"                 // for VkPhysicalDeviceFeatures, VkPhysicalDeviceProperties, VkDevice, VkDevice_T, VkPhysicalDevice, VkPhysicalDevice_T, VkQueue, VkQueue_T, VK_NULL_HANDLE



//Available queue families of a physical device
struct QueueFamilyIndices {
	uint32 graphicsFamily = -1;
	uint32 presentFamily = -1;
	LuxMap<int32, uint32> computeFamilies;

	inline bool isGraphicsComplete() { return (graphicsFamily != -1 && presentFamily != -1); }
};




//Structure containing all the useful data of a physical device
struct _VkPhysicalDevice {
	VkPhysicalDevice device = VK_NULL_HANDLE;		//Actual VkPhysicalDevice structure
	VkPhysicalDeviceProperties properties;			//Physical device properties
	VkPhysicalDeviceFeatures features;				//Physical device features
	QueueFamilyIndices indices;						//Indices of the queue families
	uint32 score = 0;								//Device performances evalutation

	_VkPhysicalDevice(const VkPhysicalDevice vDevice, const VkPhysicalDeviceProperties vProperties, const VkPhysicalDeviceFeatures vFeatures, const QueueFamilyIndices vIndices) {
		device = vDevice;
		properties = vProperties;
		features = vFeatures;
		indices = vIndices;
	}
	_VkPhysicalDevice() {}
};





struct graphicsDevice {
	_VkPhysicalDevice PD;						//Main physical device for graphics
	VkDevice LD;								//Main logical device for graphics
	VkQueue graphicsQueue;						//Main graphics queue. Runs on graphicsLD
	VkQueue presentQueue;						//Main graphics queue. Runs on graphicsLD
};

struct computeDevice {
	_VkPhysicalDevice PD;						//Main physical device for computing
	VkDevice LD;								//Main logical device for computing
	LuxMap<VkQueue, uint32> computeQueues;		//Main compute queues. Run on computeLD
};