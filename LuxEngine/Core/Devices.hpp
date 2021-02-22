
#pragma once
#include "vulkan/vulkan.h"
#include "LuxEngine/Types/Integers/Integers.hpp"
#include "LuxEngine/Types/Containers/LuxString.hpp"
#include "LuxEngine/Core/Devices_t.hpp"
#define LUX_H_DEVICES




namespace lux::core::dvc{
	extern graphicsDevice			graphics;	padExt(graphics)	//Main graphics device
	extern computeDevice			compute;	padExt(compute)		//Main compute device
	extern RtArray<computeDevice>	secondary;	padExt(secondary)	//Secondary compute devices


	void preInit( );
	void				getPhysical( );
	void				createLogical(const _VkPhysicalDevice* pPD, VkDevice* pLD, RtArray<VkQueue>* pComputeQueues);
	int32		        rate(         const _VkPhysicalDevice* pDevice);
	bool				isSuitable(      const VkPhysicalDevice vDevice, String* pErrorText);
	bool				checkExtensions( const VkPhysicalDevice vDevice);
	QueueFamilyIndices	getQueueFamilies(const VkPhysicalDevice vDevice);
}
