
#include "LuxEngine/Engine/Engine.h"








void Engine::CShader_create_commandBuffer(uint64 CShader) {
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
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;						//Set command buffer type. It needs to be continue because it's called every frame
	//beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;				//Set command buffer type. It needs to be continue because it's called every frame
	//Start recording commands
	Try(vkBeginCommandBuffer(CShaders[CShader].commandBuffer, &beginInfo)) Quit("Fatal error");

	//Bind pipeline and descriptor sets to the command buffer
	vkCmdBindPipeline(CShaders[CShader].commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, CShaders[CShader].pipeline);
	vkCmdBindDescriptorSets(CShaders[CShader].commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, CShaders[CShader].pipelineLayout, 0, 1, &CShaders[CShader].descriptorSet, 0, null);
	//Dispatch the compute shader to execute it with the specified workgroups
	vkCmdDispatch(CShaders[CShader].commandBuffer, sc<uint32>(ceil(sc<float>(COMPUTE_WIDTH) / WORKGROUP_SIZE)), sc<uint32>(ceil(sc<float>(COMPUTE_HEIGHT) / WORKGROUP_SIZE)), 1);

	//End command buffer recording
	Try(vkEndCommandBuffer(CShaders[CShader].commandBuffer)) Quit("Fatal error");
}



//TODO set name
void Engine::runCommandBuffer(uint64 CShader) {
	VkCommandBuffer computeCommandBuffers[] = { CShaders[CShader].commandBuffer };
	//Submit the recorded command buffer to a queue
	VkSubmitInfo submitInfo = {};															//Create submit infos to submit the command buffer to the queue
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;											//Set structure type
	submitInfo.commandBufferCount = 1;															//Set number of command buffers
	submitInfo.pCommandBuffers = computeCommandBuffers;											//Set command buffer to submit

	VkFence fence;																			//Create a fence object
	VkFenceCreateInfo fenceCreateInfo = {};													//Create fence create infos to create the fence
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;								//Set structure type
	fenceCreateInfo.flags = 0;																	//Default flags
	Try(vkCreateFence(compute.LD, &fenceCreateInfo, null, &fence)) Quit("Fatal error");		//Create the fence

	Try(vkQueueSubmit(compute.computeQueues[0], 1, &submitInfo, fence)) Quit("Fatal error");//Submit the command buffer to the compute queue //TODO choose queue
	Try(vkWaitForFences(compute.LD, 1, &fence, VK_TRUE, 100000000000)) Quit("Fatal error");	//Wait for the fence. When the command buffer has ended to be executed, the fence is signaled and the program can continue
	vkDestroyFence(compute.LD, fence, null);												//Destroy the fence object
}
