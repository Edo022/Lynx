#pragma once
#define LNX_H_SHADER_T
#include "Lynx/Core/IncludeVulkan.hpp"
#include "Lynx/Types/VPointer.hpp"
#include "Lynx/Types/Vectors/Vectors.hpp"
#include "Lynx/Types/Containers/RtArray.hpp"
//FIXME add option to not allow cpu access (no data pointer. just vdata)







namespace lnx{
	class Window;
}


namespace lnx::shd{
	// template<class... tElmTypes> struct ShaderCmp_t{};
	template<bufferType tBufft, class... tElmTypes> struct ShaderElm_b {
		vram::ptr<char, eVRam, tBufft> vdata;	//Gpu data
		ram::ptr<char>                  data;	//Local data copy

		// template<bufferType tCpBufft> auto& operator=(const ShaderElm_b<tCpBufft, tElmTypes...>& pShaderElm) {
		//TODO add operator= for different buffer types
		template<bufferType tCpBufft> auto& operator=(const ShaderElm_b<tCpBufft, tElmTypes...>& pShaderElm) {
			vdata = pShaderElm.vdata;
			 data = pShaderElm. data;
		}
	};




	/**
	 * @brief Base class for shader interface classes
	 */
	struct ShaderInterface_b {
		vk::DescriptorPool				descriptorPool;			//A descriptor pool containing the descriptor sets
		vk::DescriptorSet				descriptorSet;			//The descriptor sets of the instance (storage buffers, push constants, uniform buffers etc...)
		lnx::RtArray<vk::CommandBuffer>	commandBuffers;			//The command buffers to execute the shader or other vulkan commands
		//TODO SAVE COMMAND BUFFERS ARRAY IN RENDER CORE AND USE ONE BUFFER PER OBJECT

		/**
		 * @brief //TODO
		 * ! NOT A GLSL LAYOUT
		 */
		struct Layout{
			Layout(){};
			Layout(const Layout&) = delete;
			Layout(const Layout&&) = delete;
			void operator==(const Layout&) = delete;
			void operator==(const Layout&&) = delete;

			vk::DescriptorSetLayout	          descriptorSetLayout;	//Layout of the descriptor sets
			vk::ShaderModule                  shaderModule;			//Shader module created from the compiled shader file
			vk::PipelineShaderStageCreateInfo shaderStageCreateInfo;//Shader stage

			vk::PipelineLayout                pipelineLayout;		//Layout of the pipeline
		};


		~ShaderInterface_b(){
			//Clear descriptors sets, descriptor pool and descriptor layout
			core::dvc::graphics.ld.freeDescriptorSets(descriptorPool, 1, &descriptorSet);
			core::dvc::graphics.ld.destroyDescriptorPool(descriptorPool, nullptr);
		}
	};
}




