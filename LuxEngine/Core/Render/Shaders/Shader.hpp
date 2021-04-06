#pragma once
#define LUX_H_CSHADER
#include "LuxEngine/Core/Render/Shaders/Shader_t.hpp"
#include "LuxEngine/Core/Render/Window/Window.hpp"
#include "LuxEngine/Types/Containers/LuxString.hpp"
#include "LuxEngine/Types/Containers/RaArray.hpp"




namespace lux::core::c::shaders{
	extern String								shaderPath;			//Path to the shaders folder
	// extern RtArray<LuxShaderLayout_t>			CShadersLayouts;	//Layout of the render shaders


	uint32*			cshaderReadFromFile(uint32* pLength, const char* pFilePath);
	vk::ShaderModule	cshaderCreateModule(const vk::Device vDevice, uint32* pCode, const uint32* pLength);

	void			createPipeline(const ShaderLayout vLayout, shd::Shader_b::Layout& layout_, Window& pWindow);
}
