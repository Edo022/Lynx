#pragma once

#include <vulkan/vulkan.h>
#include <LuxEngine/Types/Integers/Integers.h>
#include "LuxEngine/Types/Containers/LuxArray.h"
#include <vector>



//Available queue families of a physical device
struct QueueFamilyIndices {
	uint32 graphicsFamily = -1;
	uint32 presentFamily = -1;
	LuxDynArray<int32> computeFamilies;

	inline bool isGraphicsComplete() { return (graphicsFamily != -1 && presentFamily != -1); }
};




//Structure containing all the useful data of a physical device
struct _VkPhysicalDevice {
	VkPhysicalDevice device = VK_NULL_HANDLE;		//Actual VkPhysicalDevice structure
	VkPhysicalDeviceProperties properties;			//Physical device properties
	VkPhysicalDeviceFeatures features;				//Physical device features
	QueueFamilyIndices indices;						//Indices of the queue families
	uint32 score = 0;								//Device performances evalutation

	_VkPhysicalDevice(VkPhysicalDevice _device, VkPhysicalDeviceProperties _properties, VkPhysicalDeviceFeatures _features, QueueFamilyIndices _indices) {
		device = _device;
		properties = _properties;
		features = _features;
		indices = _indices;
	}
	_VkPhysicalDevice() {}
};
