

#include "LuxEngine/Core/Engine.h"
#include "LuxEngine/Core/Compute/CShader_t.h"








// Shader components create functions -------------------------------------------------------------------------------------------------------//








//This function creates the descriptor sets layout, the pipeline and the pipeline layout of a shader
//*   vRenderShader | the type of the shader
//*   pCellNum      | The number of cells to bing to the shader. The shader inputs must match those cells
void Engine::cshaderCreateDefLayout(const ShaderLayout vRenderShader, const uint32 pCellNum) {
	{ //Create descriptor set layout
		lux::Array<VkDescriptorSetLayoutBinding> bindingLayouts(pCellNum);
		for(uint32 i = 0; i < pCellNum; ++i) {										//Create a binding layout for each cell
			bindingLayouts[i] = VkDescriptorSetLayoutBinding{ 						//The binding layout describes what to bind in a shader binding point and how to use it
				.binding{ i },														//Binding point in the shader
				.descriptorType{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER },				//Type of the descriptor. It depends on the type of data that needs to be bound
				.descriptorCount{ 1 },												//Number of descriptors
				.stageFlags{ VK_SHADER_STAGE_COMPUTE_BIT },							//Stage where to use the layout
				.pImmutableSamplers{ nullptr },										//Default
			};
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
		//Create shader module
		lux::String shaderFileName; uint32 fileLength;
		switch(vRenderShader) {																	//Set shader file name
			case LUX_DEF_SHADER_LINE_2D: shaderFileName = "Line2D"; break;
			case LUX_DEF_SHADER_COPY: shaderFileName = "FloatToIntBuffer"; break;
			default: break; //TODO add unknown shader check
		}
		CShadersLayouts[vRenderShader].shaderModule = cshaderCreateModule(compute.LD, cshaderReadFromFile(&fileLength, (shaderPath + shaderFileName + ".comp.spv").begin( )), &fileLength);


		//Create stage info
		CShadersLayouts[vRenderShader].shaderStageCreateInfo = VkPipelineShaderStageCreateInfo{
			.sType{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO },		//Set structure type
			.stage{ VK_SHADER_STAGE_COMPUTE_BIT },								//Use it in the compute stage
			.module{ CShadersLayouts[vRenderShader].shaderModule },				//Set shader module
			.pName{ "main" },													//Set the main function as entry point
		};


		//Create pipeline layout
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
			.sType{ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO },				//Structure type
			.setLayoutCount{ 1 },												//Number of set layouts
			.pSetLayouts{ &CShadersLayouts[vRenderShader].descriptorSetLayout },	//Set set layout
		};
		TryVk(vkCreatePipelineLayout(compute.LD, &pipelineLayoutCreateInfo, nullptr, &CShadersLayouts[vRenderShader].pipelineLayout)) Exit("Unable to create pipeline layout");
	}




	{ //Create the pipeline
		VkComputePipelineCreateInfo pipelineCreateInfo = { 						//Create pipeline creation infos
			.sType{ VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO },			//Structure type
			.stage{ CShadersLayouts[vRenderShader].shaderStageCreateInfo },		//Use the previously created shader stage creation infos
			.layout{ CShadersLayouts[vRenderShader].pipelineLayout },			//Use the previously created pipeline layout
		};
		TryVk(vkCreateComputePipelines(compute.LD, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &CShadersLayouts[vRenderShader].pipeline)) Exit("Unable to create comput pipeline");
		vkDestroyShaderModule(compute.LD, CShadersLayouts[vRenderShader].shaderModule, nullptr);	//Destroy the shader module
	}
}








