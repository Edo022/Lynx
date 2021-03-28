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
}