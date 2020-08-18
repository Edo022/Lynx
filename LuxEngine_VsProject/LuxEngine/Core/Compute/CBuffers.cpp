

#include "LuxEngine/Core/Core.h"
#include "LuxEngine/Core/Compute/CBuffers.h"
#include "LuxEngine/Core/Graphics/Graphics.h"
#include "LuxEngine/Core/Graphics/GCommands.h"
#include "LuxEngine/Core/Devices.h"







namespace lux::core::c::buffers{
	void init( ){

	}








	//Creates and allocates a buffer in the memory of a device
	//*   vDevice: the logical device where to create the buffer
	//*   vSize: the size of the buffer in bytes
	//*   vUsage: flags defining the usage of the buffer (VK_BUFFER_USAGE...)
	//*   vProperties: flags defining the properties of the memory (VK_MEMORY_PROPERTY_...)
	//*   pBuffer: the buffer object to allocate
	//*   pMemory: the memory of the buffer
	void createBuffer(const VkDevice vDevice, const VkDeviceSize vSize, const VkBufferUsageFlags vUsage, const VkMemoryPropertyFlags vProperties, VkBuffer* pBuffer, VkDeviceMemory* pMemory) {
		VkBufferCreateInfo bufferInfo{ };
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = vSize;
		bufferInfo.usage = vUsage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		TryVk(vkCreateBuffer(vDevice, &bufferInfo, nullptr, pBuffer)) printError("Failed to create buffer", true, -1);

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(vDevice, *pBuffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{ };
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = g::findMemoryType(memRequirements.memoryTypeBits, vProperties);

		//TODO check out of memory
		//TODO don't quit in VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS but return an error code
		switch(vkAllocateMemory(vDevice, &allocInfo, nullptr, pMemory)) {
			case VK_SUCCESS: break;
			case VK_ERROR_OUT_OF_DEVICE_MEMORY: {	//IF out of device memory, use the host memory
				VkMemoryAllocateInfo allocInfo2{ };
				allocInfo2.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				allocInfo2.allocationSize = memRequirements.size;
				allocInfo2.memoryTypeIndex = g::findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
				switch(vkAllocateMemory(vDevice, &allocInfo2, nullptr, pMemory)) {
					case VK_SUCCESS: break;
					case VK_ERROR_OUT_OF_HOST_MEMORY: //TODO add case. same as next out of host memory
					default: printError("Failed to allocate buffer memory", true, -1);
				}
				break;
			}
			case VK_ERROR_OUT_OF_HOST_MEMORY:	//TODO If out of host memory
			case VK_ERROR_TOO_MANY_OBJECTS:		//TODO
			default: printError("Failed to allocate buffer memory", true, -1);
		}

		TryVk(vkBindBufferMemory(vDevice, *pBuffer, *pMemory, 0)) printError("Failed to bind buffer", true, -1);
	}




	//TODO remove if not used
	//Creates and submits a command buffer to copy from vSrcBuffer to dstBuffer
	//*   vSrcBuffer: the source buffer where to read the data
	//*   vDstBuffer: the destination buffer where to copy the data
	//*   vSize: the size in bytes of the data to copy
	void copyBuffer(const VkBuffer vSrcBuffer, const VkBuffer vDstBuffer, const VkDeviceSize vSize) {
		VkBufferCopy copyRegion{ };												//Create buffer copy object
		copyRegion.size = vSize;												//Set size of the copied region
		//TODO add offset and automatize cells
		//copyRegion.dstOffset
		VkCommandBuffer commandBuffer = g::cmd::beginSingleTimeCommands( );		//Start command buffer
		vkCmdCopyBuffer(commandBuffer, vSrcBuffer, vDstBuffer, 1, &copyRegion);	//Record the copy command
		g::cmd::endSingleTimeCommands(commandBuffer);							//End command buffer
	}
}