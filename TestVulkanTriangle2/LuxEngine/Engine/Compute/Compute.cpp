
#include "LuxEngine/Engine/Engine.h"






//Dark magic. Don't worry, it works
static VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallbackFn(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64 object, uint64 location, int32 messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData) {
	printf("Debug Report: %s: %s\n", pLayerPrefix, pMessage);
	return VK_FALSE;
}




void Engine::runCompute() {
	LuxCell vertices = createGpuCell(4, true);
	uint32* mappedVertices = (uint32*)mapGpuBuffer(&CBuffers[1]); mappedVertices[1] = 1;
	newCShader({ windowOutput, vertices }, "LuxEngine/Contents/shaders/comp.spv");
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
LuxShader Engine::newCShader(LuxArray<LuxCell> buffers, const char* shaderPath) {
	if (buffers.size() > CBuffers.size()) return -1; //TODO check indices

	LuxShader shader = CShaders.add(LuxCShader{});
	CShaders[shader].commandBuffers.resize(swapChainImages.size());

	CShader_create_descriptorSetLayouts(buffers, shader);
	CShader_create_descriptorSets(buffers, shader);
	CShader_create_CPipeline(shaderPath, shader);
	CShader_create_commandBuffers(shader);
	return shader;
}







//Creates a memory buffer in a compute device and saves it in the LuxArray "computeBuffers"
//*   size: the size in bytes of the buffer
//*   cpuAccessible:whether the CPU can access the buffer or not. Non accessible memory is faster but it cannot be mapped
//        Trying to map a non accessible cell will result in an access violation
//*   Returns the buffer's index in the array. -1 if an error occurs
LuxBuffer Engine::createGpuBuffer(uint64 size, LuxBufferClass bufferClass, bool cpuAccessible){
	_LuxBufferStruc buffer;					//Create the buffer struct															
	buffer.size = sc<uint32>(size);			//Set its size and create the vkBuffer as an host visible storage buffer with transfer source capabilities
	createBuffer(
		compute.LD, buffer.size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		(cpuAccessible) ? VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT : VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
		//(cpuAccessible) ? VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT : VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		buffer.buffer, buffer.memory
	);
	buffer.bufferClass = bufferClass;		//Set its class	 
	buffer.cpuAccessible = cpuAccessible;
	return CBuffers.add(buffer);			//Add it to the buffer array and return its index
}
//TODO ctrl+u ctrl+shift+u




//Creates a memory cell into a buffer. Allocates a new buffer if there are no compatible buffers with free cells
//*   cellSize: the size in bytes of the cell
//*   cpuAccessible: whether the CPU can access the cell or not. Non accessible memory is faster but it cannot be mapped
//*   Returns the cell's code. -1 if an error occurs
LuxCell Engine::createGpuCell(uint64 cellSize, bool cpuAccessible){
	LuxBufferClass bufferClass;																	//Create a variable that stores the class of the buffer
	if (cellSize <= LUX_BUFFER_CLASS_50) bufferClass = LUX_BUFFER_CLASS_50;						//Find the required buffer class. A cell must have a size smaller than or equal to the size of the class to belong to it
	else if (cellSize <= LUX_BUFFER_CLASS_5K) bufferClass = LUX_BUFFER_CLASS_5K;
	else if (cellSize <= LUX_BUFFER_CLASS_500K) bufferClass = LUX_BUFFER_CLASS_500K;
	else if (cellSize <= LUX_BUFFER_CLASS_2MLN) bufferClass = LUX_BUFFER_CLASS_2MLN;
	else bufferClass = LUX_BUFFER_CLASS_LRG;

	if (bufferClass != LUX_BUFFER_CLASS_LRG) {													//If it's a static buffer
		LuxBuffer buffer = -1;																		//Initialize the buffer variable. It stores the index of the buffer where the cell will be created
		forEach(CBuffers, i) {																		//Find the buffer. For each of the pre-existent buffers
			if (CBuffers.isValid(i) &&																	//It can be used
				CBuffers[i].cpuAccessible == cpuAccessible &&											//It's of the same memory type,
				CBuffers[i].bufferClass == bufferClass &&												//If it's of the right class,
				CBuffers[i].cells.usedSize() < __lp_static_buffer_size / CBuffers[i].bufferClass) {		//And it has free cells,
				buffer = i;																					//Save its index
			}
		}
		if (buffer == (LuxBuffer)-1) buffer = createGpuBuffer(__lp_static_buffer_size, bufferClass, cpuAccessible);		//If no buffer was found, create a new one with the specified class and a size equal to the static buffer default size and save its index
		return sc<LuxCell>(__lp_cellCode(buffer, CBuffers[buffer].cells.add(sc<char>(1)), bufferClass));				//Create a new cell in the buffer and return its code
	}
	else return sc<LuxCell>(__lp_cellCode(createGpuBuffer(cellSize, LUX_BUFFER_CLASS_LRG, cpuAccessible), 0, cellSize));//If it's a custom size buffer, create a new buffer and return its code
}
