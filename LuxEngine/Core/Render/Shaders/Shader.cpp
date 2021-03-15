#include "LuxEngine/Core/Render/Shaders/Shader.hpp"
#include "LuxEngine/Core/LuxAutoInit.hpp"
#include "LuxEngine/Core/Core.hpp"
#include "LuxEngine/System/System.hpp"








// Shader components create functions -------------------------------------------------------------------------------------------------------//








namespace lux::core::c::shaders{
	alignCache String						shaderPath; //FIXME MAKE WINDOW-LOCAL
	// alignCache RaArray<lux::obj::RenderSpace2D*, uint32> CRenderSpaces; //FIXME MAKE WINDOW-LOCAL
	// alignCache RtArray<LuxShaderLayout_t>	CShadersLayouts;








	//TODO remove
	//Deprecated function
	//Compiles a shader from a file. Shader files must have the .comp extension
	static bool compileShader(const char* pShaderPath) {
		_wds(return system((c::shaders::shaderPath + "/glslc.exe " + pShaderPath + " -o " + pShaderPath + ".spv").begin()) == 0;)
		//FIXME USE EXE IN DEPS OR COMPILE DIRECTLY
		_lnx(return system((lux::sys::dir::thisDir + "/" + getEnginePath() + "/deps/Linux/Vulkan-1.2.170.0/x86_64/bin/glslc " + pShaderPath + " -o " + pShaderPath + ".spv").begin()) == 0;)
	}


	luxAutoInit(LUX_H_CSHADER){
		c::shaders::shaderPath = sys::dir::thisDir + "/" + getEnginePath() + "/LuxEngine/Contents/shaders/";

		try {
			for(const auto& name : std::filesystem::recursive_directory_iterator((char*)c::shaders::shaderPath.begin())) {

				String luxStrPath = String((char8*)name.path().u8string().c_str()); //FIXME add string operator+(char)
				_wds(sys::dir::fixWindowsPath(luxStrPath));
				if(sys::dir::getExtensionFromPath(luxStrPath) == "comp") {
					if(!compileShader(luxStrPath.begin())) dbg::printError("compilation error");
					else { Normal printf("%s", (char*)luxStrPath.begin()); }
				}
			}
		}
		catch(const std::system_error& e) {
			std::cout << "system_error. code: " << e.code() << "\nmessage: " << e.what() << '\n';
		}


		// //Create default shaders
		// CShadersLayouts.resize(ShaderLayout::LUX_DEF_SHADER_NUM);
		// shaders::createDefLayout(LUX_DEF_SHADER_2D_LINE,   4, { 0, 0, 0, 1 });
		// shaders::createDefLayout(LUX_DEF_SHADER_2D_BORDER, 4, { 0, 0, 0, 1 });
		// shaders::createDefLayout(LUX_DEF_SHADER_CLEAR,     4, { 0, 0, 0, 0 });
		// //FIXME fix that 01010001 thing
	}








	/**
	 * @brief Reads a shader from a file and saves it into a padded int32 array
	 * @param pLength A pointer to an int32 where to store the padded code length
	 * @param pFilePath A pointer to a char array containing the path to the compiled shader file
	 * @return A pointer to the array where the code was saved
	 */
	uint32* cshaderReadFromFile(uint32* pLength, const char* pFilePath) {
		FILE* fp;
		_wds(fopen_s(&fp, pFilePath, "rb"));							//Open the file
		_lnx(fp = fopen(  pFilePath, "rb"));
		if(!fp) {
			printf("Could not find or open file: %s\n", pFilePath);
			return 0;
		}
		_wds(
			_fseeki64(fp, 0, SEEK_END);										//Go to the end of the file
			int32 filesize = scast<int32>(_ftelli64(fp));					//And get the file count
			_fseeki64(fp, 0, SEEK_SET);										//Go to the beginning of the file
		)
		_lnx(
			fseek(fp, 0, SEEK_END);
			int32 filesize = scast<int32>(ftell(fp));
			fseek(fp, 0, SEEK_SET);
		)

		int32 paddedFileSize = int32(ceil(filesize / 4.0)) * 4;			//Calculate the padded count

		char* str = (char*)malloc(sizeof(char) * paddedFileSize);		//Allocate a buffer to save the file (Freed in createShaderModule function #LLID CSF0000)
		fread(str, filesize, sizeof(char), fp);							//Read the file
		fclose(fp);														//Close the file
		for(int32 i = filesize; i < paddedFileSize; ++i) str[i] = 0;	//Add padding

		*pLength = paddedFileSize;										//Set length
		return (uint32*)str;											//Return the buffer
	}




