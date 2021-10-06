#pragma once
////#define LNX_H_SHADER
#include "Lynx/Core/Render/Shaders/ShaderInterface_t.hpp"
#include "Lynx/Types/Containers/String.hpp"




namespace lnx::core{
	class RenderCore;
}
namespace lnx::core::shaders{
	_lnx_init_var_dec(String, shaderPath);   //Path to the shaders folder //TODO EVALUATE AT RUNTIME
	_lnx_init_var_dec(uint32, pipelineNum);  //The number of shaders. Used in static initialization and in Window class to allocate a lookup table for the pipelines
	_lnx_init_var_dec(RtArray<shd::ShaderInterface_b::Layout*>, pipelineLayouts);
	_lnx_init_fun_decdec(LNX_H_SHADER);



	uint32*          loadSpv(uint64* pLength, const char* pFilePath);
	vk::ShaderModule createModule(const vk::Device vDevice, uint32* pCode, const uint64 pLength);
	void             createPipeline(const uint32 vPipelineIndex, RenderCore& pRenderCore);
}
