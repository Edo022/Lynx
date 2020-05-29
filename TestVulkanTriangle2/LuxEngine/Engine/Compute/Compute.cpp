
#include "LuxEngine/Engine/Engine.h"


static VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallbackFn(
	VkDebugReportFlagsEXT			flags,
	VkDebugReportObjectTypeEXT		objectType,
	uint64							object,
	uint64							location,
	int32							messageCode,
	const char* pLayerPrefix,
	const char* pMessage,
	void* pUserData) {

	printf("Debug Report: %s: %s\n", pLayerPrefix, pMessage);

	return VK_FALSE;
}





void Render::run2() {
	// Buffer size of the storage buffer that will contain the rendered mandelbrot set.
	bufferSize = sizeof(Pixel) * COMPUTE_WIDTH * COMPUTE_HEIGHT;

	createBuffer(bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, buffer, bufferMemory);
	createComputeDescriptorSetLayout();
	createDescriptorSet();
	createComputePipeline();
	createComputeCommandBuffer();
}



void Render::createComputeDescriptorSetLayout() {
	//Here we specify a binding of type VK_DESCRIPTOR_TYPE_STORAGE_BUFFER to the binding point 0.
	//This binds to
	//  layout(std140, binding = 0) buffer buf
	//in the compute shader.
	VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
	descriptorSetLayoutBinding.binding = 0; // binding = 0
	descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	descriptorSetLayoutBinding.descriptorCount = 1;
	descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.bindingCount = 1; // only a single binding in this descriptor set layout. 
	descriptorSetLayoutCreateInfo.pBindings = &descriptorSetLayoutBinding;

	// Create the descriptor set layout. 
	Try(vkCreateDescriptorSetLayout(compute.LD, &descriptorSetLayoutCreateInfo, NULL, &computeDescriptorSetLayout)) Quit("Fatal error");
}

void Render::createDescriptorSet() {
	//So we will allocate a descriptor set here.
	//But we need to first create a descriptor pool to do that.
	//Our descriptor pool can only allocate a single storage buffer.
	VkDescriptorPoolSize descriptorPoolSize = {};
	descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	descriptorPoolSize.descriptorCount = 1;

	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
	descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCreateInfo.maxSets = 1; // we only need to allocate one descriptor set from the pool.
	descriptorPoolCreateInfo.poolSizeCount = 1;
	descriptorPoolCreateInfo.pPoolSizes = &descriptorPoolSize;

	// create descriptor pool.
	Try(vkCreateDescriptorPool(compute.LD, &descriptorPoolCreateInfo, NULL, &computeDescriptorPool)) Quit("Fatal error");

	//With the pool allocated, we can now allocate the descriptor set.
	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
	descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.descriptorPool = computeDescriptorPool; // pool to allocate from.
	descriptorSetAllocateInfo.descriptorSetCount = 1; // allocate a single descriptor set.
	descriptorSetAllocateInfo.pSetLayouts = &computeDescriptorSetLayout;

	// allocate descriptor set.
	Try(vkAllocateDescriptorSets(compute.LD, &descriptorSetAllocateInfo, &computeDescriptorSet)) Quit("Fatal error");

	//Next, we need to connect our actual storage buffer with the descrptor.
	//We use vkUpdateDescriptorSets() to update the descriptor set.

	// Specify the buffer to bind to the descriptor.
	VkDescriptorBufferInfo descriptorBufferInfo = {};
	descriptorBufferInfo.buffer = buffer;
	descriptorBufferInfo.offset = 0;
	descriptorBufferInfo.range = bufferSize;

	VkWriteDescriptorSet writeDescriptorSet = {};
	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.dstSet = computeDescriptorSet;						// write to this descriptor set.
	writeDescriptorSet.dstBinding = 0;										// write to the first, and only binding.
	writeDescriptorSet.descriptorCount = 1;									// update a single descriptor.
	writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;	// storage buffer.
	writeDescriptorSet.pBufferInfo = &descriptorBufferInfo;

	// perform the update of the descriptor set.
	vkUpdateDescriptorSets(compute.LD, 1, &writeDescriptorSet, 0, NULL);
}




//We create a compute pipeline here.
void Render::createComputePipeline() {
	//Create a shader module. A shader module basically just encapsulates some shader code.
	uint32 filelength;
	// the code in comp.spv was created by running the command:
	// glslangValidator.exe -V shader.comp
	uint32* code = readShaderFromFile(&filelength, "LuxEngine/Contents/shaders/comp.spv"); //TODO use unique read file function for shaders
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.pCode = code;
	createInfo.codeSize = filelength;

	Try(vkCreateShaderModule(compute.LD, &createInfo, NULL, &computeShaderModule)) Quit("Fatal error");
	delete[] code;

	//Now let us actually create the compute pipeline.
	//A compute pipeline is very simple compared to a graphics pipeline.
	//It only consists of a single stage with a compute shader.

	//So first we specify the compute shader stage, and it's entry point(main).
	VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};
	shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	shaderStageCreateInfo.module = computeShaderModule;
	shaderStageCreateInfo.pName = "main";

	//The pipeline layout allows the pipeline to access descriptor sets.
	//So we just specify the descriptor set layout we created earlier.
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts = &computeDescriptorSetLayout;
	Try(vkCreatePipelineLayout(compute.LD, &pipelineLayoutCreateInfo, NULL, &computePipelineLayout)) Quit("Fatal error");

	VkComputePipelineCreateInfo pipelineCreateInfo = {};
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.stage = shaderStageCreateInfo;
	pipelineCreateInfo.layout = computePipelineLayout;

	//Now, we finally create the compute pipeline.
	Try(vkCreateComputePipelines(compute.LD, VK_NULL_HANDLE, 1, &pipelineCreateInfo, NULL, &computePipeline)) Quit("Fatal error");
}

