
#include "LuxEngine/Engine/Engine.h"








// Shader components create functions -------------------------------------------------------------------------------------------------------//









void Engine::CShader_createDescriptorSetLayouts(const LuxArray<LuxCell>* pBufferIndices, const LuxShader vCShader) {
	//Specify a binding of type VK_DESCRIPTOR_TYPE_STORAGE_BUFFER to the binding point32 0
	//This binds to
	//  layout(std430, binding = 0) buffer buf
	//in the compute shader
	LuxArray<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings(pBufferIndices->size());
	forEach(*pBufferIndices, i) {
		VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};						//Create a descriptor set layout binding. The binding describes what to bind in a shader binding point and how to use it
		descriptorSetLayoutBinding.binding = sc<uint32>(i);										//Set the binding point in the shader
		descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;			//Set the type of the descriptor
		descriptorSetLayoutBinding.descriptorCount = 1;											//Set the number of descriptors
		descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;					//Use it in the compute stage
		descriptorSetLayoutBindings[i] = descriptorSetLayoutBinding;						//Save it in the layout binding array
	}

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};						//This structure contains all the descriptors of the bindings that will be used by the shader
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;	//Set structure type
	descriptorSetLayoutCreateInfo.bindingCount = sc<uint32>(descriptorSetLayoutBindings.size());			//Set number of binding points
	descriptorSetLayoutCreateInfo.pBindings = (new LuxArray<VkDescriptorSetLayoutBinding>(descriptorSetLayoutBindings))->data(); //Set descriptors to bind

	//Create the descriptor set layout
	TryVk(vkCreateDescriptorSetLayout(compute.LD, new VkDescriptorSetLayoutCreateInfo(descriptorSetLayoutCreateInfo), null, &CShaders[vCShader].descriptorSetLayout)) Exit("Unable to create descriptor set layout");
}




//Created the descriptor pool and allocates in it the descriptor sets
//*   pCells: an array of memory cells to bind to the shader
//*      The shader inputs must match those cells
//*      the binding index is the same as their index in the array
//*   vCShader: the shader where to create the descriptor pool and allocate the descriptor buffers
void Engine::CShader_createDescriptorSets(const LuxArray<LuxCell>* pCells, const LuxShader vCShader) {
	//Create descriptor pool and descriptor set allocate infos
		//This struct defines the size of a descriptor pool (how many descriptor sets it can contain)
		VkDescriptorPoolSize descriptorPoolSize = {};
		descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		descriptorPoolSize.descriptorCount = sc<uint32>(pCells->size());

		//This struct contains the informations about the descriptor pool. a descriptor pool contains the descriptor sets
		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};						//Create descriptor pool create infos
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;		//Set structure type
		descriptorPoolCreateInfo.maxSets = 1;												//Allocate only one descriptor set
		descriptorPoolCreateInfo.poolSizeCount = 1;											//One pool size
		descriptorPoolCreateInfo.pPoolSizes = &descriptorPoolSize;							//Set pool size
		descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;	//The descriptor sets can be freed
		//Create descriptor pool
		TryVk(vkCreateDescriptorPool(compute.LD, new VkDescriptorPoolCreateInfo(descriptorPoolCreateInfo), null, &CShaders[vCShader].descriptorPool)) Exit("Unable to create descriptor pool");

		//This structure contains the informations about the descriptor set
		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};						//Create descriptor set allocate infos
		descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;	//Set structure type
		descriptorSetAllocateInfo.descriptorPool = CShaders[vCShader].descriptorPool;		//Set descriptor pool where to allocate the descriptor
		descriptorSetAllocateInfo.descriptorSetCount = 1;									//Allocate a single descriptor
		descriptorSetAllocateInfo.pSetLayouts = &CShaders[vCShader].descriptorSetLayout;	//Set set layouts
		//Allocate descriptor set
		TryVk(vkAllocateDescriptorSets(compute.LD, new VkDescriptorSetAllocateInfo(descriptorSetAllocateInfo), &CShaders[vCShader].descriptorSet)) Exit("Unable to allocate descriptor sets");
	


	 //Create a descriptor set write for every buffer and update the descriptor sets
		LuxArray<VkWriteDescriptorSet> writeDescriptorSets(pCells->size());
		forEach(*pCells, i) {
			//Connect the storage buffer to the descrptor
			VkDescriptorBufferInfo descriptorBufferInfo = {};									//Create descriptor buffer infos
			descriptorBufferInfo.buffer = CBuffers[__lp_buffer_from_cc((*pCells)[i])].buffer;	//Set buffer
			descriptorBufferInfo.offset = (__lp_isShared_from_cc((*pCells)[i]) == 1) ? (__lp_cellIndex_from_cc((*pCells)[i]) * __lp_cellSize_from_cc((*pCells)[i])) : 0;													//Set offset
			descriptorBufferInfo.range = __lp_cellSize_from_cc((*pCells)[i]);					//Set size of the buffer

			VkWriteDescriptorSet writeDescriptorSet = {};										//Create write descriptor set
			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;						//Set structure type
			writeDescriptorSet.dstSet = CShaders[vCShader].descriptorSet;							//Set descriptor set
			writeDescriptorSet.dstBinding = sc<uint32>(i);											//Set binding
			writeDescriptorSet.descriptorCount = 1;													//Set number of descriptors
			writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;					//Use it as a storage
			writeDescriptorSet.pBufferInfo = new VkDescriptorBufferInfo(descriptorBufferInfo);		//Set descriptor buffer info

			writeDescriptorSets[i] = writeDescriptorSet;										//Save descriptor set
		}
		//Update descriptor sets
		vkUpdateDescriptorSets(compute.LD, writeDescriptorSets.size(), (new LuxArray(writeDescriptorSets))->data(), 0, null);
	
}