	/**
	 * @brief Creates a shader module from a compiled shader code and its length in bytes.
	 *		The shader code is freen when the function returns
	 * @param vDevice The logical device to use to create the shader module
	 * @param pCode A pointer to an int32 array containing the shader code
	 * @param pLength A pointer to the code length
	 * @return The created shader module
	 */
	VkShaderModule cshaderCreateModule(const VkDevice vDevice, uint32* pCode, const uint32* pLength) {
		VkShaderModuleCreateInfo createInfo{ 							//Create shader module infos
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,			//Set structure type
			.codeSize = *pLength,											//Set the count of the compiled shader code
			.pCode = pCode													//Set the shader code
		};

		VkShaderModule shaderModule;										//Create the shader module
		dbg::checkVk(vkCreateShaderModule(vDevice, &createInfo, nullptr, &shaderModule), "Failed to create shader module");
		free(pCode);														//#LLID CSF0000 Free memory
		return shaderModule;												//Return the created shader module
	}








	// Shader components ------------------------------------------------------------------------------------------------------------------------//








	/**
	 * @brief reates the descriptor sets layout, the pipeline and the pipeline layout of a shader
	 * @param vRenderShader the type of the shader
	 * @param pCellNum The number of cells to bind to the shader. The shader inputs must match those cells
	 * @param pIsReadOnly //FIXME REMOVE
	 */
	void createDefLayout(const ShaderLayout vRenderShader, const uint32 pCellNum, const RtArray<bool>& pIsReadOnly, Window& pWindow) {
		{ //Create descriptor set layout
			RtArray<VkDescriptorSetLayoutBinding> bindingLayouts(pCellNum);
			for(uint32 i = 0; i < pCellNum; ++i) {										//Create a binding layout for each cell
				bindingLayouts[i] = VkDescriptorSetLayoutBinding{ 							//The binding layout describes what to bind in a shader binding point and how to use it
					.binding            = i,													//Binding point in the shader
					.descriptorType     = (pIsReadOnly[i]) ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,//Type of the descriptor. It depends on the type of data that needs to be bound
					.descriptorCount    = 1,													//Number of descriptors
					.stageFlags         = VK_SHADER_STAGE_COMPUTE_BIT,							//Stage where to use the layout
					.pImmutableSamplers = nullptr												//Default
				};
			}

			//Create a VkDescriptorSetLayoutCreateInfo structure. It contains all the bindings layouts and it's used to create the the VkDescriptorSetLayout
			VkDescriptorSetLayoutCreateInfo layoutCreateInfo{
				.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,	//Structure type
				.pNext = nullptr,	 											//default
				.flags = 0,	 													//default
				.bindingCount = bindingLayouts.count(),	 					//Number of binding layouts
				.pBindings = (bindingLayouts.begin())	 						//The binding layouts
			};
			//Create the descriptor set layout
			dbg::checkVk(vkCreateDescriptorSetLayout(dvc::compute.LD, &layoutCreateInfo, nullptr, &pWindow.CShadersLayouts[vRenderShader].descriptorSetLayout), "Unable to create descriptor set layout");
		}




		{ //Create pipeline layout
			//Create shader module
			String shaderFileName; uint32 fileLength;
			switch(vRenderShader) {																	//Set shader file name
				case LUX_DEF_SHADER_2D_LINE:   shaderFileName = "Line2D";           break;
				case LUX_DEF_SHADER_2D_BORDER: shaderFileName = "Border2D";         break;
				case LUX_DEF_SHADER_CLEAR:     shaderFileName = "FloatToIntBuffer"; break;
				default: dbg::printError("Unknown shader: %d", vRenderShader);
			}
			pWindow.CShadersLayouts[vRenderShader].shaderModule = cshaderCreateModule(dvc::compute.LD, cshaderReadFromFile(&fileLength, (shaderPath + shaderFileName + ".comp.spv").begin()), &fileLength);


			//Create stage info
			pWindow.CShadersLayouts[vRenderShader].shaderStageCreateInfo = VkPipelineShaderStageCreateInfo{
				.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,		//Set structure type
				.stage  = VK_SHADER_STAGE_COMPUTE_BIT,								//Use it in the compute stage
				#ifndef __INTELLISENSE__ 	//! Intellisense sees "module" as the C++ module keyword. This code is enabled during compilation
				.module = pWindow.CShadersLayouts[vRenderShader].shaderModule,				//Set shader module
				.pName  = "main"													//Set the main function as entry point
				#endif
			};


			//Create pipeline layout
			VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
				.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,		//Structure type
				.setLayoutCount = 1,													//Number of set layouts
				.pSetLayouts    = &pWindow.CShadersLayouts[vRenderShader].descriptorSetLayout	//Set set layout
			};
			dbg::checkVk(vkCreatePipelineLayout(dvc::compute.LD, &pipelineLayoutCreateInfo, nullptr, &pWindow.CShadersLayouts[vRenderShader].pipelineLayout), "Unable to create pipeline layout");
		}




