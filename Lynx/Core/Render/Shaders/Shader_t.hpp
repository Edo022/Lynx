#pragma once
#define LNX_H_SHADER_T
#include "Lynx/Core/IncludeVulkan.hpp"
#include "Lynx/Types/VPointer.hpp"
#include "Lynx/Types/Vectors/Vectors.hpp"
#include "Lynx/Types/Containers/RtArray.hpp"





//Default render shaders layouts
//The identifier value corresponds to the index of the shader layout in the engine's default shader layouts array
// enum ShaderLayout : uint32 {
// 	LNX_DEF_SHADER_2D_LINE,
// 	LNX_DEF_SHADER_2D_BORDER,
// 	LNX_DEF_SHADER_2D_RENDER_SPACE,		//FIXME
// 	LNX_DEF_SHADER_3D_RENDER_SPACE,		//FIXME
// 	LNX_DEF_SHADER_CLEAR,				//Default shader used to copy and clear the output buffer
// 	LNX_DEF_SHADER_NUM					//The number of shader layouts
// };




namespace lnx{
	class Window;
}
namespace lnx::shd{


	template<bufferType buft> struct ShaderElm_b {
		vram::ptr<char, VRam, buft> vdata;						//Gpu data
		ram::ptr<char>              data;						//Local data copy
		uint32                      bind;						//GLSL binding point //FIXME PROBABLY USELESS. REMOVE
	};




	struct Shader_b {
		vk::DescriptorPool				descriptorPool;			//A descriptor pool containing the descriptor sets
		vk::DescriptorSet				descriptorSet;			//The descriptor sets of the instance (storage buffers, push constants, uniform buffers etc...)
		lnx::RtArray<vk::CommandBuffer>	commandBuffers;			//The command buffers to execute the shader or other vulkan commands

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


		~Shader_b(){
			//Clear descriptors sets, descriptor pool and descriptor layout
			core::dvc::graphics.LD.freeDescriptorSets(descriptorPool, 1, &descriptorSet);
			core::dvc::graphics.LD.destroyDescriptorPool(descriptorPool, nullptr);
		}
	};
}




