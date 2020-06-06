
#include "LuxEngine/Engine/Engine.h"



void Engine::runGraphics(bool _useVSync, float _FOV) {
	useVSync = _useVSync;
	FOV = _FOV;
	stdTime start = now;

	if (enableValidationLayers) { Failure printf("D E B U G    M O D E"); }				MainSeparator;
	Normal  printf("Initializing Vulkan");							initVulkan();		MainSeparator;
	Success printf("Initialization completed in %f s", (sc<stdDuration>(now - start)).count());
}




void Engine::initVulkan() {
	//Initialize vulkan
	Normal printf("    Creating VK Surface...               ");		createSurface();					SuccessNoNl printf("ok");	NewLine;
	Normal printf("    Searching for physical devices...    ");		getPhysicalDevices();				NewLine;
	Normal printf("    Creating VK command pool...          ");		createGraphicsCommandPool();		SuccessNoNl printf("ok");
	/**/												 			setupDebugMessenger();

	//Create textures
	bufferSize = sizeof(Pixel) * COMPUTE_WIDTH * COMPUTE_HEIGHT;	//Set the size of the buffer
	createBuffer(compute.LD, bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, buffer, bufferMemory);
	createTextureImage();
	createTextureImageView();
	createTextureSampler();

	//Create an object for the render
	createVertexBuffer();
	createIndexBuffer();

	//Create swapchain render components
	createDescriptorSetLayout();
	Normal printf("    Creating VK swapchain...             ");		createSwapChain();					SuccessNoNl printf("ok");
	createSyncObjects();
	createDrawCommandBuffers();
}




void Engine::createSurface() {
	Try(glfwCreateWindowSurface(instance, window, nullptr, &surface)) Quit("Failed to create window surface");
}




void Engine::createSyncObjects() {
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (int64 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(graphics.LD, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(graphics.LD, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(graphics.LD, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
			Quit("Failed to create synchronization objects for a frame");
		}
	}
}




void Engine::setupDebugMessenger() {
	if (!enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	populateDebugMessengerCreateInfo(createInfo);
	Try(CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger)) Quit("Failed to set up debug messenger");
}




void Engine::drawFrame() {
	//Wait fences
	vkWaitForFences(graphics.LD, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	//Update render result
	runCommandBuffer();
	transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	copyBufferToImage(buffer, textureImage, WIDTH, HEIGHT);
	transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	//Acquire swapchain image
	uint32 imageIndex;
	VkResult result = vkAcquireNextImageKHR(graphics.LD, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR) { recreateSwapChain(); return; }
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) Quit("Failed to acquire swapchain image");

	if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) vkWaitForFences(graphics.LD, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
	imagesInFlight[imageIndex] = inFlightFences[currentFrame];




	static VkSubmitInfo submitInfo{};
	static VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	static VkPresentInfoKHR presentInfo{};
	static VkSwapchainKHR swapChains[] = { swapChain };

	static bool staticInit = true;
	if (staticInit) {
		staticInit = false;
		//Submit to queue
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.commandBufferCount = 1;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pWaitDstStageMask = waitStages;

		//Present
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;

	}

	//Submit to queue
	submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
	submitInfo.pWaitSemaphores = waitSemaphores;
	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(graphics.LD, 1, &inFlightFences[currentFrame]);
	Try(vkQueueSubmit(graphics.graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame])) Quit("Failed to submit draw command buffer");


	//Present
	presentInfo.pWaitSemaphores = signalSemaphores;
	presentInfo.pImageIndices = &imageIndex;

	result = vkQueuePresentKHR(graphics.presentQueue, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
		framebufferResized = false;
		recreateSwapChain();
		vkDeviceWaitIdle(graphics.LD);
	}
	else Try(result) Quit("Failed to present swapchain image");

	//Update frame number
	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}




void Engine::framebufferResizeCallback(GLFWwindow* window, int32 width, int32 height) {
	auto engine = reinterpret_cast<Engine*>(glfwGetWindowUserPointer(window));
	engine->framebufferResized = true;
}




void Engine::cleanupGraphics() {
	cleanupSwapChain();																//Clear swapchain components

	vkDestroySampler(graphics.LD, textureSampler, nullptr);							//Destroy sampler
	vkDestroyDescriptorSetLayout(graphics.LD, descriptorSetLayout, nullptr);		//Destroy descriptor set layout
	vkDestroyCommandPool(graphics.LD, graphicsCommandPool, nullptr);				//Destroy graphics command pool

	vkDestroyImage(graphics.LD, textureImage, nullptr);								//Destroy texture image
	vkDestroyImageView(graphics.LD, textureImageView, nullptr);						//Destroy texture image view
	vkFreeMemory(graphics.LD, textureImageMemory, nullptr);							//Free texture image memory

	vkDestroyBuffer(graphics.LD, vertexBuffer, nullptr);							//Destroy vertex buffer
	vkFreeMemory(graphics.LD, vertexBufferMemory, nullptr);							//Free vertex buffer memory
	vkDestroyBuffer(graphics.LD, indexBuffer, nullptr);								//Destroy index buffer
	vkFreeMemory(graphics.LD, indexBufferMemory, nullptr);							//Free index buffer memory

	for (int64 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {								//For every frame
		vkDestroySemaphore(graphics.LD, renderFinishedSemaphores[i], nullptr);			//Destroy his render semaphore
		vkDestroySemaphore(graphics.LD, imageAvailableSemaphores[i], nullptr);			//Destroy his image  semaphore
		vkDestroyFence(graphics.LD, inFlightFences[i], nullptr);						//Destroy his fence
	}


	if (graphics.PD.properties.deviceID != compute.PD.properties.deviceID) vkDestroyDevice(graphics.LD, nullptr);	//If the compute and the graphics devices are not the same, destroy the graphics device
	vkDestroyDevice(compute.LD, nullptr);																			//Destroy the compute device
	for (auto device : secondary) vkDestroyDevice(device.LD, nullptr);													//Destroy all the secondary devices

	if (enableValidationLayers) DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);					//Destroy the debug messenger if present
	vkDestroySurfaceKHR(instance, surface, nullptr);																//Destroy the vulkan surface
}




// Other ------------------------------------------------------------------------------------------------------------------------------------//




VkFormat Engine::findSupportedFormat(LuxArray<VkFormat> candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
	for (VkFormat format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(graphics.PD.device, format, &props);

		if ((tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) ||
			(tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)) {
			return format;
		}
	}
	Quit("Failed to find a supported format");
}




//Returns the index of the memory with the specified type and properties. Exits if not found
uint32 Engine::findMemoryType(uint32 typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;							//Get memory properties
	vkGetPhysicalDeviceMemoryProperties(graphics.PD.device, &memProperties);

	for (uint32 i = 0; i < memProperties.memoryTypeCount; i++) {			//Search for the memory that has the specified properties and type and return its index
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) return i;
	}
	Quit("Failed to find suitable memory type");
}




//Creates and submits a command buffer to copy from srcBuffer to dstBuffer
void Engine::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
	VkBufferCopy copyRegion{};												//Create buffer copy object
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();				//Start command buffer
	copyRegion.size = size;													//Set size of the copied region
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);	//Record the copy command
	endSingleTimeCommands(commandBuffer);									//End command buffer
}




VKAPI_ATTR VkBool32 VKAPI_CALL Engine::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
}
