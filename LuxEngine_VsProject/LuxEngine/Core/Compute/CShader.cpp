

#include "LuxEngine/Core/Compute/CShader.h"
#include "LuxEngine/Core/Core.h"







// Shader components create functions -------------------------------------------------------------------------------------------------------//







namespace lux::core::c::shaders{
	String									shaderPath;
	Map<lux::obj::RenderSpace2D*, uint32>	CRenderSpaces;
	Array<LuxShaderLayout_t>				CShadersLayouts;

	VkCommandPool							commandPool;
	Map<LuxShader_t, uint32>				CShaders;
	DynArray<VkCommandBuffer>				CShadersCBs;

	FenceDE									addShaderFence;
	LuxShader								clearShader;







	//Reads a shader from a file and saves it into a padded int32 array
	//*   pLength: a pointer to an int32 where to store the padded code length
	//*   pFilePath: a pointer to a char array containing the path to the compiled shader file
	//*   returns a pointer to the array where the code is saved
	uint32* cshaderReadFromFile(uint32* pLength, const char* pFilePath) {
		FILE* fp;
		fopen_s(&fp, pFilePath, "rb");									//Open the file
		if(fp == NULL) {
			printf("Could not find or open file: %s\n", pFilePath);
			return 0;
		}

		_fseeki64(fp, 0, SEEK_END);										//Go to the end of the file
		int32 filesize = scast<int32>(_ftelli64(fp));					//And get the file size
		_fseeki64(fp, 0, SEEK_SET);										//Go to the beginning of the file
		int32 paddedFileSize = int32(ceil(filesize / 4.0)) * 4;			//Calculate the padded size

		char* str = (char*)malloc(sizeof(char) * paddedFileSize);		//Allocate a buffer to save the file (Freed in createShaderModule function #LLID CSF0000)
		fread(str, filesize, sizeof(char), fp);							//Read the file
		fclose(fp);														//Close the file
		for(int32 i = filesize; i < paddedFileSize; ++i) str[i] = 0;	//Add padding

		*pLength = paddedFileSize;										//Set length
		return (uint32*)str;											//Return the buffer
	}



	//Creates a shader module from a compiled shader code and its size in bytes
	//*   vDevice: the logical device to use to create the shader module
	//*   pCode: a pointer to an int32 array containing the shader code
	//*   pLength: a pointer to the code length
	VkShaderModule cshaderCreateModule(const VkDevice vDevice, uint32* pCode, const uint32* pLength) {
		VkShaderModuleCreateInfo createInfo{ };								//Create shader module infos
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;		//Set structure type
		createInfo.codeSize = *pLength;										//Set the size of the compiled shader code
		createInfo.pCode = pCode;											//Set the shader code

		VkShaderModule shaderModule;										//Create the shader module
		TryVk(vkCreateShaderModule(vDevice, &createInfo, nullptr, &shaderModule)) printError("Failed to create shader module", false, -2);
		free(pCode);														//#LLID CSF0000 Free memory
		return shaderModule;												//Return the created shader module
	}








	// Shader components ------------------------------------------------------------------------------------------------------------------------//