void Engine::CShader_createPipeline(const char* shaderPath, const LuxShader vCShader) {
	uint32 fileLength;																//Create the shader module
	VkShaderModule shaderModule = createShaderModule(compute.LD, readShaderFromFile(&fileLength, shaderPath), &fileLength);

	VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};						//Create shader stage infos
	shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;	//Set structure type
	shaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;							//Use it in the compute stage
	shaderStageCreateInfo.module = shaderModule;										//Set compute module
	shaderStageCreateInfo.pName = "main";												//Set the main function as entry point

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};						//Create pipeline create infos
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;		//Set structure type
	pipelineLayoutCreateInfo.setLayoutCount = 1;										//Set number of set layouts
	pipelineLayoutCreateInfo.pSetLayouts = &CShaders[vCShader].descriptorSetLayout;		//Set set layout
	//Create pipeline layout
	TryVk(vkCreatePipelineLayout(compute.LD, &pipelineLayoutCreateInfo, null, &CShaders[vCShader].pipelineLayout)) Exit("Unable to create pipeline layout");

	VkComputePipelineCreateInfo pipelineCreateInfo = {};							//Create pipeline create infos 
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;			//Set structure type
	pipelineCreateInfo.stage = shaderStageCreateInfo;									//Set shader stage infos
	pipelineCreateInfo.layout = CShaders[vCShader].pipelineLayout;						//Set pipeline layout
	//Create the compute pipeline
	TryVk(vkCreateComputePipelines(compute.LD, VK_NULL_HANDLE, 1, &pipelineCreateInfo, null, &CShaders[vCShader].pipeline)) Exit("Unable to create comput pipeline");
	vkDestroyShaderModule(compute.LD, shaderModule, null);							//Destroy the shader module
}




