#include "LuxEngine/Core/Render/GCommands.hpp"
#include "LuxEngine/Core/Devices.hpp"
#include "LuxEngine/Core/LuxAutoInit.hpp"








namespace lux::core::render::cmd{
	alignCache vk::CommandPool            singleTimeCommandPool = nullptr;
	// alignCache RtArray<vk::CommandBuffer> singleTimeCommandBuffers;




	luxAutoInit(LUX_H_GCOMMANDS){
		createGraphicsCommandPool();
		_dbg(createDebugMessenger());
	}



	void createGraphicsCommandPool() { //FIXME probably useless
		auto poolInfo = vk::CommandPoolCreateInfo().setQueueFamilyIndex(dvc::graphics.PD.indices.graphicsFamily);
		dvc::graphics.LD.createCommandPool(&poolInfo, nullptr, &singleTimeCommandPool);
	}




	/**
	 * @brief Creates, allocates and begins a command buffer for a single time submit
	 * @return Returns the command buffer
	 */
	vk::CommandBuffer beginSingleTimeCommands() {
		vk::CommandBuffer commandBuffer;
		auto allocInfo = vk::CommandBufferAllocateInfo()
			.setCommandPool        (singleTimeCommandPool)
			.setLevel              (vk::CommandBufferLevel::ePrimary)
			.setCommandBufferCount (1)
		;
		dvc::graphics.LD.allocateCommandBuffers(&allocInfo, &commandBuffer);
		auto beginInfo = vk::CommandBufferBeginInfo().setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
		commandBuffer.begin(&beginInfo);
		return commandBuffer;
	}




	/** //TODO probably useless
	 * @brief Ends and submits a single time submit command. Then waits until it's executed and frees its memory
	 */
	void endSingleTimeCommands(const vk::CommandBuffer vCommandBuffer) {
		vCommandBuffer.end();
		auto submitInfo = vk::SubmitInfo()
			.setCommandBufferCount (1)
			.setPCommandBuffers    (&vCommandBuffer)
		;
		core::render::graphicsQueueSubmit_m.lock();
			dvc::graphics.graphicsQueue.submit(1, &submitInfo, VK_NULL_HANDLE);
			dvc::graphics.graphicsQueue.waitIdle();
		core::render::graphicsQueueSubmit_m.unlock();
		dvc::graphics.LD.freeCommandBuffers(singleTimeCommandPool, 1, &vCommandBuffer);
	}
}