
#include "LuxEngine/Engine/Engine.h"


void Engine::runGraphics(const bool vUseVSync, const float vFOV) {
	useVSync = vUseVSync;
	FOV = vFOV;

	luxDebug(Failure printf("D E B U G    M O D E"));													MainSeparator;
	Normal  printf("Initializing Vulkan");							initVulkan();						MainSeparator;
}




void Engine::initVulkan() {
	//Initialize vulkan
	Normal printf("    Creating VK Surface...               ");		createSurface();					SuccessNoNl printf("ok");	NewLine;
	Normal printf("    Searching for physical devices...    ");		getPhysicalDevices();											NewLine;
	Normal printf("    Creating VK command pool...          ");		createGraphicsCommandPool();		SuccessNoNl printf("ok");
	Normal printf("    Creating VK swapchain...             ");		createSwapChain();					SuccessNoNl printf("ok");

	luxDebug(createDebugMessenger());
	createSyncObjects();
}




inline void Engine::createSurface() {
	TryVk(glfwCreateWindowSurface(instance, window, nullptr, &surface)) Exit("Failed to create window surface");
}


void Engine::createDebugMessenger() {
	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	populateDebugMessengerCreateInfo(createInfo);
	TryVk(CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger)) Exit("Failed to set up debug messenger");
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

	for (int64 i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		if (vkCreateSemaphore(graphics.LD, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(graphics.LD, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(graphics.LD, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
			Exit("Failed to create synchronization objects for a frame");
		}
	}
}







void Engine::drawFrame() {
	//TODO create separated command buffer

	//Wait fences
	vkWaitForFences(graphics.LD, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX); Failure printf(">> %s at line %d", __func__, __LINE__);
	if (framebufferResized) goto recreateSwapchain_; Failure printf(">> %s at line %d", __func__, __LINE__);



	//Acquire swapchain image
	uint32 imageIndex;
	switch (vkAcquireNextImageKHR(graphics.LD, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex)) {
		case VK_SUBOPTIMAL_KHR: case VK_SUCCESS: Failure printf(">> %s at line %d", __func__, __LINE__); break;
		case VK_ERROR_OUT_OF_DATE_KHR: recreateSwapChain(); Failure printf(">> %s at line %d", __func__, __LINE__); return;
		default:  Failure printf(">> %s at line %d", __func__, __LINE__);Exit("Failed to acquire swapchain image");
	}
	if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) vkWaitForFences(graphics.LD, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX); Failure printf(">> %s at line %d", __func__, __LINE__);
	imagesInFlight[imageIndex] = inFlightFences[currentFrame]; Failure printf(">> %s at line %d", __func__, __LINE__);




	static VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	//Update render result submitting the command buffers to the compute queue
	static VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO; Failure printf(">> %s at line %d", __func__, __LINE__);
	submitInfo.waitSemaphoreCount = 1; Failure printf(">> %s at line %d", __func__, __LINE__);
	submitInfo.pWaitSemaphores = &imageAvailableSemaphores[currentFrame]; Failure printf(">> %s at line %d", __func__, __LINE__);
	submitInfo.signalSemaphoreCount = 1; Failure printf(">> %s at line %d", __func__, __LINE__);
	submitInfo.pSignalSemaphores = &renderFinishedSemaphores[currentFrame]; Failure printf(">> %s at line %d", __func__, __LINE__);
	submitInfo.commandBufferCount = 1; Failure printf(">> %s at line %d", __func__, __LINE__);
	submitInfo.pCommandBuffers = &CShaders[0].commandBuffers[imageIndex]; Failure printf(">> %s at line %d", __func__, __LINE__);
	submitInfo.pWaitDstStageMask = waitStages; Failure printf(">> %s at line %d", __func__, __LINE__);

	vkResetFences(graphics.LD, 1, &inFlightFences[currentFrame]); Failure printf(">> %s at line %d", __func__, __LINE__);
	TryVk(vkQueueSubmit(graphics.graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame])) Exit("Failed to submit graphics command buffer");
	 Failure printf(">> %s at line %d", __func__, __LINE__);

	//Present
	static VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR; Failure printf(">> %s at line %d", __func__, __LINE__);
	presentInfo.waitSemaphoreCount = 1; Failure printf(">> %s at line %d", __func__, __LINE__);
	presentInfo.pWaitSemaphores = &renderFinishedSemaphores[currentFrame]; Failure printf(">> %s at line %d", __func__, __LINE__);
	presentInfo.swapchainCount = 1; Failure printf(">> %s at line %d", __func__, __LINE__);
	presentInfo.pSwapchains = &swapChain; Failure printf(">> %s at line %d", __func__, __LINE__);
	presentInfo.pImageIndices = &imageIndex; Failure printf(">> %s at line %d", __func__, __LINE__);

	switch (vkQueuePresentKHR(graphics.presentQueue, &presentInfo)) { 
		case VK_SUCCESS:  Failure printf(">> %s at line %d", __func__, __LINE__);break;
		case VK_ERROR_OUT_OF_DATE_KHR: case VK_SUBOPTIMAL_KHR: {
			recreateSwapchain_:
			framebufferResized = false; Failure printf(">> %s at line %d", __func__, __LINE__);
			recreateSwapChain(); Failure printf(">> %s at line %d", __func__, __LINE__);
			vkDeviceWaitIdle(graphics.LD); Failure printf(">> %s at line %d", __func__, __LINE__);
			break;
		}
		default:  Failure printf(">> %s at line %d", __func__, __LINE__);Exit("Failed to present swapchain image");
	}

	//Update frame number
	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT; Failure printf(">> %s at line %d", __func__, __LINE__);
	glfwSwapBuffers(window); Failure printf(">> %s at line %d", __func__, __LINE__);
}




