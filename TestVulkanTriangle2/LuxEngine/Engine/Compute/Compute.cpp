
#include "LuxEngine/Engine/Engine.h"



//Dark magic. Don't worry, it works
static VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallbackFn(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64 object, uint64 location, int32 messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData) {
	printf("Debug Report: %s: %s\n", pLayerPrefix, pMessage);
	return VK_FALSE;
}




void Engine::runCompute() {
	uint64 imageOutput = createComputeBuffer(sizeof(Pixel) * COMPUTE_WIDTH * COMPUTE_HEIGHT);
	uint64 vertices = createComputeBuffer(4);
	uint32* mappedVertices = (uint32*)mapGpuBuffer(&CBuffers[1]); mappedVertices[1] = 1;
	newCShader({ imageOutput, vertices }, "LuxEngine/Contents/shaders/comp.spv");
	//CShader_create_descriptorSetLayouts();
	//CShader_create_descriptorSets();
	//CShader_create_CPipeline();
	//CShader_create_commandBuffer();
}




void Engine::cleanupCompute() {
	forEach(CBuffers, i) {
		if (CBuffers.isValid(i)) {
			vkDestroyBuffer(compute.LD, CBuffers[i].buffer, null);										//Destroy the buffer
			vkFreeMemory(compute.LD, CBuffers[i].memory, null);									//Free the buffer's memory
		}
	}

	vkDestroyDescriptorPool(compute.LD, computeDescriptorPool, null);				//Destroy the descriptor pool
	vkDestroyDescriptorSetLayout(compute.LD, computeDescriptorSetLayout, null);		//Destroy the descriptor set layout

	vkDestroyPipeline(compute.LD, computePipeline, null);							//Destroy the pipeline
	vkDestroyPipelineLayout(compute.LD, computePipelineLayout, null);				//Destroy the pipeline layout

	vkDestroyCommandPool(compute.LD, computeCommandPool, null);						//Destroy the compute command pool
}




// Utilities --------------------------------------------------------------------------------------------------------------------------------//




//TODO check device limits
//*   bufferIndices: the indices of the buffers to bind. Each index must correspond to a CBuffers's element
//*   returns 0 if the operation succeed, 1 if the indices cannot be used, 2 if the file cannot be found, -1 if an unknown error occurs 
int32 Engine::newCShader(LuxArray<uint64> bufferIndices, const char* shaderPath) {
	if (bufferIndices.size() > CBuffers.size()) return -1;
	CShader_create_descriptorSetLayouts(bufferIndices);
	CShader_create_descriptorSets(bufferIndices);
	CShader_create_CPipeline();
	CShader_create_commandBuffer();
	return 0;
}




//This function creates a buffer in a compute device and saves it in the LuxArray "computeBuffers"
//*   size: the size in bytes of the buffer
//*   Returns the buffer's index in the array. -1 if an error occurs
uint64 Engine::createComputeBuffer(uint32 size){
	LuxGpuBuffer buffer_;
	buffer_.size = size;
	createBuffer(compute.LD, buffer_.size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, buffer_.buffer, buffer_.memory);
	void* data;
	//vkMapMemory(compute.LD, buffer_.memory, 0, buffer_.size, 0, &data);

	return buffer_.ID = CBuffers.add(buffer_);
}
