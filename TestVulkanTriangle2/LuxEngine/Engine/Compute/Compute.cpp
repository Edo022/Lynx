
#include "LuxEngine/Engine/Engine.h"



//Dark magic. Don't worry, it works
static VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallbackFn(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64 object, uint64 location, int32 messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData) {
	printf("Debug Report: %s: %s\n", pLayerPrefix, pMessage);
	return VK_FALSE;
}




void Engine::runCompute() {
	uint64 imageOutput = createGpuBuffer(sizeof(Pixel) * COMPUTE_WIDTH * COMPUTE_HEIGHT);
	uint64 vertices = createGpuBuffer(4);
	uint32* mappedVertices = (uint32*)mapGpuBuffer(&CGpuBuffers[1]); mappedVertices[1] = 1;
	newCShader({ imageOutput, vertices }, "LuxEngine/Contents/shaders/comp.spv");
	//CShader_create_descriptorSetLayouts();
	//CShader_create_descriptorSets();
	//CShader_create_CPipeline();
	//CShader_create_commandBuffer();
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
//*   bufferIndices: the indices of the buffers to bind. Each index must correspond to a CGpuBuffers's element
//*   returns the index of the created shader if the operation succeed, -1 if the indices cannot be used, -2 if the file cannot be found, -3 if an unknown error occurs 
int32 Engine::newCShader(LuxArray<uint64> bufferIndices, const char* shaderPath) {
	if (bufferIndices.size() > CGpuBuffers.size()) return -1;

	uint64 shaderIndex = CShaders.add(LuxCShader{});
	CShader_create_descriptorSetLayouts(bufferIndices, shaderIndex);
	CShader_create_descriptorSets(bufferIndices, shaderIndex);
	CShader_create_CPipeline(shaderPath, shaderIndex);
	CShader_create_commandBuffer(shaderIndex);

	return shaderIndex;
}




//This function creates a buffer in a compute device and saves it in the LuxArray "computeBuffers"
//*   size: the size in bytes of the buffer
//*   Returns the buffer's index in the array. -1 if an error occurs
uint64 Engine::createGpuBuffer(uint32 size){
	LuxGpuBuffer buffer_;
	buffer_.size = size;
	createBuffer(compute.LD, buffer_.size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, buffer_.buffer, buffer_.memory);
	void* data;
	return buffer_.ID = CGpuBuffers.add(buffer_);
}
