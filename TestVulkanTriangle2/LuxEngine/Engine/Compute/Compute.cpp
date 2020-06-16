
#include "LuxEngine/Engine/Engine.h"






//Dark magic. Don't worry, it works
static VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallbackFn(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64 object, uint64 location, int32 messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData) {
	printf("Debug Report: %s: %s\n", pLayerPrefix, pMessage);
	return VK_FALSE;
}




void Engine::runCompute() {
	LuxCell imageOutput = createGpuBuffer(sizeof(Pixel) * COMPUTE_WIDTH * COMPUTE_HEIGHT, LUX_BUFFER_CLASS_LRG);
	LuxCell vertices = createGpuBuffer(4, LUX_BUFFER_CLASS_LRG);
	uint32* mappedVertices = (uint32*)mapGpuBuffer(&CBuffers[1]); mappedVertices[1] = 1;
	newCShader({ imageOutput, vertices }, "LuxEngine/Contents/shaders/comp.spv");
}




void Engine::cleanupCompute() {
	forEach(CBuffers, i) {
		if (CBuffers.isValid(i)) {
			vkDestroyBuffer(compute.LD, CBuffers[i].buffer, null);										//Destroy the buffer
			vkFreeMemory(compute.LD, CBuffers[i].memory, null);									//Free the buffer's memory
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
	if (buffers.size() > CBuffers.size()) return -1; //TODO check indices

	uint64 shaderIndex = CShaders.add(LuxCShader{});
	CShaders[shaderIndex].commandBuffers.resize(swapChainImages.size());

	CShader_create_descriptorSetLayouts(buffers, shaderIndex);
	CShader_create_descriptorSets(buffers, shaderIndex);
	CShader_create_CPipeline(shaderPath, shaderIndex);
	CShader_create_commandBuffers(shaderIndex);

	return shaderIndex;
}







//Creates a memory buffer in a compute device and saves it in the LuxArray "computeBuffers"
//*   size: the size in bytes of the buffer
//*   Returns the buffer's index in the array. -1 if an error occurs
LuxBuffer Engine::createGpuBuffer(uint64 size, LuxBufferClass bufferClass){
	_LuxBufferStruc buffer;
	buffer.size = sc<uint32>(size);
	createBuffer(compute.LD, buffer.size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, buffer.buffer, buffer.memory);
	buffer.bufferClass = bufferClass;
	
	return buffer.ID = CBuffers.add(buffer);
}


//Creates a memory cell into a buffer. Creates a new buffer if the others are full
//*   cellSize: the size in bytes of the cell
//*   Returns the cell's code. -1 if an error occurs
LuxCell Engine::createGpuCell(uint64 cellSize) {
	LuxBufferClass bufferClass;
	if (cellSize <= LUX_BUFFER_CLASS_50) bufferClass = LUX_BUFFER_CLASS_50;
	else if (cellSize <= LUX_BUFFER_CLASS_5K) bufferClass = LUX_BUFFER_CLASS_5K;
	else if (cellSize <= LUX_BUFFER_CLASS_500K) bufferClass = LUX_BUFFER_CLASS_500K;
	else if (cellSize <= LUX_BUFFER_CLASS_2MLN) bufferClass = LUX_BUFFER_CLASS_2MLN;
	else bufferClass = LUX_BUFFER_CLASS_LRG;
	
	if (bufferClass != LUX_BUFFER_CLASS_LRG) {
		LuxBuffer buffer = -1;
		forEach(CBuffers, i) {
			if (CBuffers[i].bufferClass == bufferClass && CBuffers[i].cells.usedSize() == CBuffers[i].cells.size() && CBuffers.isValid(i)) {
				buffer = i;
			}
		}
		if (buffer == (LuxBuffer)-1) buffer = createGpuBuffer(50000000/*50MB*/, bufferClass);
		return sc<LuxCell>(__lp_cellCode(buffer, CBuffers[buffer].cells.add(sc<char>(1)), bufferClass));
	}
	else return sc<LuxCell>(__lp_cellCode(createGpuBuffer(cellSize, LUX_BUFFER_CLASS_LRG), 0, cellSize));
} //TODO track buffers
