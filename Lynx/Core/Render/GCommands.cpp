#include "Lynx/Core/Render/GCommands.hpp"
#include "Lynx/Core/Render/Render.hpp"
#include "Lynx/Core/Devices.hpp"
#include "Lynx/Core/Init.hpp"








namespace lnx::core::render::cmd{
	_lnx_init_var_set_def((vk::CommandPool), singleTimeCommandPool, lnx::core::render::cmd) {
		pVar = nullptr;
	}


	_lnx_init_fun_def(LNX_H_GCOMMANDS, lnx::core::render::cmd){
		createGraphicsCommandPool();
		_dbg(createDebugMessenger());
	}




	/** //TODO probably useless
	 * @brief //TODO
	 */
	void createGraphicsCommandPool() {
		auto poolInfo = vk::CommandPoolCreateInfo().setQueueFamilyIndex(dvc::g_graphics().pd.indices.graphicsFamily);
		switch(dvc::g_graphics().ld.createCommandPool(&poolInfo, nullptr, &g_singleTimeCommandPool())){ vkDefaultCases; }
	}




	/** //TODO probably useless
	 * @brief Creates, allocates and begins a command buffer for a single time submit
	 *     This function should only be used by the engine
	 * Complexity: O(1)
	 * @return Returns the created command buffer
	 */
	vk::CommandBuffer beginSingleTimeCommands() {
		vk::CommandBuffer commandBuffer;
		auto allocInfo = vk::CommandBufferAllocateInfo()
			.setCommandPool        (g_singleTimeCommandPool())
			.setLevel              (vk::CommandBufferLevel::ePrimary)
			.setCommandBufferCount (1)
		;
		switch(dvc::g_graphics().ld.allocateCommandBuffers(&allocInfo, &commandBuffer)){ vkDefaultCases; }

		auto beginInfo = vk::CommandBufferBeginInfo().setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
		switch(commandBuffer.begin(&beginInfo)){ vkDefaultCases; }

		return commandBuffer;
	}




	/** //TODO probably useless
	 * @brief Ends and submits a single time submit command, then waits until it's executed and frees its memory
	 *     This function should only be used by the engine
	 * Complexity: Unknown [depends on when the command buffer gets executed by the GPU]
	 */
	void endSingleTimeCommands(const vk::CommandBuffer vCommandBuffer) {
		switch(vCommandBuffer.end()){ vkDefaultCases; }
		auto submitInfo = vk::SubmitInfo()
			.setCommandBufferCount (1)
			.setPCommandBuffers    (&vCommandBuffer)
		;
		core::render::g_graphicsQueueSubmit_m().lock();
			switch(dvc::g_graphics().cqs[0].submit(1, &submitInfo, nullptr)){
				case vk::Result::eErrorDeviceLost: dbg::logError("Device lost"); break;
				vkDefaultCases;
			}
			switch(dvc::g_graphics().cqs[0].waitIdle()){
				case vk::Result::eErrorDeviceLost: dbg::logError("Device lost"); break;
				vkDefaultCases;
			}
		core::render::g_graphicsQueueSubmit_m().unlock();
		dvc::g_graphics().ld.freeCommandBuffers(g_singleTimeCommandPool(), 1, &vCommandBuffer);
	}
}