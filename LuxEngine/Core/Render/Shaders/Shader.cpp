#include "LuxEngine/Core/Render/Shaders/Shader.hpp"
#include "LuxEngine/Core/LuxAutoInit.hpp"
#include "LuxEngine/Core/Core.hpp"
#include "LuxEngine/System/System.hpp"








// Shader components create functions -------------------------------------------------------------------------------------------------------//








namespace lux::core::c::shaders{
	alignCache String						shaderPath; //FIXME MAKE WINDOW-LOCAL
	// alignCache RtArray<LuxShaderLayout_t>	CShadersLayouts;




	luxAutoInit(LUX_H_CSHADER){
		c::shaders::shaderPath = sys::dir::thisDir + "/" + getEnginePath() + "/LuxEngine/Contents/shaders/"; //BUG FIX SHADER PATH
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
			_fseeki64(fp, 0, SEEK_END);				//Go to the end of the file
			uint32 filesize = _ftelli64(fp);		//And get the file count
			_fseeki64(fp, 0, SEEK_SET);				//Go to the beginning of the file
		)
		_lnx(
			fseek(fp, 0, SEEK_END);
			uint32 filesize = ftell(fp); //FIXME use uint64
			fseek(fp, 0, SEEK_SET);
		)

		uint32 paddedFileSize = (uint32)(ceil(filesize / 4.0)) * 4;			//Calculate the padded count

		char* str = (char*)malloc(sizeof(char) * (uint64)paddedFileSize);	//Allocate a buffer to save the file (Freed in createShaderModule function #LLID CSF0000)
		fread(str, (uint64)filesize, sizeof(char), fp);						//Read the file
		fclose(fp);															//Close the file
		for(uint32 i = filesize; i < paddedFileSize; ++i) str[i] = 0;		//Add padding

		*pLength = paddedFileSize;											//Set length
		return (uint32*)str;												//Return the buffer
	}




	/**
	 * @brief Creates a shader module from a compiled shader code and its length in bytes.
	 *		The shader code is freen when the function returns
	 * @param vDevice The logical device to use to create the shader module
	 * @param pCode A pointer to an int32 array containing the shader code
	 * @param pLength A pointer to the code length
	 * @return The created shader module
	 */
	//FIXME FIX
	vk::ShaderModule cshaderCreateModule(const vk::Device vDevice, uint32* pCode, const uint32* pLength) {
		auto createInfo = vk::ShaderModuleCreateInfo() 					//Create shader module infos
			.setCodeSize (*pLength)											//Set the count of the compiled shader code
			.setPCode    (pCode)											//Set the shader code
		;

		vk::ShaderModule shaderModule;									//Create the shader module
		auto r = vDevice.createShaderModule(&createInfo, nullptr, &shaderModule); //FIXME remove r or check it at runtime
		free(pCode);													//#LLID CSF0000 Free memory
		return shaderModule;											//Return the created shader module
	}








	// Shader components ------------------------------------------------------------------------------------------------------------------------//








	/**
	 * @brief reates the descriptor sets layout, the pipeline and the pipeline layout of a shader
	 * @param vRenderShader the type of the shader
	 * @param pCellNum The number of cells to bind to the shader. The shader inputs must match those cells
	 * @param pIsReadOnly //FIXME REMOVE
	 *///FIXME CREATE LAYOUTS IN GENERATED SHADERS .CPPs
	void createDefLayout(const ShaderLayout vRenderShader, Shader_b::Layout& layout_, Window& pWindow) {
		{ //Create the pipeline
			auto pipelineCreateInfo = vk::ComputePipelineCreateInfo() 						//Create pipeline creation infos
				.setStage  (layout_.shaderStageCreateInfo)		//Use the previously created shader stage creation infos
				.setLayout (layout_.pipelineLayout)				//Use the previously created pipeline layout
			;
			dvc::compute.LD.createComputePipelines(nullptr, 1, &pipelineCreateInfo, nullptr, &pWindow.CShadersLayouts[vRenderShader].pipeline); //FIXME USE FUNCTION FOR SINGLE PIPELINE
			// dvc::compute.LD.destroyShaderModule(pWindow.CShadersLayouts[vRenderShader].shaderModule, nullptr);	//Destroy the shader module
			core::dvc::compute.LD.destroyShaderModule(layout_.shaderModule, nullptr); //TODO move to shader implementation
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