#pragma once


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
	uint32 size;				//The size in bytes of the buffer
	VkBuffer buffer;			//The actual Vulkan buffer
	VkDeviceMemory memory;		//The memory of the buffer
	bool cpuAccessible;
	bool isMapped = false;		//Whether the buffer is mapped or not

	LuxBufferClass bufferClass;	//The class of the buffer
	LuxMap<char> cells;			//This array contains no data. It's used to save the state of a cell (used or free) //TODO use a LuxBitArray
};
