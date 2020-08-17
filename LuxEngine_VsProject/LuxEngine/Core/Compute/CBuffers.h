
#pragma once
#include "vulkan/vulkan.h"
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Types/Containers/LuxMap.h"
#include "LuxEngine/Types/EngineTypes.h"




namespace lux::core::c::buffers{
	////Buffer classes
	////The class of a buffer defines how large are its cells
	////The actual size depends on the device properties, but it's guaranteed to be at least as the class name says
	//enum LuxBufferClass : uint32 {
	//	LUX_BUFFER_CLASS_50,
	//	LUX_BUFFER_CLASS_5K,
	//	LUX_BUFFER_CLASS_500K,
	//	LUX_BUFFER_CLASS_2MLN,
	//	LUX_BUFFER_CLASS_LRG,
	//	LUX_BUFFER_CLASS_NUM
	//};
	//extern Array<uint32> uniformSizes;
	//extern Array<uint32> storageSizes;
	void init( );


	////This structure groups the components of a Vulkan buffer
	//struct LuxBuffer_t {
	//	uint32 size;					//The size in bytes of the buffer
	//	LuxBufferClass bufferClass;		//The class of the buffer
	//	bool cpuAccessible;				//Whether or not the buffer is accessible from the CPU
	//	bool readOnly;					//Whether or not the buffer is read only from the shader

	//	VkBuffer buffer;				//The actual Vulkan buffer
	//	VkDeviceMemory memory;			//The memory of the buffer
	//	lux::Map<int8, uint32> cells;	//This array contains no data. It's used to save the state of a cell (used or free) //TODO use a LuxBitArray
	//	bool isMapped{ false };			//Whether or not the buffer is mapped
	//};
	//extern Map<LuxBuffer_t, uint32>	CBuffers;				//List of GPU buffers





	void		createBuffer(const VkDevice vDevice, const VkDeviceSize vSize, const VkBufferUsageFlags vUsage, const VkMemoryPropertyFlags vProperties, VkBuffer* pBuffer, VkDeviceMemory* pMemory);
	void		copyBuffer(const VkBuffer vSrcBuffer, const VkBuffer vDstBuffer, const VkDeviceSize vSize);

	//LuxBuffer	gpuBufferCreate(const uint32 vSize, const LuxBufferClass vBufferClass, const bool vCpuAccessible, const bool vReadOnly);
	//LuxCell		gpuCellCreate(const uint32 vCellSize, const bool vCpuAccessible, const bool vReadOnly = false);
	//bool		gpuCellDestroy(const LuxCell vCell);
	//void* gpuCellMap(const LuxCell vCell);
}