//Creates the descriptor pool and allocates in it the descriptor sets
//*   pCShader      | a pointer to the shader where to create the descriptor pool and allocate the descriptor buffers
//*   pCells        | an array of memory cells to bind to the shader
//*      The shader inputs must match those cells
//*      the binding index is the same as their index in the array
//*   vShaderLayout | the shader layout
void Engine::cshaderCreateDescriptorSets(LuxShader_t* pCShader, const lux::Array<LuxCell>& pCells, const ShaderLayout vShaderLayout) {
	//This struct defines the size of a descriptor pool (how many descriptor sets it can contain)
	VkDescriptorPoolSize descriptorPoolSize = {
		.type{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER },
		.descriptorCount{ pCells.size( ) },
	};
	//Create the descriptor pool that will contain the descriptor sets
	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {				//This struct contains the informations about the descriptor pool
		.sType{ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO },			//Set structure type
		.flags{ VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT },		//The descriptor sets can be freed
		.maxSets{ 1 },														//Allocate only one descriptor set
		.poolSizeCount{ 1 },												//One pool size
		.pPoolSizes{ &descriptorPoolSize },									//Set pool size
	};
	TryVk(vkCreateDescriptorPool(compute.LD, &descriptorPoolCreateInfo, nullptr, &pCShader->descriptorPool)) Exit("Unable to create descriptor pool");




	//Allocate descriptor sets
	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {			//This structure contains the informations about the descriptor set
		.sType{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO },			//Set structure type
		.descriptorPool{ pCShader->descriptorPool },						//Set descriptor pool where to allocate the descriptor
		.descriptorSetCount{ 1 },											//Allocate a single descriptor
		.pSetLayouts{ &CShadersLayouts[vShaderLayout].descriptorSetLayout },//Set set layouts
	};
	TryVk(vkAllocateDescriptorSets(compute.LD, &descriptorSetAllocateInfo, &pCShader->descriptorSet)) Exit("Unable to allocate descriptor sets");




	//Create a descriptor set write for each buffer and update the descriptor sets
	lux::Array<VkWriteDescriptorSet> writeDescriptorSets(pCells.size( ));
	for(uint32 i = 0; i < pCells.size( ); ++i) {
		//Connect the storage buffer to the descrptor
		VkDescriptorBufferInfo* descriptorBufferInfo = (VkDescriptorBufferInfo*)malloc(sizeof(VkDescriptorBufferInfo));	//Create descriptor buffer infos
		descriptorBufferInfo->buffer = CBuffers[getBufferIndex(pCells[i])].buffer;	//Set buffer
		descriptorBufferInfo->offset = getCellOffset(&compute.PD, pCells[i]);		//Set buffer offset
		descriptorBufferInfo->range = getCellSize(pCells[i]);						//Set buffer size

		writeDescriptorSets[i] = VkWriteDescriptorSet{ 					//Create write descriptor set
			.sType{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET },					//Set structure type
			.dstSet{ pCShader->descriptorSet },									//Set descriptor set
			.dstBinding{ i },													//Set binding
			.descriptorCount{ 1 },												//Set number of descriptors
			.descriptorType{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER },				//Use it as a storage
			.pBufferInfo{ descriptorBufferInfo },								//Set descriptor buffer info
		};
		pCShader->__lp_ptrs.add((void*)descriptorBufferInfo);			//Save the struct in the pointers that needs to be freed
	}
	//Update descriptor sets
	vkUpdateDescriptorSets(compute.LD, writeDescriptorSets.size( ), writeDescriptorSets.begin( ), 0, nullptr);
}








