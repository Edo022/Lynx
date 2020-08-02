
#pragma once



namespace lux::core::g{
	extern VkCommandPool			singleTimeCommandPool;
	extern Array<VkCommandBuffer>	singleTimeCommandBuffers;

	void				createGraphicsCommandPool();
	VkCommandBuffer		beginSingleTimeCommands();
	void				endSingleTimeCommands(const VkCommandBuffer vCommandBuffer);
}