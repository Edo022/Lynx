
#pragma once
#include "LuxEngine/Core/Compute/CShader_t.h"
#include "LuxEngine/Core/Compute/CBuffers.h"
#include "LuxEngine/Core/Compute/Compute.h"
#include "LuxEngine/Core/Graphics/GSwapchain.h"
#include "LuxEngine/Core/Graphics/Window.h"
#include "LuxEngine/Core/Devices.h"
#include "LuxEngine/Types/Containers/LuxString.h"
#include "LuxEngine/Types/Containers/LuxMap.h"
#include "LuxEngine/Types/LuxFenceDE.h"
#include "LuxEngine/Types/LuxObject/2D/2DRenderSpace.h"
#define LUX_H_CSHADER






namespace lux::core::c::shaders{
	extern String							shaderPath;			//Path to the shaders folder
	extern Map<obj::RenderSpace2D*, uint32>	CRenderSpaces;		//List of renderSpaces
	extern DynArray<LuxShaderLayout_t>			CShadersLayouts;	//Layout of the render shaders

	extern VkCommandPool					commandPool;		//The command pool where render spaces commands are submitted
	extern Map<LuxShader_t, uint32>			CShaders;			//Per-object shaders
	extern DynArray<VkCommandBuffer>		CShadersCBs;		//Per-object command buffers

	extern FenceDE							addShaderFence;		//A fence that synchronizes the creation of a new object's shader and the frame render
	extern LuxShader						clearShader;




	void preInit( );

	uint32*			cshaderReadFromFile(uint32* pLength, const char* pFilePath);
	VkShaderModule	cshaderCreateModule(const VkDevice vDevice, uint32* pCode, const uint32* pLength);

	void			createDefaultCommandBuffers( );
	//void			createDefLayout(const ShaderLayout vRenderShader, const uint32 pCellNum, const DynArray<bool> pIsReadOnly);
	void			createDefLayout(const ShaderLayout vRenderShader, const uint32 pCellNum, const DynArray<bool>& pIsReadOnly);
	void			createDescriptorSets(LuxShader_t* pCShader, const DynArray<rem::Cell>& pCells, ShaderLayout vShaderLayout);
	void			createCommandBuffers(LuxShader_t* pCShader, const ShaderLayout vShaderLayout, const uint32 vGroupCountX, const uint32 vGroupCounty, const uint32 vGroupCountz);

	LuxShader		newShader(const DynArray<rem::Cell>& pCells, const ShaderLayout vShaderLayout, const uint32 vGroupCountX, const uint32 vGroupCounty, const uint32 vGroupCountz);
	void			updateShaderCall(const LuxShader shader, const ShaderLayout vShaderLayout, const uint32 vGroupCountX, const uint32 vGroupCounty, const uint32 vGroupCountz);
	bool			destroyShader(const LuxShader vCShader);
}
