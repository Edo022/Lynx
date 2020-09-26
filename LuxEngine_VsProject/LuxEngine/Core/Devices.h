
#pragma once
#include "vulkan/vulkan.h"
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Types/Containers/LuxString.h"
#include "LuxEngine/Core/Devices_t.h"
#define LUX_H_DEVICES




namespace lux::core::dvc{
	extern graphicsDevice		graphics;		//Main graphics device
	extern computeDevice		compute;		//Main compute device
	extern DynArray<computeDevice>	secondary;		//Secondary compute devices


	void preInit( );
	void				deviceGetPhysical( );
	void				deviceCreateLogical(const _VkPhysicalDevice* pPD, VkDevice* pLD, DynArray<VkQueue>* pComputeQueues);
	static				int32 deviceRate(const _VkPhysicalDevice* pDevice);
	bool				deviceIsSuitable(const VkPhysicalDevice vDevice, String* pErrorText);
	bool				deviceCheckExtensions(const VkPhysicalDevice vDevice);
	QueueFamilyIndices	deviceGetQueueFamilies(const VkPhysicalDevice vDevice);
}
