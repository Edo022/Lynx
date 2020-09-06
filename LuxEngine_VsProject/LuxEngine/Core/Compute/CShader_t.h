#pragma once
#include <vulkan/vulkan.h>
#include "LuxEngine/Types/Containers/LuxArray.h"
#include "LuxEngine/Types/Containers/LuxDynArray.h"
#define LUX_H_CSHADER_T



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
	VkDescriptorPool descriptorPool;						//A descriptor pool containing the descriptor sets
	VkDescriptorSet descriptorSet;							//The descriptor sets of the instance (storage buffers, push constants, uniform buffers etc...)
	lux::Array <VkCommandBuffer> commandBuffers;			//The command buffers to execute the shader or other vulkan commands
	lux::DynArray<void*> __lp_ptrs;							//Same as LuxShaderLayout_t
};


