
#include "LuxEngine/Engine/Engine.h"



//Dark magic. Don't worry, it works
static VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallbackFn(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64 object, uint64 location, int32 messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData) {
	printf("Debug Report: %s: %s\n", pLayerPrefix, pMessage);
	return VK_FALSE;
}




void Render::RunCompute() {
	createComputeDescriptorSetLayout();
	createDescriptorSet();
	createComputePipeline();
	createComputeCommandBuffer();
}




void Render::createComputeDescriptorSetLayout() {
	//Specify a binding of type VK_DESCRIPTOR_TYPE_STORAGE_BUFFER to the binding point32 0
	//This binds to
	//  layout(std140, binding = 0) buffer buf
	//in the compute shader
	VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};				//Create a descriptor set layout binding
	descriptorSetLayoutBinding.binding = 0;											//Set binding in shader
	descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;	//Set descriptor type
	descriptorSetLayoutBinding.descriptorCount = 1;									//Set number of descriptors
	descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;			//Use it in the compute stage

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};			//Create descriptor set layout create infos
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;//Set structure type
	descriptorSetLayoutCreateInfo.bindingCount = 1;									//Bind one descriptor
	descriptorSetLayoutCreateInfo.pBindings = &descriptorSetLayoutBinding;			//Set descriptor to bind

	// Create the descriptor set layout. 
	Try(vkCreateDescriptorSetLayout(compute.LD, &descriptorSetLayoutCreateInfo, null, &computeDescriptorSetLayout)) Quit("Fatal error");
}





void Render::createDescriptorSet() {
	VkDescriptorPoolSize descriptorPoolSize = {};
	descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	descriptorPoolSize.descriptorCount = 1;

	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};						//Create descriptor pool create infos
	descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;		//Set structure type
	descriptorPoolCreateInfo.maxSets = 1;												//Allocate only one descriptor set
	descriptorPoolCreateInfo.poolSizeCount = 1;											//One pool size
	descriptorPoolCreateInfo.pPoolSizes = &descriptorPoolSize;							//Set pool size
	//Create descriptor pool
	Try(vkCreateDescriptorPool(compute.LD, &descriptorPoolCreateInfo, null, &computeDescriptorPool)) Quit("Fatal error");


	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};						//Create descriptor set allocate infos
	descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;	//Set structure type
	descriptorSetAllocateInfo.descriptorPool = computeDescriptorPool;					//Set pool where to allocate the descriptor
	descriptorSetAllocateInfo.descriptorSetCount = 1;									//Allocate a single descriptor
	descriptorSetAllocateInfo.pSetLayouts = &computeDescriptorSetLayout;				//Set set layouts
	//Allocate descriptor set
	Try(vkAllocateDescriptorSets(compute.LD, &descriptorSetAllocateInfo, &computeDescriptorSet)) Quit("Fatal error");


	//Connect the storage buffer to the descrptor
	VkDescriptorBufferInfo descriptorBufferInfo = {};								//Create descriptor buffer infos
	descriptorBufferInfo.buffer = buffer;												//Set buffer
	descriptorBufferInfo.offset = 0;													//Set offset
	descriptorBufferInfo.range = bufferSize;											//Set size of the buffer

	VkWriteDescriptorSet writeDescriptorSet = {};									//Create write descriptor set
	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;					//Set structure type
	writeDescriptorSet.dstSet = computeDescriptorSet;									//Set descriptor set
	writeDescriptorSet.dstBinding = 0;													//Set binding
	writeDescriptorSet.descriptorCount = 1;												//Set number of descriptors
	writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;				//Use it as a storage
	writeDescriptorSet.pBufferInfo = &descriptorBufferInfo;								//Set descriptor buffer info

	vkUpdateDescriptorSets(compute.LD, 1, &writeDescriptorSet, 0, null);			//Update descriptor set
}




void Render::createComputePipeline() {
	uint32 fileLength;																//Create the shader module
	computeShaderModule = createShaderModule(compute.LD, readShaderFromFile(&fileLength, "LuxEngine/Contents/shaders/comp.spv"), &fileLength);


	VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};						//Create shader stage infos
	shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;	//Set structure type
	shaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;							//Use it in the compute stage
	shaderStageCreateInfo.module = computeShaderModule;									//Set compute module
	shaderStageCreateInfo.pName = "main";												//Set the main function as entry point


	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};						//Create pipeline create infos
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;		//Set structure type
	pipelineLayoutCreateInfo.setLayoutCount = 1;										//Set number of set layouts
	pipelineLayoutCreateInfo.pSetLayouts = &computeDescriptorSetLayout;					//Set set layout
	//Create pipeline layout
	Try(vkCreatePipelineLayout(compute.LD, &pipelineLayoutCreateInfo, null, &computePipelineLayout)) Quit("Fatal error");


	VkComputePipelineCreateInfo pipelineCreateInfo = {};							//Create pipeline create infos 
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;			//Set structure type
	pipelineCreateInfo.stage = shaderStageCreateInfo;									//Set shader stage infos
	pipelineCreateInfo.layout = computePipelineLayout;									//Set pipeline layout
	//Create the compute pipeline
	Try(vkCreateComputePipelines(compute.LD, VK_NULL_HANDLE, 1, &pipelineCreateInfo, null, &computePipeline)) Quit("Fatal error");

	vkDestroyShaderModule(compute.LD, computeShaderModule, null);					//Destroy the shader module
}




