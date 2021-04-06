#pragma once
#define LUX_H_CSHADER
#include "LuxEngine/Core/Render/Window/Window.hpp"
#include "LuxEngine/Types/Containers/LuxString.hpp"




namespace lux::core::c::shaders{
	extern String shaderPath; //Path to the shaders folder //TODO EVALUATE AT RUNTIME


	uint32*          cshaderReadFromFile(uint32* pLength, const char* pFilePath);
	vk::ShaderModule cshaderCreateModule(const vk::Device vDevice, uint32* pCode, const uint32* pLength);
	void             createPipeline(const ShaderLayout vLayout, shd::Shader_b::Layout& layout_, Window& pWindow);
}
