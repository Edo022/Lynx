#pragma once
#define LUX_H_SHADER_T
#include "LuxEngine/Types/VPointer.hpp"




namespace lux{
	template<bufferType buft> struct ShaderElm_b {
		vram::ptr<char, VRam, buft> vdata;	//Gpu data
		ram::ptr<char>              data;	//Local data copy
		uint32                      bind;	//GLSL binding point //FIXME PROBABLY USELESS. REMOVE
	};




	struct Shader_b {
		VkDescriptorPool				descriptorPool;			//A descriptor pool containing the descriptor sets
		VkDescriptorSet					descriptorSet;			//The descriptor sets of the instance (storage buffers, push constants, uniform buffers etc...)
		lux::RtArray<VkCommandBuffer>	commandBuffers;			//The command buffers to execute the shader or other vulkan commands


		~Shader_b(){
			//Clear descriptors sets, descriptor pool and descriptor layout
			vkFreeDescriptorSets   (core::dvc::compute.LD, descriptorPool, 1, &descriptorSet);
			vkDestroyDescriptorPool(core::dvc::compute.LD, descriptorPool, nullptr);
		}
	};



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

}