	//This function creates the descriptor sets layout, the pipeline and the pipeline layout of a shader
	//*   vRenderShader | the type of the shader
	//*   pCellNum      | The number of cells to bing to the shader. The shader inputs must match those cells
	void createDefLayout(const ShaderLayout vRenderShader, const uint32 pCellNum, const Array<bool> pIsReadOnly) {
		{ //Create descriptor set layout
			Array<VkDescriptorSetLayoutBinding> bindingLayouts(pCellNum);
			for(uint32 i = 0; i < pCellNum; ++i) {										//Create a binding layout for each cell
				bindingLayouts[i] = VkDescriptorSetLayoutBinding{ 						//The binding layout describes what to bind in a shader binding point and how to use it
					.binding{ i },														//Binding point in the shader
					.descriptorType{ (pIsReadOnly[i]) ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER },				//Type of the descriptor. It depends on the type of data that needs to be bound
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
			TryVk(vkCreateDescriptorSetLayout(dvc::compute.LD, layoutCreateInfo, nullptr, &CShadersLayouts[vRenderShader].descriptorSetLayout)) printError("Unable to create descriptor set layout", false, -2);
			CShadersLayouts[vRenderShader].__lp_ptrs.add((void*)layoutCreateInfo);
		}




		{ //Create pipeline layout
			//Create shader module
			String shaderFileName; uint32 fileLength;
			switch(vRenderShader) {																	//Set shader file name
				case LUX_DEF_SHADER_2D_LINE: shaderFileName = "Line2D"; break;
				case LUX_DEF_SHADER_2D_BORDER: shaderFileName = "Border2D"; break;
				case LUX_DEF_SHADER_CLEAR: shaderFileName = "FloatToIntBuffer"; break;
				default: printError("Unknown shader", vRenderShader, true);
			}
			CShadersLayouts[vRenderShader].shaderModule = cshaderCreateModule(dvc::compute.LD, cshaderReadFromFile(&fileLength, (shaderPath + shaderFileName + ".comp.spv").begin( )), &fileLength);


			//Create stage info
			CShadersLayouts[vRenderShader].shaderStageCreateInfo = VkPipelineShaderStageCreateInfo{
				.sType{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO },		//Set structure type
				.stage{ VK_SHADER_STAGE_COMPUTE_BIT },								//Use it in the compute stage
				.module{ CShadersLayouts[vRenderShader].shaderModule },				//Set shader module
				.pName{ "main" },													//Set the main function as entry point
			};


			//Create pipeline layout
			VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
				.sType{ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO },			//Structure type
				.setLayoutCount{ 1 },												//Number of set layouts
				.pSetLayouts{ &CShadersLayouts[vRenderShader].descriptorSetLayout },//Set set layout
			};
			TryVk(vkCreatePipelineLayout(dvc::compute.LD, &pipelineLayoutCreateInfo, nullptr, &CShadersLayouts[vRenderShader].pipelineLayout)) printError("Unable to create pipeline layout", false, -2);
		}




		{ //Create the pipeline
			VkComputePipelineCreateInfo pipelineCreateInfo = { 						//Create pipeline creation infos
				.sType{ VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO },			//Structure type
				.stage{ CShadersLayouts[vRenderShader].shaderStageCreateInfo },		//Use the previously created shader stage creation infos
				.layout{ CShadersLayouts[vRenderShader].pipelineLayout },			//Use the previously created pipeline layout
			};
			TryVk(vkCreateComputePipelines(dvc::compute.LD, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &CShadersLayouts[vRenderShader].pipeline)) printError("Unable to create comput pipeline", false, -2);
			vkDestroyShaderModule(dvc::compute.LD, CShadersLayouts[vRenderShader].shaderModule, nullptr);	//Destroy the shader module
		}
	}








	//Creates the descriptor pool and allocates in it the descriptor sets
	//*   pCShader      | a pointer to the shader where to create the descriptor pool and allocate the descriptor buffers
	//*   pCells        | an array of memory cells to bind to the shader
	//*      The shader inputs must match those cells
	//*      the binding index is the same as their index in the array
	//*   vShaderLayout | the shader layout
	void createDescriptorSets(LuxShader_t* pCShader, const Array<rem::Cell>& pCells, const ShaderLayout vShaderLayout) {
		//This struct defines the size of a descriptor pool (how many descriptor sets it can contain)
		uint32 storageCount = 0, uniformCount = 0;
		for(uint32 i = 0; i < pCells.size( ); i++){
			if((uint32)pCells[i]->bufferType->allocType & 0b1) uniformCount++;	//#LLID STRT 0003
			else storageCount++;
		}
		Array<VkDescriptorPoolSize> sizes((storageCount != 0) + (uniformCount != 0));
		if(storageCount != 0) sizes[0] = VkDescriptorPoolSize{
			.type{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER },
			.descriptorCount{ storageCount },
		};
		if(uniformCount != 0) sizes[1] = VkDescriptorPoolSize{
			.type{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER },
			.descriptorCount{ uniformCount },
		};

		//Create the descriptor pool that will contain the descriptor sets
		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {				//This struct contains the informations about the descriptor pool
			.sType{ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO },			//Set structure type
			.flags{ VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT },		//The descriptor sets can be freed
			.maxSets{ 1 },														//Allocate only one descriptor set
			.poolSizeCount{ sizes.size( ) },									//One pool size
			.pPoolSizes{ sizes.begin( ) },										//Set pool size
		};
		TryVk(vkCreateDescriptorPool(dvc::compute.LD, &descriptorPoolCreateInfo, nullptr, &pCShader->descriptorPool)) printError("Unable to create descriptor pool", false, -2);




		//Allocate descriptor sets
		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {			//This structure contains the informations about the descriptor set
			.sType{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO },			//Set structure type
			.descriptorPool{ pCShader->descriptorPool },						//Set descriptor pool where to allocate the descriptor
			.descriptorSetCount{ 1 },											//Allocate a single descriptor
			.pSetLayouts{ &CShadersLayouts[vShaderLayout].descriptorSetLayout },//Set set layouts
		};
		TryVk(vkAllocateDescriptorSets(dvc::compute.LD, &descriptorSetAllocateInfo, &pCShader->descriptorSet)) printError("Unable to allocate descriptor sets", false, -2);




		//Create a descriptor set write for each buffer and update the descriptor sets
		Array<VkWriteDescriptorSet> writeDescriptorSets(pCells.size( ));
		for(uint32 i = 0; i < pCells.size( ); ++i) {
			//Connect the storage buffer to the descrptor
			VkDescriptorBufferInfo* descriptorBufferInfo = (VkDescriptorBufferInfo*)malloc(sizeof(VkDescriptorBufferInfo));	//Create descriptor buffer infos
			descriptorBufferInfo->buffer = pCells[i]->buffer->buffer;				//Set buffer    //#LLID STRT 0002 Set buffer offset
			if((uint32)pCells[i]->bufferType->allocType & 0b1) descriptorBufferInfo->offset = rem::getCellOffset(pCells[i]);
			else descriptorBufferInfo->offset = rem::getCellOffset(pCells[i]);		//Set buffer offset
			descriptorBufferInfo->range = pCells[i]->cellSize;						//Set buffer size

			writeDescriptorSets[i] = VkWriteDescriptorSet{ 						//Create write descriptor set
				.sType{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET },					//Set structure type
				.dstSet{ pCShader->descriptorSet },									//Set descriptor set
				.dstBinding{ i },													//Set binding
				.descriptorCount{ 1 },												//Set number of descriptors
				.descriptorType{
					((uint32)pCells[i]->bufferType->allocType & 0b1) ?				//#LLID STRT 0001 Use it as a storage
					VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER },
				.pBufferInfo{ descriptorBufferInfo },								//Set descriptor buffer info
			};
			pCShader->__lp_ptrs.add((void*)descriptorBufferInfo);				//Save the struct in the pointers that needs to be freed
		}
		//Update descriptor sets
		vkUpdateDescriptorSets(dvc::compute.LD, writeDescriptorSets.size( ), writeDescriptorSets.begin( ), 0, nullptr);
	}








	//This function creates the default command buffers for lux rendering
	void createDefaultCommandBuffers( ) {
		{ //Render command pool
			VkCommandPoolCreateInfo commandPoolCreateInfo = { 					//Create command pool create infos
				.sType{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO },				//Set structure type
				.flags{ VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT },			//Command buffers and pool can be reset
				.queueFamilyIndex{ dvc::compute.PD.indices.computeFamilies[0] },	//Set the compute family where to bind the command pool
			};
			TryVk(vkCreateCommandPool(dvc::compute.LD, &commandPoolCreateInfo, nullptr, &commandPool)) printError("Unable to create command pool", false, -2);
		}




		{ //Copy
			//Create command pool
			static VkCommandPoolCreateInfo commandPoolCreateInfo = { 			//Create command pool create infos to create the command pool
				.sType{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO },				//Set structure type
				.flags{ 0 },														//Default falgs
				.queueFamilyIndex{ dvc::compute.PD.indices.computeFamilies[0] },	//Set the compute family where to bind the command pool
			};
			TryVk(vkCreateCommandPool(dvc::compute.LD, &commandPoolCreateInfo, nullptr, &c::copyCommandPool)) printError("Unable to create command pool", false, -2);

			//Allocate one command buffer for each swapchain image
			static VkCommandBufferAllocateInfo commandBufferAllocateInfo = { 	//Create command buffer allocate infos to allocate the command buffer in the command pool
				.sType{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO },			//Set structure type
				.level{ VK_COMMAND_BUFFER_LEVEL_PRIMARY },							//Set the command buffer as a primary level command buffer
			};
			commandBufferAllocateInfo.commandPool = c::copyCommandPool;			//Set command pool where to allocate the command buffer
			commandBufferAllocateInfo.commandBufferCount = g::swapchain::swapchainImages.size( );
			TryVk(vkAllocateCommandBuffers(dvc::compute.LD, &commandBufferAllocateInfo, c::copyCommandBuffers.begin( ))) printError("Unable to allocate command buffers", false, -2);




			//Record a present command buffers for each swapchain images
			for(uint32 imgIndex = 0; imgIndex < g::swapchain::swapchainImages.size( ); imgIndex++) {
				//Start recording commands
				VkCommandBufferBeginInfo beginInfo = { 							//Create begin infos to start recording the command buffer
					.sType{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO },			//Set structure type
					.flags{ VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT },			//Set command buffer type. Simultaneous use allows the command buffer to be executed multiple times
				};
				TryVk(vkBeginCommandBuffer(c::copyCommandBuffers[imgIndex], &beginInfo)) printError("Unable to begin command buffer recording", false, -2);


				//Create a barrier to use the swapchain image as an optimal transfer destination to copy the buffer in it
				VkImageMemoryBarrier readToWrite{ 								//Create memory barrier object
					.sType{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER },				//Set structure type
					.srcAccessMask{ VK_ACCESS_MEMORY_READ_BIT },					//Set source access mask
					.dstAccessMask{ VK_ACCESS_TRANSFER_WRITE_BIT },					//Set destination access mask. It must be writable in order to copy the buffer in it
					.oldLayout{ VK_IMAGE_LAYOUT_UNDEFINED },						//Set old layout. Swapchain images are in undefined layout after being acquired
					.newLayout{ VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL },				//Set new layout. Destination optimal allows the image to be used as a transfer destination
					.srcQueueFamilyIndex{ VK_QUEUE_FAMILY_IGNORED },				//Queue families unset
					.dstQueueFamilyIndex{ VK_QUEUE_FAMILY_IGNORED },				//Queue families unset
					.subresourceRange{												//Create subresource object
						.aspectMask{ VK_IMAGE_ASPECT_COLOR_BIT },						//Set the aspect mask
						.baseMipLevel{ 0 },												//No mipmap
						.levelCount{ 1 },												//No multi leve images
						.baseArrayLayer{ 0 },											//Set base layer
						.layerCount{ 1 },												//No multi layer
				},
				};
				readToWrite.image = g::swapchain::swapchainImages[imgIndex];	//Set swapchain image
				VkPipelineStageFlags 												//Create stage flags
					srcStage{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT },		//The swapchain image is in color output stage
					dstStage{ VK_PIPELINE_STAGE_TRANSFER_BIT };						//Change it to transfer stage to copy the buffer in it
				vkCmdPipelineBarrier(c::copyCommandBuffers[imgIndex], srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &readToWrite);

				VkBufferImageCopy region{ 										//Create bufferImageCopy region to copy the buffer into the image
					.bufferOffset{ 0 },												//No buffer offset
					.bufferRowLength{ 0 },											//dark magic
					.bufferImageHeight{ 0 },										//dark magic
					.imageSubresource{												//Create subresource object
						.aspectMask{ VK_IMAGE_ASPECT_COLOR_BIT },							//Set aspect mask
						.mipLevel{ 0 },														//No mipmap
						.baseArrayLayer{ 0 },												//Set base
						.layerCount{ 1 },													//No multi layer
				},
				.imageOffset{ 0, 0, 0 },										//No image offset
				};
				region.imageExtent = { g::swapchain::swapchainExtent.width, g::swapchain::swapchainExtent.height, 1 };	//Copy the whole buffer
				vkCmdCopyBufferToImage(c::copyCommandBuffers[imgIndex], g::wnd::gpuCellWindowOutput_i->buffer->buffer, g::swapchain::swapchainImages[imgIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);


				//Create a barrier to use the swapchain image as a present source image
				VkImageMemoryBarrier writeToRead{								//Create memory barrier object
					.sType{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER },				//Set structure type
					.srcAccessMask{ VK_ACCESS_TRANSFER_WRITE_BIT },					//Set source access mask
					.dstAccessMask{ VK_ACCESS_MEMORY_READ_BIT },					//Set destination access mask. It must be readable to be displayed
					.oldLayout{ VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL },				//Set old layout. Swapchain images is in dst optimal layout after being written
					.newLayout{ VK_IMAGE_LAYOUT_PRESENT_SRC_KHR },					//Set new layout. Swapchain images must be in this format to be displayed on screen
					.srcQueueFamilyIndex{ VK_QUEUE_FAMILY_IGNORED },				//Queue families unset
					.dstQueueFamilyIndex{ VK_QUEUE_FAMILY_IGNORED },				//Queue families unset
					.subresourceRange{												//Create subresource object
						.aspectMask{ VK_IMAGE_ASPECT_COLOR_BIT },						//Set the aspect mask
						.baseMipLevel{ 0 },												//No mipmap
						.levelCount{ 1 },												//No multi leve images
						.baseArrayLayer{ 0 },											//Set base layer
						.layerCount{ 1 },												//No multi layer
				},
				};
				writeToRead.image = g::swapchain::swapchainImages[imgIndex];	//Set swapchain image
				VkPipelineStageFlags 											//Create stage flags
					srcStage1{ VK_PIPELINE_STAGE_TRANSFER_BIT },					//The image is in transfer stage from the buffer copy
					dstStage1{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };		//Change it to color output to present them
				vkCmdPipelineBarrier(c::copyCommandBuffers[imgIndex], srcStage1, dstStage1, 0, 0, nullptr, 0, nullptr, 1, &writeToRead);

				//End command buffer recording
				TryVk(vkEndCommandBuffer(c::copyCommandBuffers[imgIndex])) printError("Failed to record command buffer", false, -2);
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
	void createCommandBuffers(LuxShader_t* pCShader, const ShaderLayout vShaderLayout, const uint32 vGroupCountX, const uint32 vGroupCounty, const uint32 vGroupCountz) {
		//Allocate command buffers
		VkCommandBufferAllocateInfo commandBufferAllocateInfo = { 		//Create command buffer allocate infos
			.sType{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO },		//Set structure type
			.commandPool{ commandPool },									//Set command pool where to allocate the command buffer
			.level{ VK_COMMAND_BUFFER_LEVEL_PRIMARY },						//Set the command buffer as a primary level command buffer
			.commandBufferCount{ 1 },										//Allocate one command buffer
		};
		pCShader->commandBuffers.resize(1);
		TryVk(vkAllocateCommandBuffers(dvc::compute.LD, &commandBufferAllocateInfo, pCShader->commandBuffers.__lp_data)) printError("Unable to allocate command buffers", false, -2);








		//TODO change only those lines when updating a shader
		//Create compute command buffer and start recording commands
		VkCommandBufferBeginInfo beginInfo = { 							//Create begin infos
			.sType{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO },			//Set structure type
			.flags{ VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT },			//Set command buffer type. Simultaneous use allows the command buffer to be executed multiple times
		};
		TryVk(vkBeginCommandBuffer(pCShader->commandBuffers[0], &beginInfo)) printError("Unable to begin command buffer recording", false, -2);


		//Bind pipeline and descriptors and run the compute shader
		vkCmdBindPipeline(pCShader->commandBuffers[0], VK_PIPELINE_BIND_POINT_COMPUTE, CShadersLayouts[vShaderLayout].pipeline);
		vkCmdBindDescriptorSets(pCShader->commandBuffers[0], VK_PIPELINE_BIND_POINT_COMPUTE, CShadersLayouts[vShaderLayout].pipelineLayout, 0, 1, &pCShader->descriptorSet, 0, nullptr);
		vkCmdDispatch(pCShader->commandBuffers[0], vGroupCountX, vGroupCounty, vGroupCountz);

		//End command buffer recording
		TryVk(vkEndCommandBuffer(pCShader->commandBuffers[0])) printError("Failed to record command buffer", false, -2);
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
	//*   returns       | the index of the shader
	//*       -1 if one or more buffers cannot be used, -2 if the file does not exist, -3 if an unknown error occurs
	LuxShader newShader(const Array<rem::Cell>& pCells, const ShaderLayout vShaderLayout, const uint32 vGroupCountX, const uint32 vGroupCounty, const uint32 vGroupCountz) {
		//TODO check buffers
		//TODO check file
		LuxShader_t shader;

		createDescriptorSets(&shader, pCells, vShaderLayout);									//Descriptor pool, descriptor sets and descriptor buffers
		createCommandBuffers(&shader, vShaderLayout, vGroupCountX, vGroupCounty, vGroupCountz);	//Create command buffers and command pool

		addShaderFence.startSecond( );
		LuxShader i = CShaders.add(shader);														//Add the shader to the shader array
		addShaderFence.endSecond( );
		return i;
	}








	//Updates the cells, layout or group conunts of a shader
	//*   Way faster than destroying and creating it again
	void updateShaderCall(const LuxShader shader, const ShaderLayout vShaderLayout, const uint32 vGroupCountX, const uint32 vGroupCounty, const uint32 vGroupCountz) {
		VkCommandBufferBeginInfo beginInfo = { 							//Create begin infos
			.sType{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO },			//Set structure type
			.flags{ VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT },			//Set command buffer type. Simultaneous use allows the command buffer to be executed multiple times
		};
		TryVk(vkBeginCommandBuffer(CShaders[shader].commandBuffers[0], &beginInfo)) printError("Unable to begin command buffer recording", false, -2);


		//Bind pipeline and descriptors and run the compute shader
		vkCmdBindPipeline(CShaders[shader].commandBuffers[0], VK_PIPELINE_BIND_POINT_COMPUTE, CShadersLayouts[vShaderLayout].pipeline);
		vkCmdBindDescriptorSets(CShaders[shader].commandBuffers[0], VK_PIPELINE_BIND_POINT_COMPUTE, CShadersLayouts[vShaderLayout].pipelineLayout, 0, 1, &CShaders[shader].descriptorSet, 0, nullptr);
		vkCmdDispatch(CShaders[shader].commandBuffers[0], vGroupCountX, vGroupCounty, vGroupCountz);


		//End command buffer recording
		TryVk(vkEndCommandBuffer(CShaders[shader].commandBuffers[0])) printError("Failed to record command buffer", false, -2);

	}






	//TODO choose device
	//TODO remove command buffer from command buffer dynarray
	//Removes a shader from the shader array, cleaning all of its components and freeing the memory
	//*   shader  | the shader to destroy
	//*   returns | true if the operation succeeded, false if the index is invalid
	bool destroyShader(const LuxShader vCShader) {
		if(vCShader >= CShaders.size( )) return false;

		//Clear descriptors sets, descriptor pool and descriptor layout
		vkFreeDescriptorSets(dvc::compute.LD, CShaders[vCShader].descriptorPool, 1, &CShaders[vCShader].descriptorSet);
		vkDestroyDescriptorPool(dvc::compute.LD, CShaders[vCShader].descriptorPool, nullptr);

		//Clear command buffers and command pool
		vkFreeCommandBuffers(dvc::compute.LD, commandPool, 1, CShaders[vCShader].commandBuffers.begin( ));
		vkDestroyCommandPool(dvc::compute.LD, commandPool, nullptr);

		//Free all the useless pointers
		for(uint32 i = 0; i < CShaders[vCShader].__lp_ptrs.size( ); ++i) free(CShaders[vCShader].__lp_ptrs[i]);

		//Remove the shader from the shader array
		CShaders.remove(vCShader);
		return true;
	}
}