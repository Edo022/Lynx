
#include "LuxEngine/Engine/Engine.h"








void Engine::CShader_create_descriptorSetLayouts(LuxArray<LuxFrag> bufferIndices, LuxShader CShader) {
	//Specify a binding of type VK_DESCRIPTOR_TYPE_STORAGE_BUFFER to the binding point32 0
	//This binds to
	//  layout(std430, binding = 0) buffer buf
	//in the compute shader
	LuxArray<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings(bufferIndices.size());
	forEach(bufferIndices, i) {
		VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};						//Create a descriptor set layout binding. The binding describes what to bind in a shader's binding point and how to use it
		descriptorSetLayoutBinding.binding = i;													//Set the binding point in the shader
		descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;			//Set the type of the descriptor
		descriptorSetLayoutBinding.descriptorCount = 1;											//Set the number of descriptors
		descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;					//Use it in the compute stage
		descriptorSetLayoutBindings[i] = descriptorSetLayoutBinding;						//Save it in the layout binding array
	}

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};						//This structure contains all the descriptors of the bindings that will be used by the shader
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;	//Set structure type
	descriptorSetLayoutCreateInfo.bindingCount = descriptorSetLayoutBindings.size();			//Set number of binding points
	descriptorSetLayoutCreateInfo.pBindings = (new LuxArray<VkDescriptorSetLayoutBinding>(descriptorSetLayoutBindings))->data(); //Set descriptors to bind

	//Create the descriptor set layout
	Try(vkCreateDescriptorSetLayout(compute.LD, new VkDescriptorSetLayoutCreateInfo(descriptorSetLayoutCreateInfo), null, &CShaders[CShader].descriptorSetLayout)) Quit("Fatal error");
}





void Engine::CShader_create_descriptorSets(LuxArray<LuxFrag> bufferIndices, LuxShader CShader) {
	{ //Create descriptor pool and descriptor set allocate infos
		//This struct defines the size of a descriptor pool (how many descriptor sets it can contain)
		VkDescriptorPoolSize descriptorPoolSize = {};
		descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		descriptorPoolSize.descriptorCount = bufferIndices.size();

		//This struct contains the informations about the descriptor pool. a descriptor pool contains the descriptor sets
		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};						//Create descriptor pool create infos
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;		//Set structure type
		descriptorPoolCreateInfo.maxSets = 1;												//Allocate only one descriptor set
		descriptorPoolCreateInfo.poolSizeCount = 1;											//One pool size
		descriptorPoolCreateInfo.pPoolSizes = &descriptorPoolSize;							//Set pool size
		//Create descriptor pool
		Try(vkCreateDescriptorPool(compute.LD, new VkDescriptorPoolCreateInfo(descriptorPoolCreateInfo), null, &CShaders[CShader].descriptorPool)) Quit("Fatal error");

		//This structure contains the informations about the descriptor set
		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};						//Create descriptor set allocate infos
		descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;	//Set structure type
		descriptorSetAllocateInfo.descriptorPool =CShaders[CShader].descriptorPool;	//Set descriptor pool where to allocate the descriptor
		descriptorSetAllocateInfo.descriptorSetCount = 1;									//Allocate a single descriptor
		descriptorSetAllocateInfo.pSetLayouts = &CShaders[CShader].descriptorSetLayout;//Set set layouts
		//Allocate descriptor set
		Try(vkAllocateDescriptorSets(compute.LD, new VkDescriptorSetAllocateInfo(descriptorSetAllocateInfo), &CShaders[CShader].descriptorSet)) Quit("Fatal error");
	}


	{ //Create a descriptor set write for every buffer and update the descriptor sets
		LuxArray<VkWriteDescriptorSet> writeDescriptorSets(bufferIndices.size());
		forEach(bufferIndices, i) {
			//Connect the storage buffer to the descrptor
			VkDescriptorBufferInfo descriptorBufferInfo = {};								//Create descriptor buffer infos
			descriptorBufferInfo.buffer = CGpuBuffers[i].buffer;								//Set buffer
			descriptorBufferInfo.offset = 0;													//Set offset
			descriptorBufferInfo.range = CGpuBuffers[i].size;									//Set size of the buffer

			VkWriteDescriptorSet writeDescriptorSet = {};									//Create write descriptor set
			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;					//Set structure type
			writeDescriptorSet.dstSet =CShaders[CShader].descriptorSet;							//Set descriptor set
			writeDescriptorSet.dstBinding = i;													//Set binding
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
	CShaders[CShader].shaderModule = createShaderModule(compute.LD, readShaderFromFile(&fileLength, shaderPath), &fileLength);


	VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};						//Create shader stage infos
	shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;	//Set structure type
	shaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;							//Use it in the compute stage
	shaderStageCreateInfo.module = CShaders[CShader].shaderModule;						//Set compute module
	shaderStageCreateInfo.pName = "main";												//Set the main function as entry point


	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};						//Create pipeline create infos
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;		//Set structure type
	pipelineLayoutCreateInfo.setLayoutCount = 1;										//Set number of set layouts
	pipelineLayoutCreateInfo.pSetLayouts = &CShaders[CShader].descriptorSetLayout;	//Set set layout
	//Create pipeline layout
	Try(vkCreatePipelineLayout(compute.LD, &pipelineLayoutCreateInfo, null, &CShaders[CShader].pipelineLayout)) Quit("Fatal error");


	VkComputePipelineCreateInfo pipelineCreateInfo = {};							//Create pipeline create infos 
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;			//Set structure type
	pipelineCreateInfo.stage = shaderStageCreateInfo;									//Set shader stage infos
	pipelineCreateInfo.layout = CShaders[CShader].pipelineLayout;						//Set pipeline layout
	//Create the compute pipeline
	Try(vkCreateComputePipelines(compute.LD, VK_NULL_HANDLE, 1, &pipelineCreateInfo, null, &CShaders[CShader].pipeline)) Quit("Fatal error");

	vkDestroyShaderModule(compute.LD, CShaders[CShader].shaderModule, null);				//Destroy the shader module
}
