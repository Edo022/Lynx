
#pragma once
#include "vulkan/vulkan.h"
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Types/Containers/LuxMap.h"
#include "LuxEngine/Types/EngineTypes.h"




namespace lux::core::c::buffers{
	//Buffer classes (size of its cells)
	enum LuxBufferClass : uint32 {
		LUX_BUFFER_CLASS_50 = 50,
		LUX_BUFFER_CLASS_5K = 5000,
		LUX_BUFFER_CLASS_500K = 500000,
		LUX_BUFFER_CLASS_2MLN = 2000000,
		LUX_BUFFER_CLASS_LRG = 0
	};


	//This structure groups the components of a Vulkan buffer
	struct LuxBuffer_t {
		uint32 size;					//The size in bytes of the buffer
		LuxBufferClass bufferClass;		//The class of the buffer
		bool cpuAccessible;				//Whether the buffer is accessible from the CPU

		VkBuffer buffer;				//The actual Vulkan buffer
		VkDeviceMemory memory;			//The memory of the buffer
		lux::Map<int8, uint32> cells;	//This array contains no data. It's used to save the state of a cell (used or free) //TODO use a LuxBitArray
		bool isMapped{ false };			//Whether or not the buffer is mapped

		LuxBuffer_t(const uint32 vSize, const LuxBufferClass vBufferClass, const bool vCpuAccessible) :
			size{ vSize }, bufferClass{ vBufferClass }, cpuAccessible{ vCpuAccessible } {
		}
	};




	extern Map<LuxBuffer_t, uint32>	CBuffers;				//List of GPU buffers

	void		createBuffer(const VkDevice vDevice, const VkDeviceSize vSize, const VkBufferUsageFlags vUsage, const VkMemoryPropertyFlags vProperties, VkBuffer* pBuffer, VkDeviceMemory* pMemory);
	void		copyBuffer(const VkBuffer vSrcBuffer, const VkBuffer vDstBuffer, const VkDeviceSize vSize);

	LuxBuffer	gpuBufferCreate(const uint32 vSize, const LuxBufferClass vBufferClass, const bool vCpuAccessible);
	LuxCell		gpuCellCreate(const uint32 vCellSize, const bool vCpuAccessible);
	bool		gpuCellDestroy(const LuxCell vCell);
	void* gpuCellMap(const LuxCell vCell);
}