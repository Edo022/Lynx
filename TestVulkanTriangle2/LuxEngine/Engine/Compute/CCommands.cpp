
#include "LuxEngine/Engine/Engine.h"








void Engine::CShader_create_commandBuffers(LuxShader CShader) {
	for (int imgIndex = 0; imgIndex < swapChainImages.size(); imgIndex++) {
		VkCommandPoolCreateInfo commandPoolCreateInfo = {};								//Create command pool create infos. The command pool contains the command buffers
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;			//Set structure type
		commandPoolCreateInfo.flags = 0;													//Default flags
		commandPoolCreateInfo.queueFamilyIndex = compute.PD.indices.computeFamilies[0];		//Set the compute family where to bind the command pool
		//Create the command pool
		Try(vkCreateCommandPool(compute.LD, &commandPoolCreateInfo, null, &CShaders[CShader].commandPool)) Quit("Fatal error");


		VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};						//Create command buffer allocate infos to allocate the command buffer in the command pool
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;	//Set structure type
		commandBufferAllocateInfo.commandPool = CShaders[CShader].commandPool;				//Set command pool where to allocate the command buffer 
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;					//Set the command buffer as a primary level command buffer
		commandBufferAllocateInfo.commandBufferCount = 1;									//Allocate one command buffer
		//Allocate command buffer
		Try(vkAllocateCommandBuffers(compute.LD, &commandBufferAllocateInfo, &CShaders[CShader].commandBuffers[imgIndex])) Quit("Fatal error");


		VkCommandBufferBeginInfo beginInfo = {};										//Create begin infos to start recording the command buffer
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;						//Set structure type
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;						//Set command buffer type
		//Start recording commands
		Try(vkBeginCommandBuffer(CShaders[CShader].commandBuffers[imgIndex], &beginInfo)) Quit("Fatal error");

		//Bind pipeline and descriptor sets to the command buffer
		vkCmdBindPipeline(CShaders[CShader].commandBuffers[imgIndex], VK_PIPELINE_BIND_POINT_COMPUTE, CShaders[CShader].pipeline);
		vkCmdBindDescriptorSets(CShaders[CShader].commandBuffers[imgIndex], VK_PIPELINE_BIND_POINT_COMPUTE, CShaders[CShader].pipelineLayout, 0, 1, &CShaders[CShader].descriptorSet, 0, null);
		//Dispatch the compute shader to execute it with the specified workgroups
		vkCmdDispatch(CShaders[CShader].commandBuffers[imgIndex], sc<uint32>(ceil(sc<float>(COMPUTE_WIDTH) / WORKGROUP_SIZE)), sc<uint32>(ceil(sc<float>(COMPUTE_HEIGHT) / WORKGROUP_SIZE)), 1);


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
		region.imageExtent = { WIDTH, HEIGHT, 1 };

		transitionImageLayout(swapChainImages[imgIndex], VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		if (CGpuBuffers.isValid(0)) vkCmdCopyBufferToImage(CShaders[CShader].commandBuffers[imgIndex], CGpuBuffers[0].buffer, swapChainImages[imgIndex], VK_IMAGE_LAYOUT_UNDEFINED, 1, &region);
		transitionImageLayout(swapChainImages[imgIndex], VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);


		//End command buffer recording
		Try(vkEndCommandBuffer(CShaders[CShader].commandBuffers[imgIndex])) Quit("Fatal error");
	}
}