		{ //Create the pipeline
			VkComputePipelineCreateInfo pipelineCreateInfo = { 						//Create pipeline creation infos
				.sType  = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,			//Structure type
				.stage  = pWindow.CShadersLayouts[vRenderShader].shaderStageCreateInfo,		//Use the previously created shader stage creation infos
				.layout = pWindow.CShadersLayouts[vRenderShader].pipelineLayout				//Use the previously created pipeline layout
			};
			dbg::checkVk(vkCreateComputePipelines(dvc::compute.LD, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &pWindow.CShadersLayouts[vRenderShader].pipeline), "Unable to create comput pipeline");
			vkDestroyShaderModule(dvc::compute.LD, pWindow.CShadersLayouts[vRenderShader].shaderModule, nullptr);	//Destroy the shader module
		}
	}








	/**
	 * @brief Creates the descriptor pool and allocates in it the descriptor sets
	 * @param pCShader A pointer to the shader where to create the descriptor pool and allocate the descriptor buffers
	 *		The shader inputs must match those cells
	 *		the binding index is the same as their index in the array
	 * @param pCells An array of memory cells to bind to the shader
	 * @param vShaderLayout The shader layout
	 */
	// void createDescriptorSets(LuxShader_t* pCShader, const RtArray<vram::Cell>& pCells, const ShaderLayout vShaderLayout) {
	void createDescriptorSets(LuxShader_t* pCShader, const RtArray<vram::Alloc_b<int32>>& pCells, const ShaderLayout vShaderLayout, Window& pWindow) {
		//This struct defines the count of a descriptor pool (how many descriptor sets it can contain)
		uint32 storageCount = 0, uniformCount = 0;
		for(uint32 i = 0; i < pCells.count(); i++) {								//For every cell
			if(pCells[i].btype == Uniform) uniformCount++;								//#LLID STRT 0003 Count uniform and
			else storageCount++;														//storage cells requested
		}
		RtArray<VkDescriptorPoolSize> sizes(!!storageCount + !!uniformCount);		//Create an array of descriptor sizes with one element for each descriptor type
		if(storageCount != 0) sizes[0] = VkDescriptorPoolSize{							//If there is at least one storage descriptor
			.type            = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,						//Set the element type as storage
			.descriptorCount = storageCount												//And set the number of descriptors
		};
		if(uniformCount != 0) sizes[(storageCount != 0)] = VkDescriptorPoolSize{	//If there is at least one uniform descriptor
			.type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,						//Set the element type as uniform
			.descriptorCount = uniformCount												//And set the number of descriptors
		};

		//Create the descriptor pool that will contain the descriptor sets
		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {						//This struct contains the informations about the descriptor pool
			.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,				//Set structure type
			.flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,			//Allow the descriptor sets to be freed
			.maxSets       = 1,															//Allocate only one descriptor set
			.poolSizeCount = sizes.count(),												//Use one pool size
			.pPoolSizes    = sizes.begin()												//Set the pool size
		};
		dbg::checkVk(vkCreateDescriptorPool(dvc::compute.LD, &descriptorPoolCreateInfo, nullptr, &pCShader->descriptorPool), "Unable to create descriptor pool");




		//Allocate descriptor sets
		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {							//This structure contains the informations about the descriptor set
			.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,				//Set structure type
			.descriptorPool     = pCShader->descriptorPool,										//Set descriptor pool where to allocate the descriptor
			.descriptorSetCount = 1,															//Allocate a single descriptor
			.pSetLayouts        = &pWindow.CShadersLayouts[vShaderLayout].descriptorSetLayout			//Set set layouts
		};
		dbg::checkVk(vkAllocateDescriptorSets(dvc::compute.LD, &descriptorSetAllocateInfo, &pCShader->descriptorSet), "Unable to allocate descriptor sets");




		//Create a descriptor set write for each buffer and update the descriptor sets
		RtArray<VkWriteDescriptorSet> writeDescriptorSets(pCells.count());
		for(uint32 i = 0; i < pCells.count(); ++i) {
			//Connect the storage buffer to the descrptor									//Create descriptor buffer infos
			VkDescriptorBufferInfo* descriptorBufferInfo = (VkDescriptorBufferInfo*)malloc(sizeof(VkDescriptorBufferInfo));
			descriptorBufferInfo->buffer = pCells[i].cell->csc.buffer;							//Set buffer    //#LLID STRT 0002 Set buffer offset
			if(pCells[i].btype == Uniform) descriptorBufferInfo->offset = pCells[i].cell->localOffset;
			else descriptorBufferInfo->offset = pCells[i].cell->localOffset;					//Set buffer offset
			descriptorBufferInfo->range = pCells[i].cell->cellSize;								//Set buffer count

			writeDescriptorSets[i] = VkWriteDescriptorSet{ 									//Create write descriptor set
				.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,						//Set structure type
				.dstSet          = pCShader->descriptorSet,										//Set descriptor set
				.dstBinding      = i,															//Set binding
				.descriptorCount = 1,															//Set number of descriptors
				.descriptorType  =																//#LLID STRT 0001 Set descriptor type
					(pCells[i].btype == Uniform) ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				.pBufferInfo     = descriptorBufferInfo											//Set descriptor buffer info
			};
		}
		//Update descriptor sets
		vkUpdateDescriptorSets(dvc::compute.LD, writeDescriptorSets.count(), writeDescriptorSets.begin(), 0, nullptr);
	}








	/**
	 * @brief Creates the shader command buffer that binds pipeline and descriptors and runs the shader
	 *		The workgroup count is define in the GLSL shader
	 * @param pCShader The shader where to create the command buffer
	 * @param vShaderLayout The render type
	 * @param vGroupCountX The number of workgroups in the x axis
	 * @param vGroupCountY The number of workgroups in the y axis
	 * @param vGroupCountZ The number of workgroups in the z axis
	 */
	void createCommandBuffers(LuxShader_t* pCShader, const ShaderLayout vShaderLayout, const uint32 vGroupCountX, const uint32 vGroupCountY, const uint32 vGroupCountZ, Window& pWindow) {
		//Allocate command buffers
		VkCommandBufferAllocateInfo commandBufferAllocateInfo = { 				//Create command buffer allocate infos
			.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,	//Set structure type
			.commandPool        = pWindow.commandPool,										//Set command pool where to allocate the command buffer
			.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,					//Set the command buffer as a primary level command buffer
			.commandBufferCount = 1													//Allocate one command buffer
		};
		pCShader->commandBuffers.resize(1);
		dbg::checkVk(vkAllocateCommandBuffers(dvc::compute.LD, &commandBufferAllocateInfo, pCShader->commandBuffers.begin()), "Unable to allocate command buffers");




		//TODO change only those lines when updating a shader
		//Create compute command buffer and start recording commands
		VkCommandBufferBeginInfo beginInfo = { 							//Create begin infos
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,			//Set structure type
			.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT			//Set command buffer type. Simultaneous use allows the command buffer to be executed multiple times
		};
		dbg::checkVk(vkBeginCommandBuffer(pCShader->commandBuffers[0], &beginInfo), "Unable to begin command buffer recording");


		//Bind pipeline and descriptors and run the compute shader
		vkCmdBindPipeline      (pCShader->commandBuffers[0], VK_PIPELINE_BIND_POINT_COMPUTE, pWindow.CShadersLayouts[vShaderLayout].pipeline);
		vkCmdBindDescriptorSets(pCShader->commandBuffers[0], VK_PIPELINE_BIND_POINT_COMPUTE, pWindow.CShadersLayouts[vShaderLayout].pipelineLayout, 0, 1, &pCShader->descriptorSet, 0, nullptr);
		vkCmdDispatch          (pCShader->commandBuffers[0], vGroupCountX, vGroupCountY, vGroupCountZ);

		//End command buffer recording
		dbg::checkVk(vkEndCommandBuffer(pCShader->commandBuffers[0]), "Failed to record command buffer");
	}








	// Create and destroy shaders ---------------------------------------------------------------------------------------------------------------//








	//FIXME
	/**
	 * @brief Adds a shader in the staging queue. The shader will be created and used by the rendering thread
	 *		All the shaders created with this function must be destroyed with CShaderDestroy(shader)
	 * @param pCells An array of memory cells to bind to the shader
	 *		The shader inputs must match those cells. The binding index is the same as their index in the array
	 * @param vShaderLayout The layout of the shader
	 * @param vGroupCountX  The number of workgroups in the x axis
	 * @param vGroupCountY  The number of workgroups in the y axis
	 * @param vGroupCountZ  The number of workgroups in the z axis
	 * @return The index of the shader
	 *		-1 if one or more buffers cannot be used, -2 if the file does not exist, -3 if an unknown error occurs
	 *	//FIXME
	 */
	// LuxShader newShader(const RtArray<vram::Cell>& pCells, const ShaderLayou tvShaderLayout, const uint32 vGroupCountX, const uint32 vGroupCountY, const uint32 vGroupCountZ) {
	LuxShader newShader(const RtArray<vram::Alloc_b<uint32>>& pCells, const ShaderLayout vShaderLayout, const uint32 vGroupCountX, const uint32 vGroupCountY, const uint32 vGroupCountZ, Window& pWindow) {
		//TODO check if the layout matches the glsl layout in the shader file. Or just make it automatic idk
		dbg::checkParam(pCells.count() == 0, "pCells", "A shader must use at least one cell. The provided cell array has size 0");
		dbg::checkParam(vGroupCountX < 1, "vGroupCountX", "The group count must be at least 1");
		dbg::checkParam(vGroupCountY < 1, "vGroupCountY", "The group count must be at least 1");
		dbg::checkParam(vGroupCountZ < 1, "vGroupCountZ", "The group count must be at least 1");
		LuxShader_t shader;

		createDescriptorSets(&shader, pCells, vShaderLayout, pWindow);									//Descriptor pool, descriptor sets and descriptor buffers
		createCommandBuffers(&shader, vShaderLayout, vGroupCountX, vGroupCountY, vGroupCountZ, pWindow);	//Create command buffers and command pool

		pWindow.addShaderFence.lock();
		LuxShader i = pWindow.swp.shaders.add(shader);
		pWindow.addShaderFence.unlock();
		return i;
	}








	/**
	 * @brief Updates the cells, layout or group conunts of a shader
	 *		Way faster than destroying and creating it again
	 */
	void updateShaderCall(const LuxShader vCShader, const ShaderLayout vShaderLayout, const uint32 vGroupCountX, const uint32 vGroupCountY, const uint32 vGroupCountZ, Window& pWindow) {
		VkCommandBufferBeginInfo beginInfo = { 							//Create begin infos
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,			//Set structure type
			.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT			//Set command buffer type. Simultaneous use allows the command buffer to be executed multiple times
		};
		pWindow.addShaderFence.lock();
		vkBeginCommandBuffer(pWindow.swp.shaders[vCShader].commandBuffers[0], &beginInfo);
		pWindow.addShaderFence.unlock();
		//Bind pipeline and descriptors and run the compute shader
		vkCmdBindPipeline      (pWindow.swp.shaders[vCShader].commandBuffers[0], VK_PIPELINE_BIND_POINT_COMPUTE, pWindow.CShadersLayouts[vShaderLayout].pipeline);
		vkCmdBindDescriptorSets(pWindow.swp.shaders[vCShader].commandBuffers[0], VK_PIPELINE_BIND_POINT_COMPUTE, pWindow.CShadersLayouts[vShaderLayout].pipelineLayout, 0, 1, &pWindow.swp.shaders[vCShader].descriptorSet, 0, nullptr);
		vkCmdDispatch          (pWindow.swp.shaders[vCShader].commandBuffers[0], vGroupCountX, vGroupCountY, vGroupCountZ);


		//End command buffer recording
		dbg::checkVk(vkEndCommandBuffer(pWindow.swp.shaders[vCShader].commandBuffers[0]), "Failed to record command buffer");
	}








	//TODO choose device
	//TODO remove command buffer from command buffer dynarray
	/**
	 * @brief Removes a shader from the shader array, cleaning all of its components and freeing the memory
	 *
	 * @param vCShader The shader to destroy
	 * @return True if the operation succeeded, false if the index is invalid
	 */
	bool destroyShader(const LuxShader vCShader, Window& pWindow) {
		if(vCShader >= pWindow.swp.shaders.count()) return false;

		//Clear descriptors sets, descriptor pool and descriptor layout
		vkFreeDescriptorSets   (dvc::compute.LD, pWindow.swp.shaders[vCShader].descriptorPool, 1, &pWindow.swp.shaders[vCShader].descriptorSet);
		vkDestroyDescriptorPool(dvc::compute.LD, pWindow.swp.shaders[vCShader].descriptorPool, nullptr);

		//Remove the shader from the shader array
		for(uint32 i = vCShader; i < pWindow.swp.shaders.count() - 1; ++i) pWindow.swp.shaders[i] = pWindow.swp.shaders[i+1]; //FIXME
		pWindow.swp.shaders.resize(pWindow.swp.shaders.count() - 1);

		return true;
	}
}