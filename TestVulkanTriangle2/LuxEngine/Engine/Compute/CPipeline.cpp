
#include "LuxEngine/Engine/Engine.h"







void Engine::CShader_create_descriptorSetLayouts(LuxArray<LuxCell> bufferIndices, LuxShader CShader) {
	//Specify a binding of type VK_DESCRIPTOR_TYPE_STORAGE_BUFFER to the binding point32 0
	//This binds to
	//  layout(std430, binding = 0) buffer buf
	//in the compute shader
	LuxArray<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings(bufferIndices.size());
	forEach(bufferIndices, i) {
		VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};						//Create a descriptor set layout binding. The binding describes what to bind in a shader's binding point and how to use it
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
	TryVk(vkCreateDescriptorSetLayout(compute.LD, new VkDescriptorSetLayoutCreateInfo(descriptorSetLayoutCreateInfo), null, &CShaders[CShader].descriptorSetLayout)) Exit("Unable to create descriptor set layout");
}





void Engine::CShader_create_descriptorSets(LuxArray<LuxCell> bufferIndices, LuxShader CShader) {
	{ //Create descriptor pool and descriptor set allocate infos
		//This struct defines the size of a descriptor pool (how many descriptor sets it can contain)
		VkDescriptorPoolSize descriptorPoolSize = {};
		descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		descriptorPoolSize.descriptorCount = sc<uint32>(bufferIndices.size());

		//This struct contains the informations about the descriptor pool. a descriptor pool contains the descriptor sets
		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};						//Create descriptor pool create infos
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;		//Set structure type
		descriptorPoolCreateInfo.maxSets = 1;												//Allocate only one descriptor set
		descriptorPoolCreateInfo.poolSizeCount = 1;											//One pool size
		descriptorPoolCreateInfo.pPoolSizes = &descriptorPoolSize;							//Set pool size
		descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;	//The descriptor sets can be freed
		//Create descriptor pool
		TryVk(vkCreateDescriptorPool(compute.LD, new VkDescriptorPoolCreateInfo(descriptorPoolCreateInfo), null, &CShaders[CShader].descriptorPool)) Exit("Unable to create descriptor pool");

		//This structure contains the informations about the descriptor set
		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};						//Create descriptor set allocate infos
		descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;	//Set structure type
		descriptorSetAllocateInfo.descriptorPool = CShaders[CShader].descriptorPool;			//Set descriptor pool where to allocate the descriptor
		descriptorSetAllocateInfo.descriptorSetCount = 1;									//Allocate a single descriptor
		descriptorSetAllocateInfo.pSetLayouts = &CShaders[CShader].descriptorSetLayout;		//Set set layouts
		//Allocate descriptor set
		TryVk(vkAllocateDescriptorSets(compute.LD, new VkDescriptorSetAllocateInfo(descriptorSetAllocateInfo), &CShaders[CShader].descriptorSet)) Exit("Unable to allocate descriptor sets");
	}


	{ //Create a descriptor set write for every buffer and update the descriptor sets
		LuxArray<VkWriteDescriptorSet> writeDescriptorSets(bufferIndices.size());
		forEach(bufferIndices, i) {
			//Connect the storage buffer to the descrptor
			VkDescriptorBufferInfo descriptorBufferInfo = {};								//Create descriptor buffer infos
			descriptorBufferInfo.buffer = CBuffers[__lp_buffer_from_cc(bufferIndices[i])].buffer;//Set buffer
			descriptorBufferInfo.offset = (__lp_isShared_from_cc(bufferIndices[i]) == 1) ? (__lp_cellIndex_from_cc(bufferIndices[i]) * __lp_cellSize_from_cc(bufferIndices[i])) : 0;													//Set offset
			descriptorBufferInfo.range = __lp_cellSize_from_cc(bufferIndices[i]);										//Set size of the buffer

			VkWriteDescriptorSet writeDescriptorSet = {};									//Create write descriptor set
			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;					//Set structure type
			writeDescriptorSet.dstSet = CShaders[CShader].descriptorSet;						//Set descriptor set
			writeDescriptorSet.dstBinding = sc<uint32>(i);										//Set binding
			writeDescriptorSet.descriptorCount = 1;												//Set number of descriptors
			writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;				//Use it as a storage
			writeDescriptorSet.pBufferInfo = new VkDescriptorBufferInfo(descriptorBufferInfo);	//Set descriptor buffer info

			writeDescriptorSets[i] = writeDescriptorSet;									//Save descriptor set
		}
		//Update descriptor sets
		vkUpdateDescriptorSets(compute.LD, writeDescriptorSets.size(), (new LuxArray(writeDescriptorSets))->data(), 0, null);
	}
}








void Engine::CShader_create_CPipeline(const char* shaderPath, LuxShader CShader) {
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
	pipelineLayoutCreateInfo.pSetLayouts = &CShaders[CShader].descriptorSetLayout;		//Set set layout
	//Create pipeline layout
	TryVk(vkCreatePipelineLayout(compute.LD, &pipelineLayoutCreateInfo, null, &CShaders[CShader].pipelineLayout)) Exit("Unable to create pipeline layout");


	VkComputePipelineCreateInfo pipelineCreateInfo = {};							//Create pipeline create infos 
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;			//Set structure type
	pipelineCreateInfo.stage = shaderStageCreateInfo;									//Set shader stage infos
	pipelineCreateInfo.layout = CShaders[CShader].pipelineLayout;						//Set pipeline layout
	//Create the compute pipeline
	TryVk(vkCreateComputePipelines(compute.LD, VK_NULL_HANDLE, 1, &pipelineCreateInfo, null, &CShaders[CShader].pipeline)) Exit("Unable to create comput pipeline");
	vkDestroyShaderModule(compute.LD, shaderModule, null);							//Destroy the shader module
}
