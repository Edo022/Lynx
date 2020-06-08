
#include "LuxEngine/Engine/Engine.h"



//Dark magic. Don't worry, it works
static VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallbackFn(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64 object, uint64 location, int32 messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData) {
	printf("Debug Report: %s: %s\n", pLayerPrefix, pMessage);
	return VK_FALSE;
}




void Engine::RunCompute() {

	createComputeBuffer(4);
	void* data;
	vkMapMemory(compute.LD, CBuffers[0].memory, 0, CBuffers[0].size, 0, &data);
	((uint32*)data)[0] = 1;
	vkUnmapMemory(compute.LD, CBuffers[0].memory);

	createComputeDescriptorSetLayout();
	createDescriptorSet();
	createComputePipeline();
	createComputeCommandBuffer();
}




void Engine::cleanupCompute() {
	vkDestroyBuffer(compute.LD, buffer, null);										//Destroy the buffer
	vkFreeMemory(compute.LD, bufferMemory, null);									//Free the buffer's memory

	vkDestroyDescriptorPool(compute.LD, computeDescriptorPool, null);				//Destroy the descriptor pool
	vkDestroyDescriptorSetLayout(compute.LD, computeDescriptorSetLayout, null);		//Destroy the descriptor set layout

	vkDestroyPipeline(compute.LD, computePipeline, null);							//Destroy the pipeline
	vkDestroyPipelineLayout(compute.LD, computePipelineLayout, null);				//Destroy the pipeline layout

	vkDestroyCommandPool(compute.LD, computeCommandPool, null);						//Destroy the compute command pool
}




// Utilities --------------------------------------------------------------------------------------------------------------------------------//




//This function creates a buffer in a compute device and saves it in the LuxArray "computeBuffers"
//*   size: the size in bytes of the buffer
//*   Returns the buffer's index in the array. -1 if an error occurs
int64 Engine::createComputeBuffer(uint32 size){
	LuxGpuBuffer buffer_;
	buffer_.size = size;
	createBuffer(compute.LD, buffer_.size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, buffer_.buffer, buffer_.memory);
	void* data;
	//vkMapMemory(compute.LD, buffer_.memory, 0, buffer_.size, 0, &data);

	return buffer_.ID = CBuffers.add(buffer_);
}
