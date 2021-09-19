#pragma once
#define LNX_H_SHADER
#include "Lynx/Core/Render/Shaders/ShaderInterface_t.hpp"
#include "Lynx/Types/Containers/String.hpp"




namespace lnx::core{
	class RenderCore;
}
namespace lnx::core::shaders{
	extern String shaderPath;	//Path to the shaders folder //TODO EVALUATE AT RUNTIME
	extern uint32 pipelineNum;	//The number of shaders. Used in static initialization and in Window class to allocate a lookup table for the pipelines
	extern RtArray<shd::ShaderInterface_b::Layout*> pipelineLayouts;


	uint32*          loadSpv(uint64* pLength, const char* pFilePath);
	vk::ShaderModule createModule(const vk::Device vDevice, uint32* pCode, const uint64 pLength);
	void             createPipeline(const uint32 vPipelineIndex, RenderCore& pRenderCore);
}
