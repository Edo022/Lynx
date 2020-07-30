

#include "LuxEngine/Engine/Engine.h"
#include "LuxEngine/Engine/Compute/CShader_t.h"





void Engine::computeInit
( ) {
	{ //Initialize window buffers and size
		gpuCellWindowOutput_i = gpuCellCreate(width * height * 4	/*A8-R8-G8-B8     UI*/, false);
		gpuCellWindowOutput = gpuCellCreate(width * height * 4 * 4	/*A32-R32-G32-B32 UF*/, false);

		gpuCellWindowSize = gpuCellCreate(4 * 2, true);
		uint32* pwindowSize = scast<uint32*>(gpuCellMap(gpuCellWindowSize));
		pwindowSize[0] = swapchainExtent.width;
		pwindowSize[1] = swapchainExtent.height;
	}

	{ //#LLID CCB0000 Create copy command buffers
		copyCommandBuffers.resize(swapchainImages.size( ));	//Resize the command buffer array in the shader
		cshaderCreateDefaultCommandBuffers( );				//Create command buffers and command pool
	}

	{ //Create default shaders
		CShadersLayouts.resize(defRenderShader::LUX_DEF_SHADER_NUM, { });
		cshaderCreateDefLayout(LUX_DEF_SHADER_LINE_2D, 3);
		cshaderCreateDefLayout(LUX_DEF_SHADER_COPY, 3);
		Engine& engine_ = lux::getEngine( );
		engine_.cshaderNew({ engine_.gpuCellWindowOutput, engine_.gpuCellWindowOutput_i, engine_.gpuCellWindowSize },
			LUX_DEF_SHADER_COPY, engine_.width / 32 + 1, engine_.height / 32 + 1, 1);
	}
}






//TODO fix
void Engine::computeCleanup( ) {
	for(uint32 i = 0; i < CBuffers.size( ); ++i) {
		if(CBuffers.isValid(i)) {									//For every buffer
			vkDestroyBuffer(compute.LD, CBuffers[i].buffer, nullptr);	//Destroy the buffer
			vkFreeMemory(compute.LD, CBuffers[i].memory, nullptr);		//Free its memory
		}
	}
}
