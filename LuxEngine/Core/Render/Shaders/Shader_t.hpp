#pragma once
#define LUX_H_SHADER_T
#include <vulkan/vulkan.hpp>
#include "LuxEngine/Types/VPointer.hpp"
#include "LuxEngine/Types/Vectors/Vectors.hpp"
#include "LuxEngine/Types/Containers/RtArray.hpp"





//Default render shaders layouts
//The identifier value corresponds to the index of the shader layout in the engine's default shader layouts array
enum ShaderLayout : uint32 {
	LUX_DEF_SHADER_2D_LINE,
	LUX_DEF_SHADER_2D_BORDER,
	LUX_DEF_SHADER_2D_RENDER_SPACE,		//Useless. It's just to allow the use of luxInitObject in render spaces
	LUX_DEF_SHADER_3D_RENDER_SPACE,		//Useless. It's just to allow the use of luxInitObject in render spaces
	LUX_DEF_SHADER_CLEAR,				//Default shader used to copy and clear the output buffer
	LUX_DEF_SHADER_NUM					//The number of shader layouts
};




namespace lux{
	class Window;


	//This struct contains the elements of a shader layout
	struct LuxShaderLayout_t{ //FIXME REMOVE
		// vk::DescriptorSetLayout	          descriptorSetLayout;	//Layout of the descriptor sets
		// vk::ShaderModule                  shaderModule;			//Shader module created from the sahader compile file
		// vk::PipelineShaderStageCreateInfo shaderStageCreateInfo;//Shader stage

		// vk::PipelineLayout                pipelineLayout;		//Layout of the pipeline
		vk::Pipeline                      pipeline;				//The pipeline that will be boud to the command buffer of the instance
	};




	template<bufferType buft> struct ShaderElm_b {
		vram::ptr<char, VRam, buft> vdata;						//Gpu data
		ram::ptr<char>              data;						//Local data copy
		uint32                      bind;						//GLSL binding point //FIXME PROBABLY USELESS. REMOVE
	};




	struct Shader_b {
		vk::DescriptorPool				descriptorPool;			//A descriptor pool containing the descriptor sets
		vk::DescriptorSet				descriptorSet;			//The descriptor sets of the instance (storage buffers, push constants, uniform buffers etc...)
		lux::RtArray<vk::CommandBuffer>	commandBuffers;			//The command buffers to execute the shader or other vulkan commands

		struct Layout{
			Layout(){};
			Layout(const Layout&) = delete;
			Layout(const Layout&&) = delete;
			void operator==(const Layout&) = delete;
			void operator==(const Layout&&) = delete;

			vk::DescriptorSetLayout	          descriptorSetLayout;	//Layout of the descriptor sets
			vk::ShaderModule                  shaderModule;			//Shader module created from the sahader compile file
			vk::PipelineShaderStageCreateInfo shaderStageCreateInfo;//Shader stage

			vk::PipelineLayout                pipelineLayout;		//Layout of the pipeline
		};


		~Shader_b(){
			//Clear descriptors sets, descriptor pool and descriptor layout
			core::dvc::compute.LD.freeDescriptorSets(descriptorPool, 1, &descriptorSet);
			core::dvc::compute.LD.destroyDescriptorPool(descriptorPool, nullptr);
		}
	};
}




