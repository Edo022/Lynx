

#include "LuxEngine/Core/Engine.h"
#include "LuxEngine/Core/Compute/Compute.h"





namespace lux::core::c{
	VkCommandPool			copyCommandPool;
	Array<VkCommandBuffer>	copyCommandBuffers;
	VkCommandBuffer			clearCommandBuffer;




	void computeInit( ) {
		{ //Initialize window buffers and size
			lux::core::g::gpuCellWindowOutput_i = lux::core::c::gpuCellCreate(lux::core::g::width * lux::core::g::height * 4	/*A8-R8-G8-B8     UI*/, false);
			lux::core::g::gpuCellWindowOutput = lux::core::c::gpuCellCreate(lux::core::g::width * lux::core::g::height * 4 * 4	/*A32-R32-G32-B32 UF*/, false);

			lux::core::g::gpuCellWindowSize = lux::core::c::gpuCellCreate(4 * 2, true);
			uint32* pwindowSize = scast<uint32*>(lux::core::c::gpuCellMap(lux::core::g::gpuCellWindowSize));
			pwindowSize[0] = lux::getEngine( ).swapchainExtent.width;
			pwindowSize[1] = lux::getEngine( ).swapchainExtent.height;
		}

		{ //#LLID CCB0000 Create copy command buffers
			copyCommandBuffers.resize(lux::getEngine( ).swapchainImages.size( ));	//Resize the command buffer array in the shader
			lux::core::c::cshaderCreateDefaultCommandBuffers( );				//Create command buffers and command pool
		}

		{ //Create default shaders
			lux::core::c::CShadersLayouts.resize(ShaderLayout::LUX_DEF_SHADER_NUM, { });
			lux::core::c::cshaderCreateDefLayout(LUX_DEF_SHADER_LINE_2D, 3);
			lux::core::c::cshaderCreateDefLayout(LUX_DEF_SHADER_COPY, 3);
			lux::core::c::cshaderNew({ lux::core::g::gpuCellWindowOutput, lux::core::g::gpuCellWindowOutput_i, lux::core::g::gpuCellWindowSize },
				LUX_DEF_SHADER_COPY, lux::core::g::width / 32 + 1, lux::core::g::height / 32 + 1, 1);
		}
	}






	//TODO fix
	void computeCleanup( ) {
		for(uint32 i = 0; i < lux::core::c::CBuffers.size( ); ++i) {
			if(lux::core::c::CBuffers.isValid(i)) {									//For every buffer
				vkDestroyBuffer(lux::getEngine( ).compute.LD, lux::core::c::CBuffers[i].buffer, nullptr);	//Destroy the buffer
				vkFreeMemory(lux::getEngine( ).compute.LD, lux::core::c::CBuffers[i].memory, nullptr);		//Free its memory
			}
		}
	}
}