#pragma once

#include <vulkan/vulkan.h>
#include <LuxEngine/Types/Integers/Integers.h>
#include "LuxEngine/Types/Containers/LuxMap.h"



//Available queue families of a physical device
struct QueueFamilyIndices {
	uint32 graphicsFamily = -1;
	uint32 presentFamily = -1;
	LuxMap<int32> computeFamilies;

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
