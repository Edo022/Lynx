#include "Lynx/Core/Render/Buffers.hpp"
#include "Lynx/Core/Render/Render.hpp"







namespace lnx::core::buffers{
	/**
	 * @brief Allocates a buffer in the VRAM of a device
	 * @param pBuffer The buffer object to allocate
	 * @param vUsage Vulkan buffer usage flags
	 * @param vSize The size in bytes of the allocation
	 * @param pMemory The memory of the buffer
	 * @param vProperties Vulkan memory properties flags
	 * @param vDevice The logical device where to allocate the buffer
	 */
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

		auto allocInfo = vk::MemoryAllocateInfo()
			.setAllocationSize  (memRequirements.size)
			.setMemoryTypeIndex (render::findMemoryType(memRequirements.memoryTypeBits, vProperties))
		;
		switch(vDevice.allocateMemory(&allocInfo, nullptr, pMemory)) {
			case vk::Result::eErrorInvalidOpaqueCaptureAddress: dbg::printError("Invalid opaque capture address"); break;
			case vk::Result::eErrorInvalidExternalHandle:       dbg::printError("Invalid external handle");        break;
			vkDefaultCases;
		}

		vDevice.bindBufferMemory(*pBuffer, *pMemory, 0);
	}
}