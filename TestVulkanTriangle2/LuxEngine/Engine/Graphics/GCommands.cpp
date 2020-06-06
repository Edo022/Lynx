
#include "LuxEngine/Engine/Engine.h"








void Engine::createGraphicsCommandPool() {
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = graphics.PD.indices.graphicsFamily;

	Try(vkCreateCommandPool(graphics.LD, &poolInfo, nullptr, &graphicsCommandPool)) Quit("Failed to create graphics command pool");
}




void Engine::createDrawCommandBuffers() {
	commandBuffers.resize(swapChainFramebuffers.size());				//One command buffer for every swapchain's framebuffer 

	VkCommandBufferAllocateInfo allocInfo{};							//Create allocate infos
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;		//Set structure type
	allocInfo.commandPool = graphicsCommandPool;							//Set command pool	
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;						//Set command buffer level
	allocInfo.commandBufferCount = (uint32)commandBuffers.size();			//Set number of command buffers
	//Allocate command buffers
	Try(vkAllocateCommandBuffers(graphics.LD, &allocInfo, commandBuffers.data())) Quit("Failed to allocate command buffers");

	for (uint64 i = 0; i < commandBuffers.size(); i++) {				//For every command buffer
		VkCommandBufferBeginInfo beginInfo{};								//Create begin info struct
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;		//Set structure type

		VkClearValue clearValues[2];										//Create clear values
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };					//Color clear value
		clearValues[1].depthStencil = { 1.0f, 0 };							//Stencil clear value

		VkRenderPassBeginInfo renderPassInfo{};								//Create render pass infos
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;	//Set structure type
		renderPassInfo.renderPass = renderPass;								//Set render pass
		renderPassInfo.framebuffer = swapChainFramebuffers[i];				//Set frame buffer
		renderPassInfo.renderArea.offset = { 0, 0 };						//No offset
		renderPassInfo.renderArea.extent = swapChainExtent;					//Maximum extent
		renderPassInfo.clearValueCount = 2;									//Clear values number
		renderPassInfo.pClearValues = clearValues;							//Set clear values


		//Begin command buffer and render pass
		Try(vkBeginCommandBuffer(commandBuffers[i], &beginInfo)) Quit("Failed to begin command buffer");
		vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		//Bind pipeline, vertices, indices and descriptors to che command buffer
		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
		vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, &vertexBuffer, new VkDeviceSize(0));
		vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT32); //LLID0
		vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);
		//Draw texture
		vkCmdDrawIndexed(commandBuffers[i], sc<uint32>(indices.size()), 1, 0, 0, 0);

		//Eng command buffer and render pass
		vkCmdEndRenderPass(commandBuffers[i]);
		Try(vkEndCommandBuffer(commandBuffers[i])) Quit("Failed to record command buffer");
	}
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
