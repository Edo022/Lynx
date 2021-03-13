#pragma once
#define LUX_H_CSHADER
#include "LuxEngine/Core/Render/Shaders/Shader_t.hpp"
#include "LuxEngine/Core/Render/Window/Window.hpp"
#include "LuxEngine/Types/Containers/LuxString.hpp"
#include "LuxEngine/Types/Containers/RaArray.hpp"







namespace lux::obj{
	struct RenderSpace2D;
}
namespace lux::core::c::shaders{
	extern String								shaderPath;			//Path to the shaders folder
	// extern RaArray<obj::RenderSpace2D*, uint32>	CRenderSpaces;		//List of renderSpaces
	// extern RtArray<LuxShaderLayout_t>			CShadersLayouts;	//Layout of the render shaders



	// void init();

	uint32*			cshaderReadFromFile(uint32* pLength, const char* pFilePath);
	VkShaderModule	cshaderCreateModule(const VkDevice vDevice, uint32* pCode, const uint32* pLength);

	void			createDefLayout(const ShaderLayout vRenderShader, const uint32 pCellNum, const RtArray<bool>& pIsReadOnly, Window* pWindow);
	void			createDescriptorSets(LuxShader_t* pCShader, const RtArray<vram::Alloc_b<int32>>& pCells, ShaderLayout vShaderLayout, Window* pWindow);
	void			createCommandBuffers( LuxShader_t* pCShader, const ShaderLayout vShaderLayout, const uint32 vGroupCountX, const uint32 vGroupCountY, const uint32 vGroupCountZ, Window* pWindow);

	LuxShader		newShader(const RtArray<vram::Alloc_b<uint32>>& pCells,  const ShaderLayout vShaderLayout, const uint32 vGroupCountX, const uint32 vGroupCountY, const uint32 vGroupCountZ, Window& pWindow);
	void			updateShaderCall(  const LuxShader vCShader, const ShaderLayout vShaderLayout, const uint32 vGroupCountX, const uint32 vGroupCountY, const uint32 vGroupCountZ, Window& pWindow);
	bool			destroyShader(     const LuxShader vCShader, Window& pWindow);

}
