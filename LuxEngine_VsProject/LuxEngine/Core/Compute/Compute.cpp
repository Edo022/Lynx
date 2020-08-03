

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
			g::gpuCellWindowOutput_i = c::gpuCellCreate(g::width * g::height * 4	/*A8-R8-G8-B8     UI*/, false);
			g::gpuCellWindowOutput = c::gpuCellCreate(g::width * g::height * 4 * 4	/*A32-R32-G32-B32 UF*/, false);

			g::gpuCellWindowSize = c::gpuCellCreate(4 * 2, true);
			uint32* pwindowSize = scast<uint32*>(c::gpuCellMap(g::gpuCellWindowSize));
			pwindowSize[0] = g::swapchainExtent.width;
			pwindowSize[1] = g::swapchainExtent.height;
		}

		{ //#LLID CCB0000 Create copy command buffers
			copyCommandBuffers.resize(g::swapchainImages.size( ));	//Resize the command buffer array in the shader
			c::cshaderCreateDefaultCommandBuffers( );				//Create command buffers and command pool
		}

		{ //Create default shaders
			c::CShadersLayouts.resize(ShaderLayout::LUX_DEF_SHADER_NUM, { });
			c::cshaderCreateDefLayout(LUX_DEF_SHADER_LINE_2D, 3);
			c::cshaderCreateDefLayout(LUX_DEF_SHADER_COPY, 3);
			c::cshaderNew({ g::gpuCellWindowOutput, g::gpuCellWindowOutput_i, g::gpuCellWindowSize },
				LUX_DEF_SHADER_COPY, g::width / 32 + 1, g::height / 32 + 1, 1);
		}
	}






	//TODO fix
	void computeCleanup( ) {
		for(uint32 i = 0; i < c::CBuffers.size( ); ++i) {
			if(c::CBuffers.isValid(i)) {									//For every buffer
				vkDestroyBuffer(g::compute.LD, c::CBuffers[i].buffer, nullptr);	//Destroy the buffer
				vkFreeMemory(g::compute.LD, c::CBuffers[i].memory, nullptr);		//Free its memory
			}
		}
	}
}