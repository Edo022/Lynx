#include "Lynx/Core/Render/GCommands.hpp"
#include "Lynx/Core/Render/Render.hpp"
#include "Lynx/Core/Devices.hpp"
#include "Lynx/Core/AutoInit.hpp"








namespace lnx::core::render::cmd{
	alignCache vk::CommandPool            singleTimeCommandPool = nullptr;




	LnxAutoInit(LNX_H_GCOMMANDS){
		createGraphicsCommandPool();
		_dbg(createDebugMessenger());
	}



	void createGraphicsCommandPool() { //FIXME probably useless
		auto poolInfo = vk::CommandPoolCreateInfo().setQueueFamilyIndex(dvc::graphics.PD.indices.graphicsFamily);
		switch(dvc::graphics.LD.createCommandPool(&poolInfo, nullptr, &singleTimeCommandPool)){
			case vk::Result::eSuccess: break;
			vkDefaultCases;
		}
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
		switch(dvc::graphics.LD.allocateCommandBuffers(&allocInfo, &commandBuffer)){
			case vk::Result::eSuccess: break;
			vkDefaultCases;
		}

		auto beginInfo = vk::CommandBufferBeginInfo().setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

		switch(commandBuffer.begin(&beginInfo)){
			case vk::Result::eSuccess: break;
			vkDefaultCases;
		}

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
			switch(dvc::graphics.graphicsQueue.submit(1, &submitInfo, nullptr)){
				case vk::Result::eSuccess: break;
				case vk::Result::eErrorDeviceLost: dbg::printError("Device lost"); break;
				vkDefaultCases;
			}

			dvc::graphics.graphicsQueue.waitIdle();
		core::render::graphicsQueueSubmit_m.unlock();
		dvc::graphics.LD.freeCommandBuffers(singleTimeCommandPool, 1, &vCommandBuffer);
	}
}