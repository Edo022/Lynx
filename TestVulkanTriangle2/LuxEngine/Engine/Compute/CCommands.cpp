
#include "LuxEngine/Engine/Engine.h"








void Engine::CShader_create_commandBuffer(LuxShader CShader) {
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
	Try(vkAllocateCommandBuffers(compute.LD, &commandBufferAllocateInfo, &CShaders[CShader].commandBuffer)) Quit("Fatal error");


	VkCommandBufferBeginInfo beginInfo = {};										//Create begin infos to start recording the command buffer
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;						//Set structure type
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;						//Set command buffer type
	//Start recording commands
	Try(vkBeginCommandBuffer(CShaders[CShader].commandBuffer, &beginInfo)) Quit("Fatal error");

	//Bind pipeline and descriptor sets to the command buffer
	vkCmdBindPipeline(CShaders[CShader].commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, CShaders[CShader].pipeline);
	vkCmdBindDescriptorSets(CShaders[CShader].commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, CShaders[CShader].pipelineLayout, 0, 1, &CShaders[CShader].descriptorSet, 0, null);
	//Dispatch the compute shader to execute it with the specified workgroups
	vkCmdDispatch(CShaders[CShader].commandBuffer, sc<uint32>(ceil(sc<float>(COMPUTE_WIDTH) / WORKGROUP_SIZE)), sc<uint32>(ceil(sc<float>(COMPUTE_HEIGHT) / WORKGROUP_SIZE)), 1);



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
	//transitionImageLayout(swapChainImages[0], VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL); //TODO hhhh
	if (CGpuBuffers.isValid(0)) vkCmdCopyBufferToImage(CShaders[CShader].commandBuffer, CGpuBuffers[0].buffer, swapChainImages[0], VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, 1, &region);
	//if (CGpuBuffers.isValid(0)) vkCmdCopyBufferToImage(CShaders[CShader].commandBuffer, CGpuBuffers[0].buffer, swapChainImages[1], VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, 1, &region);
	//transitionImageLayout(swapChainImages[0], VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);


	//End command buffer recording
	Try(vkEndCommandBuffer(CShaders[CShader].commandBuffer)) Quit("Fatal error");
}





//TODO set name
void Engine::runCommandBuffer(LuxShader CShader) {
	static VkCommandBuffer computeCommandBuffers[] = { CShaders[CShader].commandBuffer };

	//Submit the recorded command buffer to a queue
	static VkSubmitInfo submitInfo = {};													//Create submit infos to submit the command buffer to the queue
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;											//Set structure type
	submitInfo.commandBufferCount = 1;															//Set number of command buffers
	submitInfo.pCommandBuffers = computeCommandBuffers;											//Set command buffer to submit

	static VkFence fence;																	//Create a fence object
	static VkFenceCreateInfo fenceCreateInfo = {};											//Create fence create infos to create the fence
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;								//Set structure type
	fenceCreateInfo.flags = 0;																	//Default flags


	Try(vkCreateFence(compute.LD, &fenceCreateInfo, null, &fence)) Quit("Fatal error");		//Create the fence
	Try(vkQueueSubmit(compute.computeQueues[0], 1, &submitInfo, fence)) Quit("Fatal error");//Submit the command buffer to the compute queue //TODO choose queue
	Try(vkWaitForFences(compute.LD, 1, &fence, VK_TRUE, 100000000000)) Quit("Fatal error");	//Wait for the fence. When the command buffer has ended to be executed, the fence is signaled and the program can continue
	vkDestroyFence(compute.LD, fence, null);												//Destroy the fence object
}
