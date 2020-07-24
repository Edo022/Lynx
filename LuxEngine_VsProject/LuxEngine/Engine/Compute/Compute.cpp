

#include "LuxEngine/Engine/Engine.h"
#include "LuxEngine/Engine/Compute/CShader_t.h"





	void Engine::runCompute( ) {
		{
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
	}






	void Engine::cleanupCompute( ) {
		for(uint32 i = 0; i < CBuffers.size( ); ++i) {
			if(CBuffers.isValid(i)) {
				vkDestroyBuffer(compute.LD, CBuffers[i].buffer, nullptr);							//Destroy the buffer
				vkFreeMemory(compute.LD, CBuffers[i].memory, nullptr);								//Free the memory of the buffer
			}
		}

		//TODO fix
	}
