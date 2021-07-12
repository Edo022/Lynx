#include "Lynx/Core/Render/Buffers.hpp"
#include "Lynx/Core/Render/Render.hpp"







namespace lnx::core::buffers{
	/** <pre>
	 * @brief Allocates a buffer in the VRAM of a device	\n
	 *     This function should only be used by the engine	\n
	 * Complexity:											\n
	 *     Best:  O(1) [from core::render::findMemoryType]	\n
	 *     Worst: O(n) [from core::render::findMemoryType]	\n
	 *     where n = number of available memory types
	 * @param pBuffer The buffer object to allocate
	 * @param vUsage Vulkan buffer usage flags
	 * @param vSize The size in bytes of the allocation
	 * @param pMemory The memory of the buffer
	 * @param vProperties Vulkan memory properties flags
	 * @param vDevice The logical device where to allocate the buffer
	 </pre> */
	void createBuffer(vk::Buffer* pBuffer, const vk::BufferUsageFlags vUsage, const vk::DeviceSize vSize, vk::DeviceMemory* pMemory, const vk::MemoryPropertyFlags vProperties, const vk::Device vDevice) {
		auto bufferInfo = vk::BufferCreateInfo()
			.setSize        (vSize)
			.setUsage       (vUsage)
			.setSharingMode (vk::SharingMode::eExclusive)
		;
		switch(vDevice.createBuffer(&bufferInfo, nullptr, pBuffer)){
			case vk::Result::eErrorInvalidOpaqueCaptureAddress: dbg::printError("Invalid opaque capture address"); break;
			vkDefaultCases;
		}


		vk::MemoryRequirements memRequirements;
		vDevice.getBufferMemoryRequirements(*pBuffer, &memRequirements);


		auto memType = render::findMemoryType(memRequirements.memoryTypeBits, vProperties);
		dbg::checkCond(memType == (uint32)-1, "Failed to find suitable memory type");
		auto allocInfo = vk::MemoryAllocateInfo()
			.setAllocationSize  (memRequirements.size)
			.setMemoryTypeIndex (memType)
		;
		switch(vDevice.allocateMemory(&allocInfo, nullptr, pMemory)) {
			case vk::Result::eErrorInvalidOpaqueCaptureAddressKHR: dbg::printError("Invalid opaque capture address"); break;
			case vk::Result::eErrorInvalidExternalHandle:          dbg::printError("Invalid external handle");        break;
			vkDefaultCases;
		}

		switch(vDevice.bindBufferMemory(*pBuffer, *pMemory, 0)){
			case vk::Result::eErrorInvalidOpaqueCaptureAddressKHR: dbg::printError("Invalid opaque capture address"); break;
			vkDefaultCases;
		}
	}
}