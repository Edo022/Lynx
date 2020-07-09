
#include "LuxEngine/Engine/Engine.h"
#include "LuxEngine/Engine/Compute/CShader_t.h"






//Dark magic. Don't worry, it works
static VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallbackFn(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64 object, uint64 location, int32 messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData) {
	printf("Debug Report: %s: %s\n", pLayerPrefix, pMessage);
	return VK_FALSE;
}





void Engine::runCompute() {
	{
		gpuCellWindowOutput = gpuCellCreate(width * height * 4/*A8-R8-G8-B8*/, false);

		gpuCellWindowSize = gpuCellCreate(4 * 2, true);
		uint32* pwindowSize = rcast<uint32*>(gpuCellMap(gpuCellWindowSize));
		pwindowSize[0] = swapchainExtent.width;
		pwindowSize[1] = swapchainExtent.height;
	}

	{ //#LLID CCB0000 Create copy command buffers 
		copyCommandBuffers.resize(swapchainImages.size());	//Resize the command buffer array in the shader
		cshaderCreateDefaultCommandBuffers();				//Create command buffers and command pool
	}
}






void Engine::cleanupCompute() {
	forEach(CBuffers, i) {
		if (CBuffers.isValid(i)) {
			vkDestroyBuffer(compute.LD, CBuffers[i].buffer, null);							//Destroy the buffer
			vkFreeMemory(compute.LD, CBuffers[i].memory, null);								//Free the memory of the buffer
		}
	}

	//TODO fix
}
