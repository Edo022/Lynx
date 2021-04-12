#pragma once
#define LNX_H_SHADER
#include "Lynx/Core/Render/Window/Window.hpp"
#include "Lynx/Types/Containers/String.hpp"




namespace lnx::core::c::shaders{
	extern String shaderPath; //Path to the shaders folder //TODO EVALUATE AT RUNTIME


	uint32*          loadSpv(uint64* pLength, const char* pFilePath);
	vk::ShaderModule createModule(const vk::Device vDevice, uint32* pCode, const uint64 pLength);
	void             createPipeline(const ShaderLayout vLayout, shd::Shader_b::Layout& layout_, Window& pWindow);
}
