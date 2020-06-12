
#include "LuxEngine/Engine/Engine.h"


//     RAM MEMORY                      GPU MEMORY                                                                                                                               
//                                      _____________________________________________________                                                                                                             
//                                    ,'                                                     ',                                                                                                          
//                                    |   gpu buffer         ( LuxCells[0] )             |                                                                                                          
//                                    |   _______________________                             |                                                                                                           
//                                    |  |■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■  |                            |                                                                                                           
//                                    |  |■ ■ ■ ■ ■ ■            |                            |                                                                                                           
//                                    |  '───────────────────────'                            |                                                                                                           
//                                    |                                                       |                                                                                                           
//                                    |                                                       |                                                                                                           
//                                    |   shared gpu buffer  ( LuxCells[1] )             |                                                                                                        
//                                    |   _______________________    __                       |                                                                                                          
//                                    |  |■|■| | |■| | | | |■| | |     | cellSize = 2         |                                                                                                           
//                                    |  |■|■| |■| | |■| |■|■| | |   __|                      |                                                                                                           
//                                    |  '─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─'                            |                                                                                                           
//                                    |  |_______________________|       bufferSize = 24      |                                                                                                           
//                                    |   cellNum  = 12                                       |                                                                                                           
//                                    ',_____________________________________________________,'                                                                                                           
//                                                                                                                                                     



//Dark magic. Don't worry, it works
static VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallbackFn(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64 object, uint64 location, int32 messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData) {
	printf("Debug Report: %s: %s\n", pLayerPrefix, pMessage);
	return VK_FALSE;
}




void Engine::runCompute() {
	LuxCell imageOutput = createGpuBuffer(sizeof(Pixel) * COMPUTE_WIDTH * COMPUTE_HEIGHT);
	LuxCell vertices = createGpuBuffer(4);
	uint32* mappedVertices = (uint32*)mapGpuBuffer(&CGpuBuffers[1]); mappedVertices[1] = 1;
	newCShader({ imageOutput, vertices }, "LuxEngine/Contents/shaders/comp.spv");
}




void Engine::cleanupCompute() {
	forEach(CGpuBuffers, i) {
		if (CGpuBuffers.isValid(i)) {
			vkDestroyBuffer(compute.LD, CGpuBuffers[i].buffer, null);										//Destroy the buffer
			vkFreeMemory(compute.LD, CGpuBuffers[i].memory, null);									//Free the buffer's memory
		}
	}

	//TODO fix
	//vkDestroyDescriptorPool(compute.LD, computeDescriptorPool, null);				//Destroy the descriptor pool
	//vkDestroyDescriptorSetLayout(compute.LD, computeDescriptorSetLayout, null);		//Destroy the descriptor set layout

	//vkDestroyPipeline(compute.LD, computePipeline, null);							//Destroy the pipeline
	//vkDestroyPipelineLayout(compute.LD, computePipelineLayout, null);				//Destroy the pipeline layout

	//vkDestroyCommandPool(compute.LD, computeCommandPool, null);						//Destroy the compute command pool
}




// Utilities --------------------------------------------------------------------------------------------------------------------------------//




//TODO check device limits
//*   buffers: the indices of the buffers to bind. Each index must correspond to a CGpuBuffers's element
//*   returns the index of the created shader if the operation succeed, -1 if the indices cannot be used, -2 if the file cannot be found, -3 if an unknown error occurs 
int32 Engine::newCShader(LuxArray<LuxCell> buffers, const char* shaderPath) {
	if (buffers.size() > CGpuBuffers.size()) return -1;

	uint64 shaderIndex = CShaders.add(LuxCShader{});
	CShader_create_descriptorSetLayouts(buffers, shaderIndex);
	CShader_create_descriptorSets(buffers, shaderIndex);
	CShader_create_CPipeline(shaderPath, shaderIndex);
	CShader_create_commandBuffer(shaderIndex);

	return shaderIndex;
}




//Creates a memory buffer in a compute device and saves it in the LuxArray "computeBuffers"
//*   size: the size in bytes of the buffer
//*   Returns the buffer's index in the array. -1 if an error occurs
LuxCell Engine::createGpuBuffer(uint64 size){
	_LuxCell buffer;
	buffer.size = size;
	createBuffer(compute.LD, buffer.size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, buffer.buffer, buffer.memory);
	return buffer.ID = CGpuBuffers.add(buffer);
}

LuxCell Engine::createGpuFragmentedBuffer(uint64 size, uint64 fragmentSize) {
	LuxCell buffer = createGpuBuffer(size);
	CGpuBuffers[buffer].fragmentSize = fragmentSize;
	return buffer;
}