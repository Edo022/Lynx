
#include "LuxEngine/Engine/Engine.h"








void Engine::createGraphicsCommandPool() {
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = graphics.PD.indices.graphicsFamily;

	Try(vkCreateCommandPool(graphics.LD, &poolInfo, nullptr, &graphicsCommandPool)) Exit("Failed to create graphics command pool");
}




VkCommandBuffer Engine::beginSingleTimeCommands() {
	//Create allocate info
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = graphicsCommandPool;
	allocInfo.commandBufferCount = 1;

	//Allocate command buffer
	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(graphics.LD, &allocInfo, &commandBuffer);

	//Begine command recording
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	//Return the created command buffer
	return commandBuffer;
}




void Engine::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
	//End command recording
	vkEndCommandBuffer(commandBuffer);

	//Submit command buffer to queue
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;
	vkQueueSubmit(graphics.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);

	//Free memory
	vkQueueWaitIdle(graphics.graphicsQueue);
	vkFreeCommandBuffers(graphics.LD, graphicsCommandPool, 1, &commandBuffer);
}
