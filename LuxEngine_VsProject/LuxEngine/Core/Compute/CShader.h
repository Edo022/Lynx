
#pragma once
#include "LuxEngine/Core/Compute/CShader_t.h"







namespace lux::core::c{
	extern String									shaderPath;			//Path to the shaders folder
	extern Map<lux::obj::RenderSpace2D*, uint32>	CRenderSpaces;		//List of renderSpaces
	extern Array<LuxShaderLayout_t>					CShadersLayouts;	//Layout of the render shaders
	extern Map<LuxShader_t, uint32>					CShaders;			//Per-object shaders
	//TODO divide render stages
	//TODO use lux map with no effect command buffers in invalid indices, instead of recreating the entire command buffers
	extern DynArray<VkCommandBuffer>				CShadersCBs;		//Per-object command buffers
	extern FenceDE									addShaderFence;		//A fence that synchronizes the creation of a new object's shader and the frame render


	uint32*				cshaderReadFromFile(uint32* pLength, const char* pFilePath);
	VkShaderModule		cshaderCreateModule(const VkDevice vDevice, uint32* pCode, const uint32* pLength);

	void	cshaderCreateDefaultCommandBuffers();
	void	cshaderCreateDefLayout(const ShaderLayout vRenderShader, const uint32 pCellNum);
	void	cshaderCreateDescriptorSets(LuxShader_t* pCShader, const Array<LuxCell>& pCells, ShaderLayout vShaderLayout);
	void	cshaderCreateCommandBuffers(LuxShader_t* pCShader, const ShaderLayout vShaderLayout, const uint32 vGroupCountX, const uint32 vGroupCounty, const uint32 vGroupCountz);
	int32	cshaderNew(const Array<LuxCell>& pCells, const ShaderLayout vShaderLayout, const uint32 vGroupCountX, const uint32 vGroupCounty, const uint32 vGroupCountz);
	bool	cshaderDestroy(const LuxShader vCShader);
}
