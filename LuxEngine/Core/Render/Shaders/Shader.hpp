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
	VkShaderModule	cshaderCreateModule(const VkDevice vDevice, uint32* pCode, const uint32* pLength);

	void			createDefLayout(const ShaderLayout vRenderShader, const uint32 pCellNum, const RtArray<bool>& pIsReadOnly, Window& pWindow);
}