void Engine::CShader_createCommandBuffers(const LuxShader vCShader) {
	//Create command pool
	VkCommandPoolCreateInfo commandPoolCreateInfo = {};								//Create command pool create infos. The command pool contains the command buffers
	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;			//Set structure type
	commandPoolCreateInfo.flags = 0;													//Default falgs
	commandPoolCreateInfo.queueFamilyIndex = compute.PD.indices.computeFamilies[0];		//Set the compute family where to bind the command pool
	//Create the command pool
	TryVk(vkCreateCommandPool(compute.LD, &commandPoolCreateInfo, null, &CShaders[vCShader].commandPool)) Exit("Unable to create command pool");

	//Allocate command buffers
	VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};						//Create command buffer allocate infos to allocate the command buffer in the command pool
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;	//Set structure type
	commandBufferAllocateInfo.commandPool = CShaders[vCShader].commandPool;				//Set command pool where to allocate the command buffer 
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;					//Set the command buffer as a primary level command buffer
	commandBufferAllocateInfo.commandBufferCount = swapChainImages.size();				//Allocate one command buffer
	//Allocate command buffer
	TryVk(vkAllocateCommandBuffers(compute.LD, &commandBufferAllocateInfo, CShaders[vCShader].commandBuffers.data())) Exit("Unable to allocate command buffers");



	//Record command buffers
	for (int imgIndex = 0; imgIndex < swapChainImages.size(); imgIndex++) {			//For every command buffer of the swapchain images
		VkCommandBufferBeginInfo beginInfo = {};										//Create begin infos to start recording the command buffer
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;						//Set structure type
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;						//Set command buffer type
		//Start recording commands
		TryVk(vkBeginCommandBuffer(CShaders[vCShader].commandBuffers[imgIndex], &beginInfo)) Exit("Unable to begin command buffer recording");

		//Bind pipeline and descriptor sets to the command buffer
		vkCmdBindPipeline(CShaders[vCShader].commandBuffers[imgIndex], VK_PIPELINE_BIND_POINT_COMPUTE, CShaders[vCShader].pipeline);
		vkCmdBindDescriptorSets(CShaders[vCShader].commandBuffers[imgIndex], VK_PIPELINE_BIND_POINT_COMPUTE, CShaders[vCShader].pipelineLayout, 0, 1, &CShaders[vCShader].descriptorSet, 0, null);
		//Dispatch the compute shader to execute it with the specified workgroups
		vkCmdDispatch(CShaders[vCShader].commandBuffers[imgIndex], sc<uint32>(ceil(sc<float>(swapChainExtent.width) / WORKGROUP_SIZE)), sc<uint32>(ceil(sc<float>(swapChainExtent.height) / WORKGROUP_SIZE)), 1);




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

		vkCmdPipelineBarrier(CShaders[vCShader].commandBuffers[imgIndex], srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);


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

		vkCmdCopyBufferToImage(CShaders[vCShader].commandBuffers[imgIndex], CBuffers[__lp_buffer_from_cc(windowOutput)].buffer, swapChainImages[imgIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);



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

		vkCmdPipelineBarrier(CShaders[vCShader].commandBuffers[imgIndex], srcStage1, dstStage1, 0, 0, nullptr, 0, nullptr, 1, &barrier1);




		//End command buffer recording
		TryVk(vkEndCommandBuffer(CShaders[vCShader].commandBuffers[imgIndex])) Exit("Failed to record command buffer");
	}
}








// Create and destroy shaders ---------------------------------------------------------------------------------------------------------------//










//TODO check device limits
//*   pCells: an array of memory cells to bind to the shader
//*       The shader inputs must match those cells
//*       the binding index is the same as their index in the array
//*   vShaderPath: a pointer to a string containing the path to the compiled shader file
//*   returns the index of the created shader if the operation succeeded
//*       -1 if one or more buffers cannot be used
//*       -2 if the file does not exist
//*       -3 if an unknown error occurs //TODO
LuxShader Engine::CShader_new(LuxArray<LuxCell>* pCells, const char* vShaderPath) {
	//TODO check buffers
	//TODO check file

	//Add the shader to the shader array
	LuxShader shader = CShaders.add(LuxCShader{});
	//Create descriptor sets, descriptor buffer and descriptor layout
	CShader_createDescriptorSetLayouts(pCells, shader);
	CShader_createDescriptorSets(pCells, shader);
	//Create the compute pipeline
	CShader_createPipeline(vShaderPath, shader);
	//Create command buffers and command pool
	CShaders[shader].commandBuffers.resize(swapChainImages.size());
	CShader_createCommandBuffers(shader);

	//Return the index of the created shader
	return shader;
}





//TODO choose device
//Removes a shader from the shader array, cleaning all of its components and freeing the memory
//*   shader: the shader to destroy
//*   returns true if the operation succeeded, false if the index is not valid
bool Engine::CShader_destroy(const LuxShader vCShader){
	if (vCShader >= CShaders.size()) return false;

	//Clear descriptors sets, descriptor pool and descriptor layout
	vkFreeDescriptorSets(compute.LD, CShaders[vCShader].descriptorPool, 1, &CShaders[vCShader].descriptorSet);
	vkDestroyDescriptorPool(compute.LD, CShaders[vCShader].descriptorPool, null);
	vkDestroyDescriptorSetLayout(compute.LD, CShaders[vCShader].descriptorSetLayout, null);
	//Clear command buffers and command pool
	vkFreeCommandBuffers(compute.LD, CShaders[vCShader].commandPool, 1, CShaders[vCShader].commandBuffers.data());
	vkDestroyCommandPool(compute.LD, CShaders[vCShader].commandPool, null);
	//Clear compute pipeline and pipeline layout
	vkDestroyPipelineLayout(compute.LD, CShaders[vCShader].pipelineLayout, null);
	vkDestroyPipeline(compute.LD, CShaders[vCShader].pipeline, null);

	//Remove the shader from the shader array
	CShaders.remove(0);
	return true;
}
