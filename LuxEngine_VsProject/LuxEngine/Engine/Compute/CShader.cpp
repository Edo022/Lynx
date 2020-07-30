

#include "LuxEngine/Engine/Engine.h"
#include "LuxEngine/Engine/Compute/CShader_t.h"








// Shader components create functions -------------------------------------------------------------------------------------------------------//








//TODO remove shader path
//TODO use automatic default shader assignment
//This function creates the descriptor sets layout, the pipeline and the pipeline layout of a shader
//*   vRenderShader | the type of the shader
//*   pCellNum      | The number of cells to bing to the shader. The shader inputs must match those cells
void Engine::cshaderCreateDefLayout(const defRenderShader vRenderShader, const uint32 pCellNum) {
	{ //Create descriptor set layout
		lux::Array<VkDescriptorSetLayoutBinding> bindingLayouts(pCellNum);
		for(uint32 i = 0; i < pCellNum; ++i) {										//Create a binding layout for every cell
			VkDescriptorSetLayoutBinding bindingLayout = { };							//The binding layout describes what to bind in a shader binding point and how to use it
			bindingLayout.binding = i;														//Binding point in the shader
			bindingLayout.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;				//Type of the descriptor. It depends on the type of data that needs to be bound
			bindingLayout.descriptorCount = 1;												//Number of descriptors
			bindingLayout.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;							//Stage where to use the layout
			bindingLayout.pImmutableSamplers = nullptr;										//Default
			bindingLayouts[i] = bindingLayout;											//Save it in the layout binding array
		}

		//Create a VkDescriptorSetLayoutCreateInfo structure. It contains all the bindings layouts and it's used to create the the VkDescriptorSetLayout
		VkDescriptorSetLayoutCreateInfo* layoutCreateInfo = (VkDescriptorSetLayoutCreateInfo*)malloc(sizeof(VkDescriptorSetLayoutCreateInfo));
		layoutCreateInfo->sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;	//Structure type
		layoutCreateInfo->bindingCount = bindingLayouts.size( );						//Number of binding layouts
		layoutCreateInfo->pBindings = (bindingLayouts.begin( ));						//The binding layouts
		layoutCreateInfo->flags = 0;													//default
		layoutCreateInfo->pNext = nullptr;												//default

		//Create the descriptor set layout
		TryVk(vkCreateDescriptorSetLayout(compute.LD, layoutCreateInfo, nullptr, &CShadersLayouts[vRenderShader].descriptorSetLayout)) Exit("Unable to create descriptor set layout");
		CShadersLayouts[vRenderShader].__lp_ptrs.add((void*)layoutCreateInfo);
	}




	{ //Create pipeline layout
		lux::String shaderFileName;
		switch(vRenderShader) {																	//Set shader file name
			case LUX_DEF_SHADER_LINE_2D: shaderFileName = "Line2D"; break;
			case LUX_DEF_SHADER_COPY: shaderFileName = "FloatToIntBuffer"; break;
			default: break; //TODO add unknown shader check
		}
		uint32 fileLength;																		//Create the shader module
		CShadersLayouts[vRenderShader].shaderModule = cshaderCreateModule(compute.LD, cshaderReadFromFile(&fileLength, (shaderPath + shaderFileName + ".comp.spv").begin( )), &fileLength);

		VkPipelineShaderStageCreateInfo shaderStageCreateInfo = { };							//Create shader stage infos
		shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;			//Set structure type
		shaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;									//Use it in the compute stage
		shaderStageCreateInfo.module = CShadersLayouts[vRenderShader].shaderModule;					//Set shader module
		shaderStageCreateInfo.pName = "main";														//Set the main function as entry point
		CShadersLayouts[vRenderShader].shaderStageCreateInfo = shaderStageCreateInfo;			//Save the shader stage infos in the default shader object

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = { };								//Create pipeline layout creation infos
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;				//Structure type
		pipelineLayoutCreateInfo.setLayoutCount = 1;												//Number of set layouts
		pipelineLayoutCreateInfo.pSetLayouts = &CShadersLayouts[vRenderShader].descriptorSetLayout;//Set set layout
		//Create pipeline layout
		TryVk(vkCreatePipelineLayout(compute.LD, &pipelineLayoutCreateInfo, nullptr, &CShadersLayouts[vRenderShader].pipelineLayout)) Exit("Unable to create pipeline layout");
	}




	{ //Create the pipeline
		VkComputePipelineCreateInfo pipelineCreateInfo = { };									//Create pipeline creation infos
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;					//Structure type
		pipelineCreateInfo.stage = CShadersLayouts[vRenderShader].shaderStageCreateInfo;			//Use the previously created shader stage creation infos
		pipelineCreateInfo.layout = CShadersLayouts[vRenderShader].pipelineLayout;					//Use the previously created pipeline layout
		//Create the compute pipeline
		TryVk(vkCreateComputePipelines(compute.LD, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &CShadersLayouts[vRenderShader].pipeline)) Exit("Unable to create comput pipeline");
		vkDestroyShaderModule(compute.LD, CShadersLayouts[vRenderShader].shaderModule, nullptr);	//Destroy the shader module
	}
}








