

#include "LuxEngine/Core/Compute/Compute.h"
#include "LuxEngine/Core/Compute/CBuffers.h"
#include "LuxEngine/Core/Compute/CShader.h"
#include "LuxEngine/Core/Graphics/GSwapchain.h"
#include "LuxEngine/Core/Core.h"
#include "LuxEngine/Core/Devices.h"






namespace lux::core::c{
	VkCommandPool			copyCommandPool;
	Array<VkCommandBuffer>	copyCommandBuffers;
	VkCommandBuffer			clearCommandBuffer;




	void init( ) {
		{ //Initialize window buffers and size
			g::wnd::gpuCellWindowOutput_i = vmem::alloc(g::wnd::width * g::wnd::height * 4 /*A8-R8-G8-B8 UI*/, vmem::LUX_CELL_CLASS_AUTO, vmem::LUX_ALLOC_TYPE_DEDICATED_STORAGE);
			//g::wnd::gpuCellWindowOutput_i = buffers::gpuCellCreate(g::wnd::width * g::wnd::height * 4	/*A8-R8-G8-B8     UI*/, false);
			g::wnd::gpuCellWindowOutput = vmem::alloc(g::wnd::width * g::wnd::height * 4 * 4 /*A32-R32-G32-B32 UF*/, vmem::LUX_CELL_CLASS_AUTO, vmem::LUX_ALLOC_TYPE_DEDICATED_STORAGE);
			//g::wnd::gpuCellWindowOutput = buffers::gpuCellCreate(g::wnd::width * g::wnd::height * 4 * 4	/*A32-R32-G32-B32 UF*/, false);
			g::wnd::gpuCellWindowZBuffer = vmem::alloc(g::wnd::width * g::wnd::height * 4, vmem::LUX_CELL_CLASS_AUTO, vmem::LUX_ALLOC_TYPE_DEDICATED_STORAGE);
			//g::wnd::gpuCellWindowZBuffer = buffers::gpuCellCreate(g::wnd::width * g::wnd::height * 4, false);

			g::wnd::gpuCellWindowSize = vmem::alloc(4 * 2,  vmem::LUX_CELL_CLASS_AUTO, vmem::LUX_ALLOC_TYPE_SHARED_STORAGE);
			//g::wnd::gpuCellWindowSize = buffers::gpuCellCreate(4 * 2, true);
			uint32* pwindowSize = scast<uint32*>(vmem::map(g::wnd::gpuCellWindowSize));
			//uint32* pwindowSize = scast<uint32*>(buffers::gpuCellMap(g::wnd::gpuCellWindowSize));

			pwindowSize[0] = g::swapchain::swapchainExtent.width;
			pwindowSize[1] = g::swapchain::swapchainExtent.height;
		}

		{ //#LLID CCB0000 Create copy command buffers
			copyCommandBuffers.resize(g::swapchain::swapchainImages.size( ));	//Resize the command buffer array in the shader
			shaders::createDefaultCommandBuffers( );							//Create command buffers and command pool
		}

		{ //Create default shaders
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
		for(uint32 i = 0; i < vmem::buffers.size( ); ++i) {
			for(int32 j = 0; j < vmem::buffers[i].buffers.size( ); ++j){
				if(vmem::buffers[i].buffers.isValid(j)){
					vkDestroyBuffer(dvc::compute.LD, vmem::buffers[i].buffers[j].buffer, nullptr);
					vkFreeMemory(dvc::compute.LD, vmem::buffers[i].buffers[j].memory, nullptr);
				}
			}
			//if(buffers::CBuffers.isValid(i)) {											//For every buffer
			//	vkDestroyBuffer(dvc::compute.LD, buffers::CBuffers[i].buffer, nullptr);		//Destroy the buffer
			//	vkFreeMemory(dvc::compute.LD, buffers::CBuffers[i].memory, nullptr);		//Free its memory
			//}
		}
	}
}