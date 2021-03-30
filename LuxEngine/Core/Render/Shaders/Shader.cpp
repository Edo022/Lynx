#include "LuxEngine/Core/Render/Shaders/Shader.hpp"
#include "LuxEngine/Core/LuxAutoInit.hpp"
#include "LuxEngine/Core/Core.hpp"
#include "LuxEngine/System/System.hpp"








// Shader components create functions -------------------------------------------------------------------------------------------------------//








namespace lux::core::c::shaders{
	alignCache String						shaderPath; //FIXME MAKE WINDOW-LOCAL
	// alignCache RtArray<LuxShaderLayout_t>	CShadersLayouts;




	luxAutoInit(LUX_H_CSHADER){
		c::shaders::shaderPath = sys::dir::thisDir + "/" + getEnginePath() + "/LuxEngine/Contents/shaders/";
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
			uint32 filesize = _ftelli64(fp);					//And get the file count
			_fseeki64(fp, 0, SEEK_SET);										//Go to the beginning of the file
		)
		_lnx(
			fseek(fp, 0, SEEK_END);
			uint32 filesize = ftell(fp);
			fseek(fp, 0, SEEK_SET);
		)

		uint32 paddedFileSize = (uint32)(ceil(filesize / 4.0)) * 4;			//Calculate the padded count

		char* str = (char*)malloc(sizeof(char) * (uint64)paddedFileSize);		//Allocate a buffer to save the file (Freed in createShaderModule function #LLID CSF0000)
		fread(str, (uint64)filesize, sizeof(char), fp);							//Read the file
		fclose(fp);														//Close the file
		for(uint32 i = filesize; i < paddedFileSize; ++i) str[i] = 0;	//Add padding

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
			pWindow.CShadersLayouts[vRenderShader].shaderModule = cshaderCreateModule(dvc::compute.LD, cshaderReadFromFile(&fileLength, (shaderPath + shaderFileName + ".spv").begin()), &fileLength);


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








	// //TODO choose device
	// //TODO remove command buffer from command buffer dynarray
	// /**
	//  * @brief Removes a shader from the shader array, cleaning all of its components and freeing the memory
	//  *
	//  * @param vCShader The shader to destroy
	//  * @return True if the operation succeeded, false if the index is invalid
	//  */
	// //DEPRECATED FUNCTION //FIXME REMOVE
	// bool destroyShader(const LuxShader vCShader, Window& pWindow) {
	// 	pWindow.addShaderFence.lock();
	// 		if(vCShader >= pWindow.swp.shaders.count()) return false;

	// 		//Clear descriptors sets, descriptor pool and descriptor layout
	// 		vkFreeDescriptorSets   (dvc::compute.LD, pWindow.swp.shaders[vCShader].descriptorPool, 1, &pWindow.swp.shaders[vCShader].descriptorSet);
	// 		vkDestroyDescriptorPool(dvc::compute.LD, pWindow.swp.shaders[vCShader].descriptorPool, nullptr);

	// 		//Remove the shader from the shader array
	// 		for(uint32 i = vCShader; i < pWindow.swp.shaders.count() - 1; ++i) pWindow.swp.shaders[i] = pWindow.swp.shaders[i+1]; //FIXME
	// 		pWindow.swp.shaders.resize(pWindow.swp.shaders.count() - 1);
	// 	pWindow.addShaderFence.unlock();

	// 	return true;
	// }
}