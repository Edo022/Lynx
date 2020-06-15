
#include "LuxEngine/Engine/Engine.h"








void Engine::CShader_create_commandBuffers(LuxShader CShader) {
	for (int imgIndex = 0; imgIndex < swapChainImages.size(); imgIndex++) {
		VkCommandPoolCreateInfo commandPoolCreateInfo = {};								//Create command pool create infos. The command pool contains the command buffers
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;			//Set structure type
		commandPoolCreateInfo.flags = 0;													//Default flags
		commandPoolCreateInfo.queueFamilyIndex = compute.PD.indices.computeFamilies[0];		//Set the compute family where to bind the command pool
		//Create the command pool
		Try(vkCreateCommandPool(compute.LD, &commandPoolCreateInfo, null, &CShaders[CShader].commandPool)) Exit("Unable to create command pool");


		VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};						//Create command buffer allocate infos to allocate the command buffer in the command pool
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;	//Set structure type
		commandBufferAllocateInfo.commandPool = CShaders[CShader].commandPool;				//Set command pool where to allocate the command buffer 
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;					//Set the command buffer as a primary level command buffer
		commandBufferAllocateInfo.commandBufferCount = 1;									//Allocate one command buffer
		//Allocate command buffer
		Try(vkAllocateCommandBuffers(compute.LD, &commandBufferAllocateInfo, &CShaders[CShader].commandBuffers[imgIndex])) Exit("Unable to allocate command buffers");


		VkCommandBufferBeginInfo beginInfo = {};										//Create begin infos to start recording the command buffer
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;						//Set structure type
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;						//Set command buffer type
		//Start recording commands
		Try(vkBeginCommandBuffer(CShaders[CShader].commandBuffers[imgIndex], &beginInfo)) Exit("Unable to begin command buffer recording");

		//Bind pipeline and descriptor sets to the command buffer
		vkCmdBindPipeline(CShaders[CShader].commandBuffers[imgIndex], VK_PIPELINE_BIND_POINT_COMPUTE, CShaders[CShader].pipeline);
		vkCmdBindDescriptorSets(CShaders[CShader].commandBuffers[imgIndex], VK_PIPELINE_BIND_POINT_COMPUTE, CShaders[CShader].pipelineLayout, 0, 1, &CShaders[CShader].descriptorSet, 0, null);
		//Dispatch the compute shader to execute it with the specified workgroups
		vkCmdDispatch(CShaders[CShader].commandBuffers[imgIndex], sc<uint32>(ceil(sc<float>(COMPUTE_WIDTH) / WORKGROUP_SIZE)), sc<uint32>(ceil(sc<float>(COMPUTE_HEIGHT) / WORKGROUP_SIZE)), 1);




		//Create a barrier to use the swapchain image as a transfer destination optimal to copy the buffer in it
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = swapChainImages[imgIndex];
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags srcStage, dstStage;
		barrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

		vkCmdPipelineBarrier(CShaders[CShader].commandBuffers[imgIndex], srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);



		//TODO divide to dedicated command buffer and pool
		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { swapChainExtent.width, swapChainExtent.height, 1 };

		if (CGpuBuffers.isValid(0)) /*TODO remove*/ vkCmdCopyBufferToImage(CShaders[CShader].commandBuffers[imgIndex], CGpuBuffers[0].buffer, swapChainImages[imgIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);



		//Create a barrier to use the swapchain image as a present source image
		VkImageMemoryBarrier barrier1{};
		barrier1.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier1.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier1.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		barrier1.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier1.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier1.image = swapChainImages[imgIndex];
		barrier1.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier1.subresourceRange.baseMipLevel = 0;
		barrier1.subresourceRange.levelCount = 1;
		barrier1.subresourceRange.baseArrayLayer = 0;
		barrier1.subresourceRange.layerCount = 1;

		VkPipelineStageFlags srcStage1, dstStage1;
		barrier1.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier1.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		srcStage1 = VK_PIPELINE_STAGE_TRANSFER_BIT;
		dstStage1 = VK_PIPELINE_STAGE_TRANSFER_BIT;

		vkCmdPipelineBarrier(CShaders[CShader].commandBuffers[imgIndex], srcStage1, dstStage1, 0, 0, nullptr, 0, nullptr, 1, &barrier1);




		//End command buffer recording
		Try(vkEndCommandBuffer(CShaders[CShader].commandBuffers[imgIndex])) Exit("Failed to record command buffer");
	}
}