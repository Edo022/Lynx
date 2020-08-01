

#include "LuxEngine/Core/Engine.h"
#include "LuxEngine/Core/Compute/Compute.h"





namespace lux::core::c{
	VkCommandPool			copyCommandPool;
	Array<VkCommandBuffer>	copyCommandBuffers;
	VkCommandBuffer			clearCommandBuffer;




	void computeInit( ) {
		{ //Initialize window buffers and size
			lux::getEngine( ).gpuCellWindowOutput_i = lux::core::c::gpuCellCreate(lux::getEngine( ).width * lux::getEngine( ).height * 4	/*A8-R8-G8-B8     UI*/, false);
			lux::getEngine( ).gpuCellWindowOutput = lux::core::c::gpuCellCreate(lux::getEngine( ).width * lux::getEngine( ).height * 4 * 4	/*A32-R32-G32-B32 UF*/, false);

			lux::getEngine( ).gpuCellWindowSize = lux::core::c::gpuCellCreate(4 * 2, true);
			uint32* pwindowSize = scast<uint32*>(lux::core::c::gpuCellMap(lux::getEngine( ).gpuCellWindowSize));
			pwindowSize[0] = lux::getEngine( ).swapchainExtent.width;
			pwindowSize[1] = lux::getEngine( ).swapchainExtent.height;
		}

		{ //#LLID CCB0000 Create copy command buffers
			copyCommandBuffers.resize(lux::getEngine( ).swapchainImages.size( ));	//Resize the command buffer array in the shader
			lux::getEngine().cshaderCreateDefaultCommandBuffers( );				//Create command buffers and command pool
		}

		{ //Create default shaders
			lux::getEngine().CShadersLayouts.resize(ShaderLayout::LUX_DEF_SHADER_NUM, { });
			lux::getEngine().cshaderCreateDefLayout(LUX_DEF_SHADER_LINE_2D, 3);
			lux::getEngine().cshaderCreateDefLayout(LUX_DEF_SHADER_COPY, 3);
			Engine& engine_ = lux::getEngine( );
			lux::getEngine().cshaderNew({ engine_.gpuCellWindowOutput, engine_.gpuCellWindowOutput_i, engine_.gpuCellWindowSize },
				LUX_DEF_SHADER_COPY, engine_.width / 32 + 1, engine_.height / 32 + 1, 1);
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