void Render::createComputeCommandBuffer() {
	//We are getting closer to the end. In order to send commands to the device(GPU),
	//we must first record commands into a command buffer.
	//To allocate a command buffer, we must first create a command pool. So let us do that.
	VkCommandPoolCreateInfo commandPoolCreateInfo = {};
	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCreateInfo.flags = 0;
	// the queue family of this command pool. All command buffers allocated from this command pool,
	// must be submitted to queues of this family ONLY. 
	commandPoolCreateInfo.queueFamilyIndex = compute.PD.indices.computeFamilies[0];
	Try(vkCreateCommandPool(compute.LD, &commandPoolCreateInfo, NULL, &computeCommandPool)) Quit("Fatal error");

	//Now allocate a command buffer from the command pool.
	VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.commandPool = computeCommandPool; // specify the command pool to allocate from. 
	// if the command buffer is primary, it can be directly submitted to queues. 
	// A secondary buffer has to be called from some primary command buffer, and cannot be directly 
	// submitted to a queue. To keep things simple, we use a primary command buffer. 
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandBufferCount = 1; // allocate a single command buffer. 
	Try(vkAllocateCommandBuffers(compute.LD, &commandBufferAllocateInfo, &computeCommandBuffer)) Quit("Fatal error"); // allocate command buffer.

	//Now we shall start recording commands into the newly allocated command buffer.
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; // the buffer is only submitted and used once in this application.
	Try(vkBeginCommandBuffer(computeCommandBuffer, &beginInfo)) Quit("Fatal error"); // start recording commands.

	//We need to bind a pipeline, AND a descriptor set before we dispatch.

	//The validation layer will NOT give warnings if you forget these, so be very careful not to forget them.
	vkCmdBindPipeline(computeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline);
	vkCmdBindDescriptorSets(computeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipelineLayout, 0, 1, &computeDescriptorSet, 0, NULL);

	//Calling vkCmdDispatch basically starts the compute pipeline, and executes the compute shader.
	//The number of workgroups is specified in the arguments.
	//If you are already familiar with compute shaders from OpenGL, this should be nothing new to you.
	vkCmdDispatch(computeCommandBuffer, (uint32)ceil((float)(COMPUTE_WIDTH) / WORKGROUP_SIZE), (uint32)ceil((float)(COMPUTE_HEIGHT) / WORKGROUP_SIZE), 1); //one workgroup every 32 int32

	Try(vkEndCommandBuffer(computeCommandBuffer)) Quit("Fatal error"); // end recording commands.
}


void Render::runCommandBuffer() {
	//Now we shall finally submit the recorded command buffer to a queue.
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1; // submit a single command buffer
	submitInfo.pCommandBuffers = &computeCommandBuffer; // the command buffer to submit.

	//We create a fence.
	VkFence fence;
	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = 0;
	Try(vkCreateFence(compute.LD, &fenceCreateInfo, NULL, &fence)) Quit("Fatal error");

	//We submit the command buffer on the queue, at the same time giving a fence.
	Try(vkQueueSubmit(compute.computeQueues[0], 1, &submitInfo, fence)) Quit("Fatal error");
	//The command will not have finished executing until the fence is signalled.
	//So we wait here.
	//We will directly after this read our buffer from the GPU,
	//and we will not be sure that the command has finished executing unless we wait for the fence.
	//Hence, we use a fence here.
	Try(vkWaitForFences(compute.LD, 1, &fence, VK_TRUE, 100000000000)) Quit("Fatal error");

	vkDestroyFence(compute.LD, fence, NULL);
}


void Render::cleanupCompute() {
	vkFreeMemory(compute.LD, bufferMemory, NULL);
	vkDestroyBuffer(compute.LD, buffer, NULL);
	vkDestroyShaderModule(compute.LD, computeShaderModule, NULL);
	vkDestroyDescriptorPool(compute.LD, computeDescriptorPool, NULL);
	vkDestroyDescriptorSetLayout(compute.LD, computeDescriptorSetLayout, NULL);
	vkDestroyPipelineLayout(compute.LD, computePipelineLayout, NULL);
	vkDestroyPipeline(compute.LD, computePipeline, NULL);
	vkDestroyCommandPool(compute.LD, computeCommandPool, NULL);
}
