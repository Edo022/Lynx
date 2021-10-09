#pragma once
////#define LNX_H_DEVICES_T
#include "Lynx/Core/IncludeVulkan.hpp"
#include "Lynx/Types/Integers/Integers.hpp"
#include "Lynx/Types/Containers/RtArray.hpp"
//TODO MOVE TO DEDICATED NAMESPACE







//Available queue families of a physical device
struct QueueFamilyIndices {
	uint32 graphicsFamily = (uint32)-1;
	uint32 presentFamily  = (uint32)-1;
	lnx::RtArray<uint32> computeFamilies;

	QueueFamilyIndices()							: graphicsFamily{ (uint32)-1 },       presentFamily{ (uint32)-1 },      computeFamilies() {}
	QueueFamilyIndices(const QueueFamilyIndices& e) : graphicsFamily{ e.graphicsFamily }, presentFamily{ e.presentFamily }, computeFamilies(e.computeFamilies) {}

	void operator=(const QueueFamilyIndices& i){
		graphicsFamily = i.graphicsFamily;
		presentFamily = i.presentFamily;
		computeFamilies = i.computeFamilies;
	}
	void operator=(const QueueFamilyIndices&& i){
		graphicsFamily = std::move(i.graphicsFamily);
		presentFamily = std::move(i.presentFamily);
		computeFamilies = std::move(i.computeFamilies);
	}
};




//Structure containing all the useful data of a physical device
struct _VkPhysicalDevice {
	_VkPhysicalDevice(const vk::PhysicalDevice vDevice, const vk::PhysicalDeviceProperties& vProperties, const vk::PhysicalDeviceFeatures& vFeatures, const QueueFamilyIndices& vIndices) :
						  device{ vDevice }, properties{ vProperties }, features{ vFeatures }, indices{ vIndices }, score{ 0 } {}
	_VkPhysicalDevice() : device{ nullptr }, properties{             }, features{           }, indices{          }, score{ 0 } {}

	vk::PhysicalDevice				device;		//Actual vk::PhysicalDevice structure
	vk::PhysicalDeviceProperties	properties;	//Physical device properties
	vk::PhysicalDeviceFeatures		features;	//Physical device features
	QueueFamilyIndices				indices;	//Indices of the queue families
	uint32							score;		//Device performances evalutation

	//TODO REMOVE
	auto& operator=(const _VkPhysicalDevice& d){
		device = d.device;
		properties = d.properties;
		features = d.features;
		indices = d.indices;
		score = d.score;
		return *this;
	}
};





struct Device {
	Device() : pd(), ld{ nullptr }, gq{ nullptr }, pq{ nullptr } {}

	_VkPhysicalDevice pd;		//Physical device
	vk::Device		  ld;		//Logical  device
	vk::Queue gq;				//Graphics queue
	vk::Queue pq;				//Present  queue
	lnx::RtArray<vk::Queue> cqs;//Compute  queues
};