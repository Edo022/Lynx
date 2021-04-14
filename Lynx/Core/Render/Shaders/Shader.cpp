#include "Lynx/Core/Render/Shaders/Shader.hpp"
#include "Lynx/Core/AutoInit.hpp"
#include "Lynx/Core/Core.hpp"
#include "Lynx/System/System.hpp"








namespace lnx::core::shaders{
	alignCache String shaderPath;
	alignCache uint32 pipelineNum = 0;
	alignCache RtArray<shd::Shader_b::Layout*> pipelineLayouts;



	LnxAutoInit(LNX_H_SHADER){
		shaders::shaderPath = sys::dir::thisDir + "/" + getEnginePath() + "/Lynx/shaders/"; //TODO EVALUATE AT RUNTIME
	}




	/**
	 * @brief Reads a shader from a file and saves it into a padded int32 array
	 * @param pLength A pointer to an int32 where to store the padded code length
	 * @param pFilePath A pointer to a char array containing the path to the compiled shader file
	 * @return A pointer to the array where the code was saved
	 */
	uint32* loadSpv(uint64* pLength, const char* pFilePath) {
		//Open file
			FILE* fp;
			_wds(fopen_s(&fp, pFilePath, "rb"));							//Open the file
			_lnx(fp = fopen(  pFilePath, "rb"));
			if(!fp) {
				printf("Could not find or open file: %s\n", pFilePath);
				return 0;
			}

		//Get file size
			_wds(_fseeki64(fp, 0, SEEK_END); uint64 fs =  _ftelli64(fp); _fseeki64(fp, 0, SEEK_SET);)
			_lnx(    fseek(fp, 0, SEEK_END); uint64 fs = (u64)ftell(fp);     fseek(fp, 0, SEEK_SET);)

		//Calculate padded size and allocate a memory block
			uint64 pfs = ceil(fs / 4.0) * 4;
			char* str = (char*)malloc(sizeof(char) * pfs);	//! Freed in createShaderModule function

		//Read the file and add padding
			fread(str, fs, sizeof(char), fp);
			fclose(fp);
			for(uint64 i = fs; i < pfs; ++i) str[i] = 0;

		//Set length and return the block
			*pLength = pfs;
			return (uint32*)str;
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
	vk::ShaderModule createModule(const vk::Device vDevice, uint32* pCode, const uint64 pLength) {
		auto createInfo = vk::ShaderModuleCreateInfo() 					//Create shader module infos
			.setCodeSize (pLength)											//Set the count of the compiled shader code
			.setPCode    (pCode)											//Set the shader code
		;

		vk::ShaderModule shaderModule;									//Create the shader module
		switch(vDevice.createShaderModule(&createInfo, nullptr, &shaderModule)){
			case vk::Result::eErrorInvalidShaderNV:   dbg::printError("Invalid shader"); break;
			vkDefaultCases;
		}

		// free(pCode);													//#LLID CSF0000 Free memory //BUG
		return shaderModule;											//Return the created shader module
	}








	// Shader components ------------------------------------------------------------------------------------------------------------------------//








	/**
	 * @brief Creates a compute pipeline in the pipeline array of the window
	 * @param vPipelineIndex The index of the pipeline
	 * @param pWindow The window in which to create the pipeline
	 */
	void createPipeline(const uint32 vPipelineIndex, Window& pWindow) {
		auto pipelineInfo = vk::ComputePipelineCreateInfo()
			.setStage  (pipelineLayouts[vPipelineIndex]->shaderStageCreateInfo)
			.setLayout (pipelineLayouts[vPipelineIndex]->pipelineLayout)
		;
		auto r = dvc::graphics.LD.createComputePipeline(nullptr, pipelineInfo, nullptr);


		switch(r.result){
			case vk::Result::ePipelineCompileRequiredEXT: dbg::printWarning("Pipeline compile required"); [[fallthrough]];
			case vk::Result::eSuccess: pWindow.pipelines[vPipelineIndex] = r.value; break;
			case vk::Result::eErrorInvalidShaderNV:       dbg::printError("Invalid shader NV");    break;
			vkDefaultFaulures;
		}
		// core::dvc::graphics.LD.destroyShaderModule(layout_.shaderModule, nullptr);
		//FIXME^ FREE THE SHADER MODULES WHEN KILLING THE ENGINE (or closing the window? idk)
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
	// bool destroyShader(const LnxShader vCShader, Window& pWindow) {
	// 	pWindow.addObject_m.lock();
	// 		if(vCShader >= pWindow.swp.shaders.count()) return false;

	// 		//Clear descriptors sets, descriptor pool and descriptor layout
	// 		vkFreeDescriptorSets   (dvc::compute.LD, pWindow.swp.shaders[vCShader].descriptorPool, 1, &pWindow.swp.shaders[vCShader].descriptorSet);
	// 		vkDestroyDescriptorPool(dvc::compute.LD, pWindow.swp.shaders[vCShader].descriptorPool, nullptr);

	// 		//Remove the shader from the shader array
	// 		for(uint32 i = vCShader; i < pWindow.swp.shaders.count() - 1; ++i) pWindow.swp.shaders[i] = pWindow.swp.shaders[i+1]; //FIXME
	// 		pWindow.swp.shaders.resize(pWindow.swp.shaders.count() - 1);
	// 	pWindow.addObject_m.unlock();

	// 	return true;
	// }
}