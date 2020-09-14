

#include "LuxEngine/Core/Compute/Compute.h"
#include "LuxEngine/Core/Compute/CBuffers.h"
#include "LuxEngine/Core/Compute/CShader.h"
#include "LuxEngine/Core/Graphics/GSwapchain.h"
#include "LuxEngine/Core/Devices.h"
#include "LuxEngine/Core/Core.h"





#pragma optimize("", off);
PostInitializer(LUX_H_COMPUTE);
#pragma optimize("", on);
namespace lux::core::c{
	VkCommandPool			NoInitVar(copyCommandPool);
	Array<VkCommandBuffer>	NoInitLux(copyCommandBuffers);
	VkCommandBuffer			NoInitVar(clearCommandBuffer);




	void preInit( ) {
		copyCommandPool = nullptr;
		copyCommandBuffers.Array::Array( );
		clearCommandBuffer = nullptr;
	}




	void init(){
		{ //Initialize window buffers and size
			g::wnd::gpuCellWindowOutput_i = rem::alloc(g::wnd::width * g::wnd::height * 4 /*A8-R8-G8-B8 UI*/, CellClass::AUTO, AllocType::DEDICATED_STORAGE);
			g::wnd::gpuCellWindowOutput = rem::alloc(g::wnd::width * g::wnd::height * 4 * 4 /*A32-R32-G32-B32 UF*/, CellClass::AUTO, AllocType::DEDICATED_STORAGE);
			g::wnd::gpuCellWindowZBuffer = rem::alloc(g::wnd::width * g::wnd::height * 4, CellClass::AUTO, AllocType::DEDICATED_STORAGE);

			g::wnd::gpuCellWindowSize = rem::alloc(4 * 2,  CellClass::AUTO, AllocType::SHARED_STORAGE);
			uint32* pwindowSize = scast<uint32*>(g::wnd::gpuCellWindowSize->map());
			pwindowSize[0] = g::swapchain::swapchainExtent.width;
			pwindowSize[1] = g::swapchain::swapchainExtent.height;
			g::wnd::gpuCellWindowSize->unmap();
		}

		{ //#LLID CCB0000 Create copy command buffers
			copyCommandBuffers.resize(g::swapchain::swapchainImages.size( ));	//Resize the command buffer array in the shader
			shaders::createDefaultCommandBuffers( );							//Create command buffers and command pool
		}

		{ //Create default shaders
			//TODO fix
			shaders::CShadersLayouts.resize(ShaderLayout::LUX_DEF_SHADER_NUM, { });
			shaders::createDefLayout(LUX_DEF_SHADER_2D_LINE, 4, { 0, 0, 0, 1 });
			shaders::createDefLayout(LUX_DEF_SHADER_2D_BORDER, 4, { 0, 0, 0, 1 });
			shaders::createDefLayout(LUX_DEF_SHADER_CLEAR, 4, { 0, 0, 0, 0 });

			shaders::clearShader = shaders::newShader(
				{ g::wnd::gpuCellWindowOutput, g::wnd::gpuCellWindowOutput_i, core::g::wnd::gpuCellWindowZBuffer, g::wnd::gpuCellWindowSize },
				LUX_DEF_SHADER_CLEAR, (g::wnd::width * g::wnd::height) / (32 * 32) + 1, 1, 1
			);
		}
	}




	//TODO fix
	void computeCleanup( ) {
		for(uint32 i = 0; i < /*rem::buffers.size( )*/(uint32)CellClassIndex::NUM * (uint32)AllocType::NUM; ++i) {
			for(uint32 j = 0; j < rem::buffers[i].buffers.size( ); ++j){
				if(rem::buffers[i].buffers.isValid(j)){
					vkDestroyBuffer(dvc::compute.LD, rem::buffers[i].buffers[j].buffer, nullptr);
					vkFreeMemory(dvc::compute.LD, rem::buffers[i].buffers[j].memory, nullptr);
				}
			}
		}
	}
}