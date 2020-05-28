#pragma once

#include <vulkan/vulkan.h>
#include <vector>



//Available queue families of a physical device
struct QueueFamilyIndices {
	uint32_t graphicsFamily = -1;
	uint32_t presentFamily = -1;
	std::vector<int32_t> computeFamilies;

	bool isGraphicsComplete() { return graphicsFamily != -1 && presentFamily != -1; }
};




//Structure containing all useful data of a physical device
struct _VkPhysicalDevice {
	VkPhysicalDevice device = VK_NULL_HANDLE;		//Actual VkPhysicalDevice
	VkPhysicalDeviceProperties properties;			//Physical device properties
	VkPhysicalDeviceFeatures features;				//Physical device features
	QueueFamilyIndices indices;						//List of queue family indices
	uint32_t score = 0;								//Device performances evalutation

	_VkPhysicalDevice(VkPhysicalDevice _device, VkPhysicalDeviceProperties _properties, VkPhysicalDeviceFeatures _features, QueueFamilyIndices _indices) {
		device = _device;
		properties = _properties;
		features = _features;
		indices = _indices;
	}
	_VkPhysicalDevice() {}
};
