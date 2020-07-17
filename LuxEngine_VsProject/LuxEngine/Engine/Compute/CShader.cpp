

#include "LuxEngine/Engine/Engine.h"
#include "LuxEngine/Engine/Compute/CShader_t.h"








// Shader components create functions -------------------------------------------------------------------------------------------------------//








	void Engine::cshaderCreateDescriptorSetLayouts(const lux::Array<LuxCell>& pCells, LuxShader_t* pCShader) {
		//Specify a binding of type VK_DESCRIPTOR_TYPE_STORAGE_BUFFER to the binding point32 0
		//This binds to
		//  layout(std430, binding = 0) buffer buf
		//in the compute shader
		lux::Array<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings(pCells.size( ));
		for(uint32 i = 0; i < pCells.size( ); ++i) {
			VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};						//Create a descriptor set layout binding. The binding describes what to bind in a shader binding point and how to use it
			descriptorSetLayoutBinding.binding = i;													//Set the binding point in the shader
			descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;			//Set the type of the descriptor
			descriptorSetLayoutBinding.descriptorCount = 1;											//Set the number of descriptors
			descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;					//Use it in the compute stage
			descriptorSetLayoutBinding.pImmutableSamplers = null;									//Default
			descriptorSetLayoutBindings[i] = descriptorSetLayoutBinding;						//Save it in the layout binding array
		}

		//uint32* hhhhh = (uint32*)(gpuCellMap((*pCells)[0]));
		//int hh__ = hhhhh[0];

		VkDescriptorSetLayoutCreateInfo* descriptorSetLayoutCreateInfo = (VkDescriptorSetLayoutCreateInfo*)malloc(sizeof(VkDescriptorSetLayoutCreateInfo));//This structure contains all the descriptors of the bindings that will be used by the shader
		descriptorSetLayoutCreateInfo->flags = 0;														//default
		descriptorSetLayoutCreateInfo->pNext = null;													//default
		descriptorSetLayoutCreateInfo->sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;		//Set structure type
		descriptorSetLayoutCreateInfo->bindingCount = descriptorSetLayoutBindings.size( );				//Set number of binding points
		descriptorSetLayoutCreateInfo->pBindings = (descriptorSetLayoutBindings.begin( ));				//Set descriptors layouts to bind

		//Create the descriptor set layout
		TryVk(vkCreateDescriptorSetLayout(compute.LD, descriptorSetLayoutCreateInfo, null, &pCShader->descriptorSetLayout)) Exit("Unable to create descriptor set layout");
		pCShader->__lp_ptrs.add((void*)descriptorSetLayoutCreateInfo);
	}








	//Created the descriptor pool and allocates in it the descriptor sets
	//*   pCells: an array of memory cells to bind to the shader
	//*      The shader inputs must match those cells
	//*      the binding index is the same as their index in the array
	//*   vCShader: the shader where to create the descriptor pool and allocate the descriptor buffers
	void Engine::cshaderCreateDescriptorSets(const lux::Array<LuxCell>& pCells, LuxShader_t* pCShader) {
		//Create descriptor pool and descriptor set allocate infos
			//This struct defines the size of a descriptor pool (how many descriptor sets it can contain)
		VkDescriptorPoolSize descriptorPoolSize = {};
		descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		descriptorPoolSize.descriptorCount = pCells.size( );

		//This struct contains the informations about the descriptor pool. a descriptor pool contains the descriptor sets
		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};						//Create descriptor pool create infos
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;		//Set structure type
		descriptorPoolCreateInfo.maxSets = 1;												//Allocate only one descriptor set
		descriptorPoolCreateInfo.poolSizeCount = 1;											//One pool size
		descriptorPoolCreateInfo.pPoolSizes = &descriptorPoolSize;							//Set pool size
		descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;	//The descriptor sets can be freed
		//Create descriptor pool
		TryVk(vkCreateDescriptorPool(compute.LD, &descriptorPoolCreateInfo, null, &pCShader->descriptorPool)) Exit("Unable to create descriptor pool");

		//This structure contains the informations about the descriptor set
		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};						//Create descriptor set allocate infos
		descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;	//Set structure type
		descriptorSetAllocateInfo.descriptorPool = pCShader->descriptorPool;		//Set descriptor pool where to allocate the descriptor
		descriptorSetAllocateInfo.descriptorSetCount = 1;									//Allocate a single descriptor
		descriptorSetAllocateInfo.pSetLayouts = &pCShader->descriptorSetLayout;	//Set set layouts
		//Allocate descriptor set
		TryVk(vkAllocateDescriptorSets(compute.LD, &descriptorSetAllocateInfo, &pCShader->descriptorSet)) Exit("Unable to allocate descriptor sets");



		//Create a descriptor set write for every buffer and update the descriptor sets
		lux::Array<VkWriteDescriptorSet> writeDescriptorSets(pCells.size( ));
		for(uint32 i = 0; i < pCells.size( ); ++i) {
			//Connect the storage buffer to the descrptor
			VkDescriptorBufferInfo* descriptorBufferInfo = (VkDescriptorBufferInfo*)malloc(sizeof(VkDescriptorBufferInfo));	//Create descriptor buffer infos
			descriptorBufferInfo->buffer = CBuffers[__lp_buffer_from_cc(pCells[i])].buffer;		//Set buffer
			descriptorBufferInfo->offset = __lp_cellOffset_from_cc(&compute.PD, pCells[i]);		//Set buffer offset
			descriptorBufferInfo->range = __lp_cellSize_from_cc(pCells[i]);						//Set buffer size

			VkWriteDescriptorSet writeDescriptorSet = {};										//Create write descriptor set
			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;						//Set structure type
			writeDescriptorSet.dstSet = pCShader->descriptorSet;							//Set descriptor set
			writeDescriptorSet.dstBinding = i;														//Set binding
			writeDescriptorSet.descriptorCount = 1;													//Set number of descriptors
			writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;					//Use it as a storage
			writeDescriptorSet.pBufferInfo = descriptorBufferInfo;									//Set descriptor buffer info

			writeDescriptorSets[i] = writeDescriptorSet;										//Save descriptor set
			pCShader->__lp_ptrs.add((void*)descriptorBufferInfo);						//Save the struct in the pointers that needs to be freed
		}
		//Update descriptor sets
		vkUpdateDescriptorSets(compute.LD, writeDescriptorSets.size( ), writeDescriptorSets.begin( ), 0, null);
	}








	void Engine::cshaderCreatePipeline(const char* shaderPath, LuxShader_t* pCShader) {
		uint32 fileLength;																//Create the shader module
		VkShaderModule shaderModule = cshaderCreateModule(compute.LD, cshaderReadFromFile(&fileLength, shaderPath), &fileLength);

		VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};						//Create shader stage infos
		shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;	//Set structure type
		shaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;							//Use it in the compute stage
		shaderStageCreateInfo.module = shaderModule;										//Set compute module
		shaderStageCreateInfo.pName = "main";												//Set the main function as entry point

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};						//Create pipeline create infos
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;		//Set structure type
		pipelineLayoutCreateInfo.setLayoutCount = 1;										//Set number of set layouts
		pipelineLayoutCreateInfo.pSetLayouts = &pCShader->descriptorSetLayout;		//Set set layout
		//Create pipeline layout
		TryVk(vkCreatePipelineLayout(compute.LD, &pipelineLayoutCreateInfo, null, &pCShader->pipelineLayout)) Exit("Unable to create pipeline layout");

		VkComputePipelineCreateInfo pipelineCreateInfo = {};							//Create pipeline create infos
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;			//Set structure type
		pipelineCreateInfo.stage = shaderStageCreateInfo;									//Set shader stage infos
		pipelineCreateInfo.layout = pCShader->pipelineLayout;						//Set pipeline layout
		//Create the compute pipeline
		TryVk(vkCreateComputePipelines(compute.LD, VK_NULL_HANDLE, 1, &pipelineCreateInfo, null, &pCShader->pipeline)) Exit("Unable to create comput pipeline");
		vkDestroyShaderModule(compute.LD, shaderModule, null);							//Destroy the shader module
	}








	void Engine::cshaderCreateDefaultCommandBuffers( ) {
		{ //Copy
			//Create command pool
			static VkCommandPoolCreateInfo commandPoolCreateInfo = {};							//Create command pool create infos. The command pool contains the command buffers
			commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;				//Set structure type
			commandPoolCreateInfo.flags = 0;														//Default falgs
			commandPoolCreateInfo.queueFamilyIndex = compute.PD.indices.computeFamilies[0];			//Set the compute family where to bind the command pool
			TryVk(vkCreateCommandPool(compute.LD, &commandPoolCreateInfo, null, &copyCommandPool)) Exit("Unable to create command pool");

			//Allocate command buffers
			static VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};					//Create command buffer allocate infos to allocate the command buffer in the command pool
			commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;		//Set structure type
			commandBufferAllocateInfo.commandPool = copyCommandPool;								//Set command pool where to allocate the command buffer
			commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;						//Set the command buffer as a primary level command buffer
			commandBufferAllocateInfo.commandBufferCount = swapchainImages.size( );					//Allocate one command buffer for each swapchain image
			TryVk(vkAllocateCommandBuffers(compute.LD, &commandBufferAllocateInfo, copyCommandBuffers.begin( ))) Exit("Unable to allocate command buffers");



			//Record present command buffers
			for(uint32 imgIndex = 0; imgIndex < swapchainImages.size( ); imgIndex++) {	//For every command buffer of the swapchain images
				//Start recording commands
				VkCommandBufferBeginInfo beginInfo = {};								//Create begin infos to start recording the command buffer
				beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;				//Set structure type
				beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;				//Set command buffer type. Simultaneous use allows the command buffer to be executed multiple times
				TryVk(vkBeginCommandBuffer(copyCommandBuffers[imgIndex], &beginInfo)) Exit("Unable to begin command buffer recording");


				//Create a barrier to use the swapchain image as a transfer destination optimal to copy the buffer in it
				VkImageMemoryBarrier readToWrite{};										//Create memory barrier object
				readToWrite.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;					//Set structure type
				readToWrite.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;							//Set old layout. Swapchain images are in undefined layout after being acquired
				readToWrite.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;				//Set new layout. Destination optimal allows the image to be used as a transfer destination
				readToWrite.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;					//Queue families unset
				readToWrite.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;					//Queue families unset
				readToWrite.image = swapchainImages[imgIndex];								//Set swapchain image
				readToWrite.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;		//Set the aspect mask
				readToWrite.subresourceRange.baseMipLevel = 0;								//No mipmap
				readToWrite.subresourceRange.levelCount = 1;								//No multi leve images
				readToWrite.subresourceRange.baseArrayLayer = 0;							//Set base layer
				readToWrite.subresourceRange.layerCount = 1;								//No multi layer
				readToWrite.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;						//Set source access mask
				readToWrite.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;					//Set destination access mask. It must be writable in order to copy the buffer in it
				VkPipelineStageFlags srcStage, dstStage;								//Create stage flags
				srcStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;					//The swapchain image is in color output stage
				dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;									//Change it to transfer stage to copy the buffer in it
				vkCmdPipelineBarrier(copyCommandBuffers[imgIndex], srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &readToWrite);

				VkBufferImageCopy region{};												//Create bufferImageCopy region to copy the buffer to the image
				region.bufferOffset = 0;													//No buffer offset
				region.bufferRowLength = 0;													//dark magic
				region.bufferImageHeight = 0;												//dark magic
				region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;				//Set aspect mask
				region.imageSubresource.mipLevel = 0;										//No mipmap
				region.imageSubresource.baseArrayLayer = 0;									//Set base layer
				region.imageSubresource.layerCount = 1;										//No multi layer
				region.imageOffset = { 0, 0, 0 };											//No image offset
				region.imageExtent = { swapchainExtent.width, swapchainExtent.height, 1 };	//Copy the whole buffer
				vkCmdCopyBufferToImage(copyCommandBuffers[imgIndex], CBuffers[__lp_buffer_from_cc(gpuCellWindowOutput)].buffer, swapchainImages[imgIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

				//Create a barrier to use the swapchain image as a present source image
				VkImageMemoryBarrier writeToRead{};										//Create memory barrier object
				writeToRead.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;					//Set structure type
				writeToRead.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;				//Set old layout. Swapchain images is in dst optimal layout after being written
				writeToRead.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;					//Set new layout. Swapchain images must be in this format to be displayed on screen
				writeToRead.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;					//Queue families unset
				writeToRead.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;					//Queue families unset
				writeToRead.image = swapchainImages[imgIndex];								//Set swapchain image
				writeToRead.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;		//Set the aspect mask
				writeToRead.subresourceRange.baseMipLevel = 0;								//No mipmap
				writeToRead.subresourceRange.levelCount = 1;								//No multi leve images
				writeToRead.subresourceRange.baseArrayLayer = 0;							//Set base layer
				writeToRead.subresourceRange.layerCount = 1;								//No multi layer
				writeToRead.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;					//Set source access mask
				writeToRead.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;						//Set destination access mask. It must be readable to be displayed
				VkPipelineStageFlags srcStage1, dstStage1;								//Create stage flags
				srcStage1 = VK_PIPELINE_STAGE_TRANSFER_BIT;									//The image is in transfer stage from the buffer copy
				dstStage1 = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;					//Change it to color output to present them
				vkCmdPipelineBarrier(copyCommandBuffers[imgIndex], srcStage1, dstStage1, 0, 0, nullptr, 0, nullptr, 1, &writeToRead);

				//End command buffer recording
				TryVk(vkEndCommandBuffer(copyCommandBuffers[imgIndex])) Exit("Failed to record command buffer");
			}
		}




		{ //Clear
			//Create command pool
			static VkCommandPoolCreateInfo commandPoolCreateInfo2 = {};							//Create command pool create infos. The command pool contains the command buffers
			commandPoolCreateInfo2.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;				//Set structure type
			commandPoolCreateInfo2.flags = 0;														//Default falgs
			commandPoolCreateInfo2.queueFamilyIndex = compute.PD.indices.computeFamilies[0];			//Set the compute family where to bind the command pool
			TryVk(vkCreateCommandPool(compute.LD, &commandPoolCreateInfo2, null, &clearCommandPool)) Exit("Unable to create command pool");


			//Create allocate info
			VkCommandBufferAllocateInfo allocInfo2{};
			allocInfo2.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo2.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo2.commandPool = clearCommandPool;
			allocInfo2.commandBufferCount = 1;

			//Allocate command buffer
			vkAllocateCommandBuffers(graphics.LD, &allocInfo2, &clearCommandBuffer);

			//Begine command recording
			VkCommandBufferBeginInfo beginInfo2{};
			beginInfo2.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo2.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
			vkBeginCommandBuffer(clearCommandBuffer, &beginInfo2);

			vkCmdFillBuffer(clearCommandBuffer, CBuffers[__lp_buffer_from_cc(gpuCellWindowOutput)].buffer, 0, swapchainExtent.width * swapchainExtent.height * 4, 0);

			//End command recording
			vkEndCommandBuffer(clearCommandBuffer);
		}
	}








	void Engine::cshaderCreateCommandBuffers(LuxShader_t* pCShader, const uint32 vGroupCountX, const uint32 vGroupCounty, const uint32 vGroupCountz) {
		//Create command pool
		VkCommandPoolCreateInfo commandPoolCreateInfo = {};									//Create command pool create infos. The command pool contains the command buffers
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;				//Set structure type
		commandPoolCreateInfo.flags = 0;														//Default falgs
		commandPoolCreateInfo.queueFamilyIndex = compute.PD.indices.computeFamilies[0];			//Set the compute family where to bind the command pool
		TryVk(vkCreateCommandPool(compute.LD, &commandPoolCreateInfo, null, &pCShader->commandPool)) Exit("Unable to create command pool");

		//Allocate command buffers
		VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};							//Create command buffer allocate infos to allocate the command buffer in the command pool
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;		//Set structure type
		commandBufferAllocateInfo.commandPool = pCShader->commandPool;					//Set command pool where to allocate the command buffer
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;						//Set the command buffer as a primary level command buffer
		commandBufferAllocateInfo.commandBufferCount = 1;										//Allocate one command buffer
		pCShader->commandBuffers.resize(1);
		TryVk(vkAllocateCommandBuffers(compute.LD, &commandBufferAllocateInfo, pCShader->commandBuffers.__lp_data)) Exit("Unable to allocate command buffers");




		//Create compute command buffer and start recording commands
		VkCommandBufferBeginInfo beginInfo = {};											//Create begin infos to start recording the command buffer
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;							//Set structure type
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;							//Set command buffer type. Simultaneous use allows the command buffer to be executed multiple times
		TryVk(vkBeginCommandBuffer(pCShader->commandBuffers[0], &beginInfo)) Exit("Unable to begin command buffer recording");

		//Bind pipeline and descriptor sets to the command buffer
		vkCmdBindPipeline(pCShader->commandBuffers[0], VK_PIPELINE_BIND_POINT_COMPUTE, pCShader->pipeline);
		vkCmdBindDescriptorSets(pCShader->commandBuffers[0], VK_PIPELINE_BIND_POINT_COMPUTE, pCShader->pipelineLayout, 0, 1, &pCShader->descriptorSet, 0, null);


		//Dispatch the compute shader to execute it with the specified workgroups and descriptors
		//TODO fix
		vkCmdDispatch(pCShader->commandBuffers[0], vGroupCountX, vGroupCounty, vGroupCountz);

		//End command buffer recording
		TryVk(vkEndCommandBuffer(pCShader->commandBuffers[0])) Exit("Failed to record command buffer");
	}








	// Create and destroy shaders ---------------------------------------------------------------------------------------------------------------//








	//Add a shader in the staging queue. The shader will be created and used by the rendering thread
	//All the shaders created with this function must be destroyed with CShaderDestroy(shader)
	//*   pCells: an array of memory cells to bind to the shader
	//*       The shader inputs must match those cells
	//*       the binding index is the same as their index in the array
	//*   vShaderPath: a pointer to a lux::String containing the path to the compiled shader file
	//*   returns 0 if the shader was created and loaded successfully
	//*       -1 if one or more buffers cannot be used
	//*       -2 if the file does not exist
	//*       -3 if an unknown error occurs //TODO
	int32 Engine::cshaderNew(const lux::Array<LuxCell>& pCells, const char* vShaderPath, const uint32 vGroupCountX, const uint32 vGroupCounty, const uint32 vGroupCountz) {
		//TODO check buffers
		//TODO check file
		CShaders_stg_t* shaderStgPtr = new CShaders_stg_t{ LuxShader_t{ }, pCells, vShaderPath };

		cshaderCreateDescriptorSetLayouts(shaderStgPtr->cells, &shaderStgPtr->shader);					//Create descriptor layouts,
		cshaderCreateDescriptorSets(shaderStgPtr->cells, &shaderStgPtr->shader);						//Descriptor pool, descriptor sets and descriptor buffers
		cshaderCreatePipeline(shaderStgPtr->shaderPath, &shaderStgPtr->shader);							//Create the compute pipeline
		cshaderCreateCommandBuffers(&shaderStgPtr->shader, vGroupCountX, vGroupCounty, vGroupCountz);	//Create command buffers and command pool

		CShaders_stg.pushFront(shaderStgPtr);															//Add the shader to the shader array

		return 0;
	}







	//TODO choose device
	//Removes a shader from the shader array, cleaning all of its components and freeing the memory
	//*   shader: the shader to destroy
	//*   returns true if the operation succeeded, false if the index is not valid
	bool Engine::cshaderDestroy(const LuxShader vCShader) {
		if(vCShader >= CShaders.size( )) return false;

		//Clear descriptors sets, descriptor pool and descriptor layout
		vkFreeDescriptorSets(compute.LD, CShaders[vCShader].descriptorPool, 1, &CShaders[vCShader].descriptorSet);
		vkDestroyDescriptorPool(compute.LD, CShaders[vCShader].descriptorPool, null);
		vkDestroyDescriptorSetLayout(compute.LD, CShaders[vCShader].descriptorSetLayout, null);

		//Clear command buffers and command pool
		vkFreeCommandBuffers(compute.LD, CShaders[vCShader].commandPool, 1, CShaders[vCShader].commandBuffers.begin( ));
		vkDestroyCommandPool(compute.LD, CShaders[vCShader].commandPool, null);

		//Clear compute pipeline and pipeline layout
		vkDestroyPipelineLayout(compute.LD, CShaders[vCShader].pipelineLayout, null);
		vkDestroyPipeline(compute.LD, CShaders[vCShader].pipeline, null);

		//Free all the useless pointers
		for(uint32 i = 0; i < CShaders[vCShader].__lp_ptrs.size( ); ++i) free(CShaders[vCShader].__lp_ptrs[i]);

		//Remove the shader from the shader array
		CShaders.remove(vCShader);
		return true;
	}