void Engine::framebufferResizeCallback(GLFWwindow* pWindow, int32 vWidth, int32 vHeight) {
	engine.windowResizeFence.wait(0); Failure printf(">> %s at line %d", __func__, __LINE__); //from the last call of this function

	engine.framebufferResized = true; Failure printf(">> %s at line %d", __func__, __LINE__);
	engine.windowResizeFence.set(1); Failure printf(">> %s at line %d", __func__, __LINE__);

	engine.windowResizeFence.wait(2); Failure printf(">> %s at line %d", __func__, __LINE__); //from RecreateSwapchain()
	engine.windowResizeFence.set(0); Failure printf(">> %s at line %d", __func__, __LINE__);
}




void Engine::cleanupGraphics() {
	cleanupSwapChain();																//Clear swapchain components
	vkDestroyCommandPool(graphics.LD, graphicsCommandPool, nullptr);				//Destroy graphics command pool

	for (int64 i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {								//For every frame
		vkDestroySemaphore(graphics.LD, renderFinishedSemaphores[i], nullptr);			//Destroy his render semaphore
		vkDestroySemaphore(graphics.LD, imageAvailableSemaphores[i], nullptr);			//Destroy his image  semaphore
		vkDestroyFence(graphics.LD, inFlightFences[i], nullptr);						//Destroy his fence
	}


	if (graphics.PD.properties.deviceID != compute.PD.properties.deviceID) vkDestroyDevice(graphics.LD, nullptr);	//If the compute and the graphics devices are not the same, destroy the graphics device
	vkDestroyDevice(compute.LD, nullptr);																			//Destroy the compute device
	for (auto device : secondary) vkDestroyDevice(device.LD, nullptr);													//Destroy all the secondary devices

	luxDebug(DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr));					//Destroy the debug messenger if present
	vkDestroySurfaceKHR(instance, surface, nullptr);																//Destroy the vulkan surface
}




// Other ------------------------------------------------------------------------------------------------------------------------------------//




VkFormat Engine::findSupportedFormat(const LuxArray<VkFormat>* pCandidates, const VkImageTiling vTiling, const VkFormatFeatureFlags vFeatures) {
	for (VkFormat format : *pCandidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(graphics.PD.device, format, &props);

		if ((vTiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & vFeatures) == vFeatures) ||
			(vTiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & vFeatures) == vFeatures)) {
			return format;
		}
	}
	Exit("Failed to find a supported format");
}




//Returns the index of the memory with the specified type and properties. Exits if not found
uint32 Engine::findMemoryType(const uint32 vTypeFilter, const VkMemoryPropertyFlags vProperties) {
	VkPhysicalDeviceMemoryProperties memProperties;							//Get memory vProperties
	vkGetPhysicalDeviceMemoryProperties(graphics.PD.device, &memProperties);

	for (uint32 i = 0; i < memProperties.memoryTypeCount; ++i) {			//Search for the memory that has the specified properties and type and return its index
		if ((vTypeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & vProperties) == vProperties) return i;
	}
	Exit("Failed to find suitable memory type");
}




//Creates and submits a command buffer to copy from vSrcBuffer to dstBuffer
void Engine::copyBuffer(const VkBuffer vSrcBuffer, const VkBuffer vDstBuffer, const VkDeviceSize vSize) {
	VkBufferCopy copyRegion{};												//Create buffer copy object
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();				//Start command buffer
	copyRegion.size = vSize;												//Set size of the copied region
	vkCmdCopyBuffer(commandBuffer, vSrcBuffer, vDstBuffer, 1, &copyRegion);	//Record the copy command
	endSingleTimeCommands(commandBuffer);									//End command buffer
}




VKAPI_ATTR VkBool32 VKAPI_CALL Engine::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
}