//This function creates the default command buffers for lux rendering
void Engine::cshaderCreateDefaultCommandBuffers( ) {
	{ //Copy
		//Create command pool
		static VkCommandPoolCreateInfo commandPoolCreateInfo = { 			//Create command pool create infos to create the command pool
			.sType{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO },				//Set structure type
			.flags{ 0 },														//Default falgs
			.queueFamilyIndex{ compute.PD.indices.computeFamilies[0] },			//Set the compute family where to bind the command pool
		};
		TryVk(vkCreateCommandPool(compute.LD, &commandPoolCreateInfo, nullptr, &copyCommandPool)) Exit("Unable to create command pool");

		//Allocate one command buffer for each swapchain image
		static VkCommandBufferAllocateInfo commandBufferAllocateInfo = { 	//Create command buffer allocate infos to allocate the command buffer in the command pool
			.sType{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO },			//Set structure type
			.level{ VK_COMMAND_BUFFER_LEVEL_PRIMARY },							//Set the command buffer as a primary level command buffer
		};
		commandBufferAllocateInfo.commandPool = copyCommandPool;			//Set command pool where to allocate the command buffer
		commandBufferAllocateInfo.commandBufferCount = swapchainImages.size( );
		TryVk(vkAllocateCommandBuffers(compute.LD, &commandBufferAllocateInfo, copyCommandBuffers.begin( ))) Exit("Unable to allocate command buffers");




		//Record a present command buffers for each swapchain images
		for(uint32 imgIndex = 0; imgIndex < swapchainImages.size( ); imgIndex++) {
			//Start recording commands
			VkCommandBufferBeginInfo beginInfo = { 							//Create begin infos to start recording the command buffer
				.sType{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO },			//Set structure type
				.flags{ VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT },			//Set command buffer type. Simultaneous use allows the command buffer to be executed multiple times
			};
			TryVk(vkBeginCommandBuffer(copyCommandBuffers[imgIndex], &beginInfo)) Exit("Unable to begin command buffer recording");


			//Create a barrier to use the swapchain image as an optimal transfer destination to copy the buffer in it
			VkImageMemoryBarrier readToWrite{ 						//Create memory barrier object
				.sType{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER },		//Set structure type
				.srcAccessMask{ VK_ACCESS_MEMORY_READ_BIT },			//Set source access mask
				.dstAccessMask{ VK_ACCESS_TRANSFER_WRITE_BIT },			//Set destination access mask. It must be writable in order to copy the buffer in it
				.oldLayout{ VK_IMAGE_LAYOUT_UNDEFINED },				//Set old layout. Swapchain images are in undefined layout after being acquired
				.newLayout{ VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL },		//Set new layout. Destination optimal allows the image to be used as a transfer destination
				.srcQueueFamilyIndex{ VK_QUEUE_FAMILY_IGNORED },		//Queue families unset
				.dstQueueFamilyIndex{ VK_QUEUE_FAMILY_IGNORED },		//Queue families unset
				.subresourceRange{										//Create subresource object
					.aspectMask{ VK_IMAGE_ASPECT_COLOR_BIT },				//Set the aspect mask
					.baseMipLevel{ 0 },										//No mipmap
					.levelCount{ 1 },										//No multi leve images
					.baseArrayLayer{ 0 },									//Set base layer
					.layerCount{ 1 },										//No multi layer
			},
			};
			readToWrite.image = swapchainImages[imgIndex];			//Set swapchain image
			VkPipelineStageFlags 										//Create stage flags
				srcStage{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT },	//The swapchain image is in color output stage
				dstStage{ VK_PIPELINE_STAGE_TRANSFER_BIT };					//Change it to transfer stage to copy the buffer in it
			vkCmdPipelineBarrier(copyCommandBuffers[imgIndex], srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &readToWrite);

			VkBufferImageCopy region{ 								//Create bufferImageCopy region to copy the buffer into the image
				.bufferOffset{ 0 },										//No buffer offset
				.bufferRowLength{ 0 },									//dark magic
				.bufferImageHeight{ 0 },								//dark magic
				.imageSubresource{										//Create subresource object
					.aspectMask{ VK_IMAGE_ASPECT_COLOR_BIT },					//Set aspect mask
					.mipLevel{ 0 },												//No mipmap
					.baseArrayLayer{ 0 },										//Set base
					.layerCount{ 1 },											//No multi layer
			},
			.imageOffset{ 0, 0, 0 },									//No image offset
			};
			region.imageExtent = { swapchainExtent.width, swapchainExtent.height, 1 };	//Copy the whole buffer
			vkCmdCopyBufferToImage(copyCommandBuffers[imgIndex], CBuffers[getBufferIndex(gpuCellWindowOutput_i)].buffer, swapchainImages[imgIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);


			//Create a barrier to use the swapchain image as a present source image
			VkImageMemoryBarrier writeToRead{						//Create memory barrier object
				.sType{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER },		//Set structure type
				.srcAccessMask{ VK_ACCESS_TRANSFER_WRITE_BIT },			//Set source access mask
				.dstAccessMask{ VK_ACCESS_MEMORY_READ_BIT },			//Set destination access mask. It must be readable to be displayed
				.oldLayout{ VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL },		//Set old layout. Swapchain images is in dst optimal layout after being written
				.newLayout{ VK_IMAGE_LAYOUT_PRESENT_SRC_KHR },			//Set new layout. Swapchain images must be in this format to be displayed on screen
				.srcQueueFamilyIndex{ VK_QUEUE_FAMILY_IGNORED },		//Queue families unset
				.dstQueueFamilyIndex{ VK_QUEUE_FAMILY_IGNORED },		//Queue families unset
				.subresourceRange{										//Create subresource object
					.aspectMask{ VK_IMAGE_ASPECT_COLOR_BIT },				//Set the aspect mask
					.baseMipLevel{ 0 },										//No mipmap
					.levelCount{ 1 },										//No multi leve images
					.baseArrayLayer{ 0 },									//Set base layer
					.layerCount{ 1 },										//No multi layer
			},
			};
			writeToRead.image = swapchainImages[imgIndex];		//Set swapchain image
			VkPipelineStageFlags 								//Create stage flags
				srcStage1{ VK_PIPELINE_STAGE_TRANSFER_BIT },				//The image is in transfer stage from the buffer copy
				dstStage1{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };	//Change it to color output to present them
			vkCmdPipelineBarrier(copyCommandBuffers[imgIndex], srcStage1, dstStage1, 0, 0, nullptr, 0, nullptr, 1, &writeToRead);

			//End command buffer recording
			TryVk(vkEndCommandBuffer(copyCommandBuffers[imgIndex])) Exit("Failed to record command buffer");
		}
	}
}








//Creates the shader command buffer that binds pipeline and descriptors and runs the shader
//*   pCShader      | the shader where to create the command buffer
//*   vShaderLayout | the render type
//*   vGroupCountX  | the number of workgroups in the x axis
//*   vGroupCounty  | the number of workgroups in the y axis
//*   vGroupCountz  | the number of workgroups in the z axis
//The workgroup size is define in the GLSL shader
void Engine::cshaderCreateCommandBuffers(LuxShader_t* pCShader, const ShaderLayout vShaderLayout, const uint32 vGroupCountX, const uint32 vGroupCounty, const uint32 vGroupCountz) {
	//Create command pool to contain the command buffers
	VkCommandPoolCreateInfo commandPoolCreateInfo = { 				//Create command pool create infos
		.sType{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO },			//Set structure type
		.flags{ 0 },													//Default falgs
		.queueFamilyIndex{ compute.PD.indices.computeFamilies[0] },		//Set the compute family where to bind the command pool
	};
	TryVk(vkCreateCommandPool(compute.LD, &commandPoolCreateInfo, nullptr, &pCShader->commandPool)) Exit("Unable to create command pool");

	//Allocate command buffers
	VkCommandBufferAllocateInfo commandBufferAllocateInfo = { 		//Create command buffer allocate infos
		.sType{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO },		//Set structure type
		.commandPool{ pCShader->commandPool },							//Set command pool where to allocate the command buffer
		.level{ VK_COMMAND_BUFFER_LEVEL_PRIMARY },						//Set the command buffer as a primary level command buffer
		.commandBufferCount{ 1 },										//Allocate one command buffer
	};
	pCShader->commandBuffers.resize(1);
	TryVk(vkAllocateCommandBuffers(compute.LD, &commandBufferAllocateInfo, pCShader->commandBuffers.__lp_data)) Exit("Unable to allocate command buffers");




	//Create compute command buffer and start recording commands
	VkCommandBufferBeginInfo beginInfo = { 							//Create begin infos
		.sType{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO },			//Set structure type
		.flags{ VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT },			//Set command buffer type. Simultaneous use allows the command buffer to be executed multiple times
	};
	TryVk(vkBeginCommandBuffer(pCShader->commandBuffers[0], &beginInfo)) Exit("Unable to begin command buffer recording");


	//Bind pipeline and descriptors
	vkCmdBindPipeline(pCShader->commandBuffers[0], VK_PIPELINE_BIND_POINT_COMPUTE, CShadersLayouts[vShaderLayout].pipeline);
	vkCmdBindDescriptorSets(pCShader->commandBuffers[0], VK_PIPELINE_BIND_POINT_COMPUTE, CShadersLayouts[vShaderLayout].pipelineLayout, 0, 1, &pCShader->descriptorSet, 0, nullptr);
	//Dispatch the compute shader to execute it with the specified workgroups and descriptors
	vkCmdDispatch(pCShader->commandBuffers[0], vGroupCountX, vGroupCounty, vGroupCountz);


	//End command buffer recording
	TryVk(vkEndCommandBuffer(pCShader->commandBuffers[0])) Exit("Failed to record command buffer");
}








// Create and destroy shaders ---------------------------------------------------------------------------------------------------------------//







//Add a shader in the staging queue. The shader will be created and used by the rendering thread
//All the shaders created with this function must be destroyed with CShaderDestroy(shader)
//*   pCells        | an array of memory cells to bind to the shader
//*       The shader inputs must match those cells. The binding index is the same as their index in the array
//*   vShaderLayout | the layout of the shader
//*   vGroupCountX  | the number of workgroups in the x axis
//*   vGroupCounty  | the number of workgroups in the y axis
//*   vGroupCountz  | the number of workgroups in the z axis
//*   returns       | 0 if the shader was created and loaded successfully
//*       -1 if one or more buffers cannot be used, -2 if the file does not exist, -3 if an unknown error occurs
int32 Engine::cshaderNew(const lux::Array<LuxCell>& pCells, const ShaderLayout vShaderLayout, const uint32 vGroupCountX, const uint32 vGroupCounty, const uint32 vGroupCountz) {
	//TODO check buffers
	//TODO check file
	LuxShader_t shader;

	cshaderCreateDescriptorSets(&shader, pCells, vShaderLayout);									//Descriptor pool, descriptor sets and descriptor buffers
	cshaderCreateCommandBuffers(&shader, vShaderLayout, vGroupCountX, vGroupCounty, vGroupCountz);	//Create command buffers and command pool

	addShaderFence.startSecond( );
	CShaders.add(shader);																			//Add the shader to the shader array
	addShaderFence.endSecond( );

	return 0;
}








//TODO choose device
//TODO remove command buffer from command buffer dynarray
//Removes a shader from the shader array, cleaning all of its components and freeing the memory
//*   shader  | the shader to destroy
//*   returns | true if the operation succeeded, false if the index is invalid
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