//TODO fix description
//Creates the descriptor pool and allocates in it the descriptor sets
//*   pCells   | an array of memory cells to bind to the shader
//*      The shader inputs must match those cells
//*      the binding index is the same as their index in the array
//*   vCShader | the shader where to create the descriptor pool and allocate the descriptor buffers
void Engine::cshaderCreateDescriptorSets(LuxShader_t* pCShader, const lux::Array<LuxCell>& pCells, const defRenderShader vRenderShader) {
	//Create descriptor pool and descriptor set allocate infos
		//This struct defines the size of a descriptor pool (how many descriptor sets it can contain)
	VkDescriptorPoolSize descriptorPoolSize = { };
	descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	descriptorPoolSize.descriptorCount = pCells.size( );

	//This struct contains the informations about the descriptor pool. a descriptor pool contains the descriptor sets
	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = { };						//Create descriptor pool create infos
	descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;		//Set structure type
	descriptorPoolCreateInfo.maxSets = 1;												//Allocate only one descriptor set
	descriptorPoolCreateInfo.poolSizeCount = 1;											//One pool size
	descriptorPoolCreateInfo.pPoolSizes = &descriptorPoolSize;							//Set pool size
	descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;	//The descriptor sets can be freed
	//Create descriptor pool
	TryVk(vkCreateDescriptorPool(compute.LD, &descriptorPoolCreateInfo, nullptr, &pCShader->descriptorPool)) Exit("Unable to create descriptor pool");

	//This structure contains the informations about the descriptor set
	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = { };						//Create descriptor set allocate infos
	descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;	//Set structure type
	descriptorSetAllocateInfo.descriptorPool = pCShader->descriptorPool;		//Set descriptor pool where to allocate the descriptor
	descriptorSetAllocateInfo.descriptorSetCount = 1;									//Allocate a single descriptor
	descriptorSetAllocateInfo.pSetLayouts = &CShadersLayouts[vRenderShader].descriptorSetLayout;	//Set set layouts
	//Allocate descriptor set
	TryVk(vkAllocateDescriptorSets(compute.LD, &descriptorSetAllocateInfo, &pCShader->descriptorSet)) Exit("Unable to allocate descriptor sets");



	//Create a descriptor set write for every buffer and update the descriptor sets
	lux::Array<VkWriteDescriptorSet> writeDescriptorSets(pCells.size( ));
	for(uint32 i = 0; i < pCells.size( ); ++i) {
		//Connect the storage buffer to the descrptor
		VkDescriptorBufferInfo* descriptorBufferInfo = (VkDescriptorBufferInfo*)malloc(sizeof(VkDescriptorBufferInfo));	//Create descriptor buffer infos
		descriptorBufferInfo->buffer = CBuffers[getBufferIndex(pCells[i])].buffer;		//Set buffer
		descriptorBufferInfo->offset = getCellOffset(&compute.PD, pCells[i]);		//Set buffer offset
		descriptorBufferInfo->range = getCellSize(pCells[i]);						//Set buffer size

		VkWriteDescriptorSet writeDescriptorSet = { };										//Create write descriptor set
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
	vkUpdateDescriptorSets(compute.LD, writeDescriptorSets.size( ), writeDescriptorSets.begin( ), 0, nullptr);
}







//TODO fix description

void Engine::cshaderCreateDefaultCommandBuffers( ) {
	{ //Copy
		//Create command pool
		static VkCommandPoolCreateInfo commandPoolCreateInfo = { };							//Create command pool create infos. The command pool contains the command buffers
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;				//Set structure type
		commandPoolCreateInfo.flags = 0;														//Default falgs
		commandPoolCreateInfo.queueFamilyIndex = compute.PD.indices.computeFamilies[0];			//Set the compute family where to bind the command pool
		TryVk(vkCreateCommandPool(compute.LD, &commandPoolCreateInfo, nullptr, &copyCommandPool)) Exit("Unable to create command pool");

		//Allocate command buffers
		static VkCommandBufferAllocateInfo commandBufferAllocateInfo = { };					//Create command buffer allocate infos to allocate the command buffer in the command pool
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;		//Set structure type
		commandBufferAllocateInfo.commandPool = copyCommandPool;								//Set command pool where to allocate the command buffer
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;						//Set the command buffer as a primary level command buffer
		commandBufferAllocateInfo.commandBufferCount = swapchainImages.size( );					//Allocate one command buffer for each swapchain image
		TryVk(vkAllocateCommandBuffers(compute.LD, &commandBufferAllocateInfo, copyCommandBuffers.begin( ))) Exit("Unable to allocate command buffers");



		//Record present command buffers
		for(uint32 imgIndex = 0; imgIndex < swapchainImages.size( ); imgIndex++) {	//For every command buffer of the swapchain images
			//Start recording commands
			VkCommandBufferBeginInfo beginInfo = { };								//Create begin infos to start recording the command buffer
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;				//Set structure type
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;				//Set command buffer type. Simultaneous use allows the command buffer to be executed multiple times
			TryVk(vkBeginCommandBuffer(copyCommandBuffers[imgIndex], &beginInfo)) Exit("Unable to begin command buffer recording");


			//Create a barrier to use the swapchain image as a transfer destination optimal to copy the buffer in it
			VkImageMemoryBarrier readToWrite{ };									//Create memory barrier object
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

			VkBufferImageCopy region{ };											//Create bufferImageCopy region to copy the buffer to the image
			region.bufferOffset = 0;													//No buffer offset
			region.bufferRowLength = 0;													//dark magic
			region.bufferImageHeight = 0;												//dark magic
			region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;				//Set aspect mask
			region.imageSubresource.mipLevel = 0;										//No mipmap
			region.imageSubresource.baseArrayLayer = 0;									//Set base
			region.imageSubresource.layerCount = 1;										//No multi layer
			region.imageOffset = { 0, 0, 0 };											//No image offset
			region.imageExtent = { swapchainExtent.width, swapchainExtent.height, 1 };	//Copy the whole buffer
			vkCmdCopyBufferToImage(copyCommandBuffers[imgIndex], CBuffers[getBufferIndex(gpuCellWindowOutput_i)].buffer, swapchainImages[imgIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
			//vkCmdCopyBufferToImage(copyCommandBuffers[imgIndex], CBuffers[getBufferIndex(gpuCellWindowOutput)].buffer, swapchainImages[imgIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

			//Create a barrier to use the swapchain image as a present source image
			VkImageMemoryBarrier writeToRead{ };									//Create memory barrier object
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
}






//TODO fix description

//Creates the shader command buffer that binds pipeline and descriptors and runs the shader
//*   pCShader     | the shader where to create the command buffer
//*   vGroupCountX | the number of workgroups in the x axis
//*   vGroupCounty | the number of workgroups in the y axis
//*   vGroupCountz | the number of workgroups in the z axis
//The workgroup size is define in the GLSL shader
void Engine::cshaderCreateCommandBuffers(LuxShader_t* pCShader, const defRenderShader vRenderShader, const uint32 vGroupCountX, const uint32 vGroupCounty, const uint32 vGroupCountz) {
	//Create command pool
	VkCommandPoolCreateInfo commandPoolCreateInfo = { };									//Create command pool create infos. The command pool contains the command buffers
	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;				//Set structure type
	commandPoolCreateInfo.flags = 0;														//Default falgs
	commandPoolCreateInfo.queueFamilyIndex = compute.PD.indices.computeFamilies[0];			//Set the compute family where to bind the command pool
	TryVk(vkCreateCommandPool(compute.LD, &commandPoolCreateInfo, nullptr, &pCShader->commandPool)) Exit("Unable to create command pool");

	//Allocate command buffers
	VkCommandBufferAllocateInfo commandBufferAllocateInfo = { };							//Create command buffer allocate infos to allocate the command buffer in the command pool
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;		//Set structure type
	commandBufferAllocateInfo.commandPool = pCShader->commandPool;					//Set command pool where to allocate the command buffer
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;						//Set the command buffer as a primary level command buffer
	commandBufferAllocateInfo.commandBufferCount = 1;										//Allocate one command buffer
	pCShader->commandBuffers.resize(1);
	TryVk(vkAllocateCommandBuffers(compute.LD, &commandBufferAllocateInfo, pCShader->commandBuffers.__lp_data)) Exit("Unable to allocate command buffers");




	//Create compute command buffer and start recording commands
	VkCommandBufferBeginInfo beginInfo = { };											//Create begin infos to start recording the command buffer
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;							//Set structure type
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;							//Set command buffer type. Simultaneous use allows the command buffer to be executed multiple times
	TryVk(vkBeginCommandBuffer(pCShader->commandBuffers[0], &beginInfo)) Exit("Unable to begin command buffer recording");





	////Bind pipeline and descriptor sets to the command buffer
	//static int n = 0;
	//if(n == 1) {
	//	vkCmdBindPipeline(pCShader->commandBuffers[0], VK_PIPELINE_BIND_POINT_COMPUTE, CShaders[0].pipeline);
	//	//vkCmdBindPipeline(pCShader->commandBuffers[0], VK_PIPELINE_BIND_POINT_COMPUTE, pCShader->pipeline);
	//	vkCmdBindDescriptorSets(pCShader->commandBuffers[0], VK_PIPELINE_BIND_POINT_COMPUTE, CShaders[0].pipelineLayout, 0, 1, &pCShader->descriptorSet, 0, nullptr);
	//	//vkCmdBindDescriptorSets(pCShader->commandBuffers[0], VK_PIPELINE_BIND_POINT_COMPUTE, CShaders[0].pipelineLayout, 0, 1, &CShaders[0].descriptorSet, 0, nullptr);
	//}
	//else {
	//	vkCmdBindPipeline(pCShader->commandBuffers[0], VK_PIPELINE_BIND_POINT_COMPUTE, pCShader->pipeline);
	//	vkCmdBindDescriptorSets(pCShader->commandBuffers[0], VK_PIPELINE_BIND_POINT_COMPUTE, pCShader->pipelineLayout, 0, 1, &pCShader->descriptorSet, 0, nullptr);
	//}
	//n += 1;
	//Bind pipeline and descriptor sets to the command buffer

	vkCmdBindPipeline(pCShader->commandBuffers[0], VK_PIPELINE_BIND_POINT_COMPUTE, CShadersLayouts[vRenderShader].pipeline);
	vkCmdBindDescriptorSets(pCShader->commandBuffers[0], VK_PIPELINE_BIND_POINT_COMPUTE, CShadersLayouts[vRenderShader].pipelineLayout, 0, 1, &pCShader->descriptorSet, 0, nullptr);







	//Dispatch the compute shader to execute it with the specified workgroups and descriptors
	//TODO fix
	vkCmdDispatch(pCShader->commandBuffers[0], vGroupCountX, vGroupCounty, vGroupCountz);

	//End command buffer recording
	TryVk(vkEndCommandBuffer(pCShader->commandBuffers[0])) Exit("Failed to record command buffer");
}








// Create and destroy shaders ---------------------------------------------------------------------------------------------------------------//








//TODO fix description
//Add a shader in the staging queue. The shader will be created and used by the rendering thread
//All the shaders created with this function must be destroyed with CShaderDestroy(shader)
//*   pCells: an array of memory cells to bind to the shader
//*       The shader inputs must match those cells
//*       the binding index is the same as their index in the array
//*   vShaderPath: The path of the shader that will be executed (Without extension. From LuxEngine/Contents/Shaders/)
//*   returns 0 if the shader was created and loaded successfully
//*       -1 if one or more buffers cannot be used
//*       -2 if the file does not exist
//*       -3 if an unknown error occurs //TODO
int32 Engine::cshaderNew(const lux::Array<LuxCell>& pCells, const defRenderShader vRenderShader, const uint32 vGroupCountX, const uint32 vGroupCounty, const uint32 vGroupCountz) {
	//TODO check buffers
	//TODO check file
	LuxShader_t shader;

	cshaderCreateDescriptorSets(&shader, pCells, vRenderShader);						//Descriptor pool, descriptor sets and descriptor buffers
	cshaderCreateCommandBuffers(&shader, vRenderShader, vGroupCountX, vGroupCounty, vGroupCountz);	//Create command buffers and command pool

	addShaderFence.startSecond();
	CShaders.add(shader);															//Add the shader to the shader array
	addShaderFence.endSecond();

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
	vkDestroyDescriptorPool(compute.LD, CShaders[vCShader].descriptorPool, nullptr);

	//Clear command buffers and command pool
	vkFreeCommandBuffers(compute.LD, CShaders[vCShader].commandPool, 1, CShaders[vCShader].commandBuffers.begin( ));
	vkDestroyCommandPool(compute.LD, CShaders[vCShader].commandPool, nullptr);

	//Free all the useless pointers
	for(uint32 i = 0; i < CShaders[vCShader].__lp_ptrs.size( ); ++i) free(CShaders[vCShader].__lp_ptrs[i]);

	//Remove the shader from the shader array
	CShaders.remove(vCShader);
	return true;
}
