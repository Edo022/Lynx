

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




	void computeInit( ) {
		{ //Initialize window buffers and size
			g::wnd::gpuCellWindowOutput_i = buffers::gpuCellCreate(g::wnd::width * g::wnd::height * 4	/*A8-R8-G8-B8     UI*/, false);
			g::wnd::gpuCellWindowOutput = buffers::gpuCellCreate(g::wnd::width * g::wnd::height * 4 * 4	/*A32-R32-G32-B32 UF*/, false);

			g::wnd::gpuCellWindowSize = buffers::gpuCellCreate(4 * 2, true);
			uint32* pwindowSize = scast<uint32*>(buffers::gpuCellMap(g::wnd::gpuCellWindowSize));
			pwindowSize[0] = g::swapchain::swapchainExtent.width;
			pwindowSize[1] = g::swapchain::swapchainExtent.height;
		}

		{ //#LLID CCB0000 Create copy command buffers
			copyCommandBuffers.resize(g::swapchain::swapchainImages.size( ));	//Resize the command buffer array in the shader
			shaders::cshaderCreateDefaultCommandBuffers( );				//Create command buffers and command pool
		}

		{ //Create default shaders
			shaders::CShadersLayouts.resize(ShaderLayout::LUX_DEF_SHADER_NUM, { });
			shaders::cshaderCreateDefLayout(LUX_DEF_SHADER_LINE_2D, 3);
			shaders::cshaderCreateDefLayout(LUX_DEF_SHADER_COPY, 3);
			shaders::cshaderNew({ g::wnd::gpuCellWindowOutput, g::wnd::gpuCellWindowOutput_i, g::wnd::gpuCellWindowSize },
				LUX_DEF_SHADER_COPY, g::wnd::width / 32 + 1, g::wnd::height / 32 + 1, 1);
		}
	}






	//TODO fix
	void computeCleanup( ) {
		for(uint32 i = 0; i < buffers::CBuffers.size( ); ++i) {
			if(buffers::CBuffers.isValid(i)) {									//For every buffer
				vkDestroyBuffer(dvc::compute.LD, buffers::CBuffers[i].buffer, nullptr);	//Destroy the buffer
				vkFreeMemory(dvc::compute.LD, buffers::CBuffers[i].memory, nullptr);		//Free its memory
			}
		}
	}
}