void Render::createComputeCommandBuffer() {
	VkCommandPoolCreateInfo commandPoolCreateInfo = {};								//Create command pool create infos
	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;			//Set structure type
	commandPoolCreateInfo.flags = 0;													//Default flags
	commandPoolCreateInfo.queueFamilyIndex = compute.PD.indices.computeFamilies[0];		//Set the compute family where to bind the command pool
	//Create the command pool
	Try(vkCreateCommandPool(compute.LD, &commandPoolCreateInfo, null, &computeCommandPool)) Quit("Fatal error");


	VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};						//Create command buffer allocate infos to allocate the command buffer in the command pool
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;	//Set structure type
	commandBufferAllocateInfo.commandPool = computeCommandPool;							//Set command pool where to allocate the command buffer 
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;					//Set the command buffer as a primary level command buffer
	commandBufferAllocateInfo.commandBufferCount = 1;									//Allocate one command buffer
	//Allocate command buffer
	Try(vkAllocateCommandBuffers(compute.LD, &commandBufferAllocateInfo, &computeCommandBuffer)) Quit("Fatal error");


	VkCommandBufferBeginInfo beginInfo = {};										//Create begin infos to start recording the command buffer
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;						//Set structure type
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;					//Set command buffer type. It needs to be continue because it's called every frame
	//Start recording commands
	Try(vkBeginCommandBuffer(computeCommandBuffer, &beginInfo)) Quit("Fatal error");	

	//Bind pipeline and descriptor sets to the command buffer
	vkCmdBindPipeline(computeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline);
	vkCmdBindDescriptorSets(computeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipelineLayout, 0, 1, &computeDescriptorSet, 0, null);
	//Dispatch the compute shader to execute it with the specified workgroups
	vkCmdDispatch(computeCommandBuffer, (uint32)ceil((float)(COMPUTE_WIDTH) / WORKGROUP_SIZE), (uint32)ceil((float)(COMPUTE_HEIGHT) / WORKGROUP_SIZE), 1);

	//End command buffer recording
	Try(vkEndCommandBuffer(computeCommandBuffer)) Quit("Fatal error");
}




void Render::runCommandBuffer() {
	//Now we shall finally submit the recorded command buffer to a queue.
	VkSubmitInfo submitInfo = {};															//Create submit infos to submit the command buffer to the queue
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;											//Set structure type
	submitInfo.commandBufferCount = 1;															//Set number of command buffers
	submitInfo.pCommandBuffers = &computeCommandBuffer;											//Set command buffer to submit

	VkFence fence;																			//Create a fence object
	VkFenceCreateInfo fenceCreateInfo = {};													//Create fence create infos to create the fence
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;								//Set structure type
	fenceCreateInfo.flags = 0;																	//Default flags
	Try(vkCreateFence(compute.LD, &fenceCreateInfo, null, &fence)) Quit("Fatal error");		//Create the fence

	Try(vkQueueSubmit(compute.computeQueues[0], 1, &submitInfo, fence)) Quit("Fatal error");//Submit the command buffer to the compute queue //TODO choose queue
	Try(vkWaitForFences(compute.LD, 1, &fence, VK_TRUE, 100000000000)) Quit("Fatal error");	//Wait for the fence. When the command buffer has ended to be executed, the fence is signaled and the program can continue
	vkDestroyFence(compute.LD, fence, null);												//Destroy the fence object
}




void Render::cleanupCompute() {
	vkDestroyBuffer(compute.LD, buffer, null);										//Destroy the buffer
	vkFreeMemory(compute.LD, bufferMemory, null);									//Free the buffer's memory

	vkDestroyDescriptorPool(compute.LD, computeDescriptorPool, null);				//Destroy the descriptor pool
	vkDestroyDescriptorSetLayout(compute.LD, computeDescriptorSetLayout, null);		//Destroy the descriptor set layout

	vkDestroyPipeline(compute.LD, computePipeline, null);							//Destroy the pipeline
	vkDestroyPipelineLayout(compute.LD, computePipelineLayout, null);				//Destroy the pipeline layout

	vkDestroyCommandPool(compute.LD, computeCommandPool, null);						//Destroy the compute command pool
}
