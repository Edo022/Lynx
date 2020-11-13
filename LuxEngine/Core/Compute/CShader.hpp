#pragma once
#define LUX_H_CSHADER
#include "LuxEngine/Core/Compute/CShader_t.hpp"
#include "LuxEngine/Core/Compute/CBuffers.hpp"
#include "LuxEngine/Core/Compute/Compute.hpp"
#include "LuxEngine/Core/Graphics/GSwapchain.hpp"
#include "LuxEngine/Core/Graphics/Window.hpp"
#include "LuxEngine/Core/Devices.hpp"
#include "LuxEngine/Types/Containers/LuxString.hpp"
#include "LuxEngine/Types/Containers/LuxMap.hpp"
#include "LuxEngine/Types/LuxFenceDE.hpp"
#include "LuxEngine/Types/LuxObject/2D/2DRenderSpace.hpp"








namespace lux::core::c::shaders{
	extern String							shaderPath;			//Path to the shaders folder
	extern Map<obj::RenderSpace2D*, uint32>	CRenderSpaces;		//List of renderSpaces
	extern DynArray<LuxShaderLayout_t>		CShadersLayouts;	//Layout of the render shaders

	extern VkCommandPool					commandPool;		//The command pool where render spaces commands are submitted
	extern Map<LuxShader_t, uint32>			CShaders;			//Per-object shaders
	extern DynArray<VkCommandBuffer>		CShadersCBs;		//Per-object command buffers

	extern FenceDE							addShaderFence;		//A fence that synchronizes the creation of a new object's shader and the frame render
	extern LuxShader						clearShader;




	void preInit( );

	uint32*			cshaderReadFromFile(uint32* pLength, const char* pFilePath);
	VkShaderModule	cshaderCreateModule(const VkDevice vDevice, uint32* pCode, const uint32* pLength);

	void			createDefaultCommandBuffers( );
	void			createDefLayout(const ShaderLayout vRenderShader, const uint32 pCellNum, const DynArray<bool>& pIsReadOnly);
	void			createDescriptorSets(LuxShader_t* pCShader, const DynArray<rem::Cell>& pCells, ShaderLayout vShaderLayout);
	void			createCommandBuffers( LuxShader_t* pCShader, const ShaderLayout vShaderLayout, const uint32 vGroupCountX, const uint32 vGroupCountY, const uint32 vGroupCountZ);

	LuxShader		newShader(const DynArray<rem::Cell>& pCells, const ShaderLayout vShaderLayout, const uint32 vGroupCountX, const uint32 vGroupCountY, const uint32 vGroupCountZ);
	void			updateShaderCall(  const LuxShader vCShader, const ShaderLayout vShaderLayout, const uint32 vGroupCountX, const uint32 vGroupCountY, const uint32 vGroupCountZ);
	bool			destroyShader(     const LuxShader vCShader);
}
