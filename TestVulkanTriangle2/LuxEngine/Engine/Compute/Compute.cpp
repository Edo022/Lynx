
#include "LuxEngine/Engine/Engine.h"



//Dark magic. Don't worry, it works
static VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallbackFn(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64 object, uint64 location, int32 messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData) {
	printf("Debug Report: %s: %s\n", pLayerPrefix, pMessage);
	return VK_FALSE;
}




void Engine::RunCompute() {
	bufferSize1 = 4;
	createBuffer(compute.LD, bufferSize1, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, buffer1, bufferMemory1);

	void* data;
	vkMapMemory(compute.LD, bufferMemory1, 0, bufferSize1, 0, &data);
	((uint32*)data)[0] = 1;
	vkUnmapMemory(compute.LD, bufferMemory1);

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
