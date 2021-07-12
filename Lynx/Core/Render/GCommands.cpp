#include "Lynx/Core/Render/GCommands.hpp"
#include "Lynx/Core/Render/Render.hpp"
#include "Lynx/Core/Devices.hpp"
#include "Lynx/Core/AutoInit.hpp"








namespace lnx::core::render::cmd{
	alignCache vk::CommandPool singleTimeCommandPool = nullptr;




	LnxAutoInit(LNX_H_GCOMMANDS){
		createGraphicsCommandPool();
		_dbg(createDebugMessenger());
	}




	/** //TODO probably useless
	 * @brief //TODO
	 */
	void createGraphicsCommandPool() {
		auto poolInfo = vk::CommandPoolCreateInfo().setQueueFamilyIndex(dvc::graphics.pd.indices.graphicsFamily);
		switch(dvc::graphics.ld.createCommandPool(&poolInfo, nullptr, &singleTimeCommandPool)){ vkDefaultCases; }
	}




	/** //TODO probably useless
	 * @brief Creates, allocates and begins a command buffer for a single time submit	\n
	 *     This function should only be used by the engine								\n
	 * Complexity: O(1)
	 * @return Returns the created command buffer
	 */
	vk::CommandBuffer beginSingleTimeCommands() {
		vk::CommandBuffer commandBuffer;
		auto allocInfo = vk::CommandBufferAllocateInfo()
			.setCommandPool        (singleTimeCommandPool)
			.setLevel              (vk::CommandBufferLevel::ePrimary)
			.setCommandBufferCount (1)
		;
		switch(dvc::graphics.ld.allocateCommandBuffers(&allocInfo, &commandBuffer)){ vkDefaultCases; }

		auto beginInfo = vk::CommandBufferBeginInfo().setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
		switch(commandBuffer.begin(&beginInfo)){ vkDefaultCases; }

		return commandBuffer;
	}




	/** //TODO probably useless
	 * @brief Ends and submits a single time submit command, then waits until it's executed and frees its memory	\n
	 *     This function should only be used by the engine															\n
	 * Complexity: Unknown [depends on when the command buffer gets executed by the GPU]
	 */
	void endSingleTimeCommands(const vk::CommandBuffer vCommandBuffer) {
		switch(vCommandBuffer.end()){ vkDefaultCases; }
		auto submitInfo = vk::SubmitInfo()
			.setCommandBufferCount (1)
			.setPCommandBuffers    (&vCommandBuffer)
		;
		core::render::graphicsQueueSubmit_m.lock();
			switch(dvc::graphics.cqs[0].submit(1, &submitInfo, nullptr)){
				case vk::Result::eErrorDeviceLost: dbg::printError("Device lost"); break;
				vkDefaultCases;
			}
			switch(dvc::graphics.cqs[0].waitIdle()){
				case vk::Result::eErrorDeviceLost: dbg::printError("Device lost"); break;
				vkDefaultCases;
			}
		core::render::graphicsQueueSubmit_m.unlock();
		dvc::graphics.ld.freeCommandBuffers(singleTimeCommandPool, 1, &vCommandBuffer);
	}
}