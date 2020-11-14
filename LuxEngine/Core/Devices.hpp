
#pragma once
#include "vulkan/vulkan.h"
#include "LuxEngine/Types/Integers/Integers.hpp"
#include "LuxEngine/Types/Containers/LuxString.hpp"
#include "LuxEngine/Core/Devices_t.hpp"
#define LUX_H_DEVICES




namespace lux::core::dvc{
	extern graphicsDevice			graphics;	//Main graphics device
	extern computeDevice			compute;	//Main compute device
	extern RTArray<computeDevice>	secondary;	//Secondary compute devices


	void preInit( );
	void				deviceGetPhysical( );
	void				deviceCreateLogical(   const _VkPhysicalDevice* pPD,     VkDevice* pLD, RTArray<VkQueue>* pComputeQueues);
	static int32		deviceRate(            const _VkPhysicalDevice* pDevice													);
	bool				deviceIsSuitable(      const VkPhysicalDevice   vDevice, String* pErrorText								);
	bool				deviceCheckExtensions( const VkPhysicalDevice   vDevice													);
	QueueFamilyIndices	deviceGetQueueFamilies(const VkPhysicalDevice   vDevice													);
}
