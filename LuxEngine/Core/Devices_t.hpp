#pragma once
#define LUX_H_DEVICES_T
#include "LuxEngine/Types/Integers/Integers.hpp"
#include "LuxEngine/Types/Containers/RtArray.hpp"
#include "vulkan/vulkan_core.h"








//Available queue families of a physical device
struct QueueFamilyIndices {
	uint32					graphicsFamily = (uint32)-1;
	uint32					presentFamily  = (uint32)-1;
	lux::RtArray<uint32>	computeFamilies;

	QueueFamilyIndices( )							: graphicsFamily{ (uint32)-1 },       presentFamily{ (uint32)-1 },      computeFamilies( ) { }
	QueueFamilyIndices(const QueueFamilyIndices& e) : graphicsFamily{ e.graphicsFamily }, presentFamily{ e.presentFamily }, computeFamilies{ e.computeFamilies } { }

	inline bool isGraphicsComplete( ) { return (graphicsFamily != -1 && presentFamily != -1); }
};




//Structure containing all the useful data of a physical device
struct _VkPhysicalDevice {
	_VkPhysicalDevice( ) :					device{ nullptr }, properties{             }, features{           }, indices{          }, score{ 0     } { }
	_VkPhysicalDevice(const VkPhysicalDevice vDevice, const VkPhysicalDeviceProperties& vProperties, const VkPhysicalDeviceFeatures& vFeatures, const QueueFamilyIndices& vIndices) :
											device{ vDevice }, properties{ vProperties }, features{ vFeatures }, indices{ vIndices }, score{ 0     } { }

	VkPhysicalDevice			device;		//Actual VkPhysicalDevice structure
	VkPhysicalDeviceProperties	properties;	//Physical device properties
	VkPhysicalDeviceFeatures	features;	//Physical device features
	QueueFamilyIndices			indices;	//Indices of the queue families
	uint32						score;		//Device performances evalutation
};





struct graphicsDevice {
	graphicsDevice( ) : 				 PD(    ), LD{ nullptr }, graphicsQueue{ nullptr       }, presentQueue{ nullptr      } { }

	_VkPhysicalDevice	PD;					//Main physical device for graphics
	VkDevice			LD;					//Main logical device for graphics
	VkQueue				graphicsQueue;		//Main graphics queue. Runs on graphicsLD
	VkQueue				presentQueue;		//Main graphics queue. Runs on graphicsLD
};




struct computeDevice {
	computeDevice( ) : 					PD(    ), LD{ nullptr }, computeQueues(             ) { }

	_VkPhysicalDevice	PD;					//Main physical device for computing
	VkDevice			LD;					//Main logical device for computing
	lux::RtArray<VkQueue> computeQueues;	//Main compute queues. Run on computeLD
};