#pragma once
#include <vulkan/vulkan.h>
#include "LuxEngine/Types/Containers/LuxArray.h"
#include "LuxEngine/Types/Containers/LuxDynArray.h"




//This struct contains the elements of a shader layout
struct LuxShaderLayout_t{
	VkDescriptorSetLayout descriptorSetLayout;				//Layout of the descriptor sets
	VkShaderModule shaderModule;							//Shader module created from the sahader compile file
	VkPipelineShaderStageCreateInfo shaderStageCreateInfo;	//Shader stage

	VkPipelineLayout pipelineLayout;						//Layout of the pipeline
	VkPipeline pipeline;									//The pipeline that will be boud to the command buffer of the instance

	//Since vulkan uses pointers everywhere, I need to create the objects with malloc() to prevent them from vanishing randomly
	//This array saves all of them so they can be free()d when the shader is destroyed
	lux::DynArray<void*> __lp_ptrs;
};




//This struct contains the elements of a shader instance
struct LuxShader_t {
	//Descriptors
	VkDescriptorPool descriptorPool;						//A descriptor pool containing the descriptor sets
	VkDescriptorSet descriptorSet;							//The descriptor sets of the instance (storage buffers, push constants, uniform buffers etc...)

	//Commands
	VkCommandPool commandPool;								//A command pool containing the command buffers
	lux::Array <VkCommandBuffer> commandBuffers;			//The command buffers to execute the shader or other vulkan commands

	lux::DynArray<void*> __lp_ptrs;							//Same as LuxShaderLayout_t
};


