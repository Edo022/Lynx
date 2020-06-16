
#include "LuxEngine/Engine/Engine.h"


void Engine::runGraphics(bool _useVSync, float _FOV) {
	useVSync = _useVSync;
	FOV = _FOV;
	LuxTime start = luxGetTime();

	luxDebug(Failure printf("D E B U G    M O D E"));													MainSeparator;
	Normal  printf("Initializing Vulkan");							initVulkan();						MainSeparator;
	Success printf("Initialization completed in %f s", luxTimeGetDuration(start));
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

	for (int64 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
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
	vkWaitForFences(graphics.LD, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
	if (framebufferResized) goto recreateSwapchain_;
	



	//Acquire swapchain image
	uint32 imageIndex;
	switch (vkAcquireNextImageKHR(graphics.LD, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex)) {
		case VK_SUBOPTIMAL_KHR: case VK_SUCCESS: break;
		case VK_ERROR_OUT_OF_DATE_KHR: recreateSwapChain(); return;
		default: Exit("Failed to acquire swapchain image");
	}
	if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) vkWaitForFences(graphics.LD, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
	imagesInFlight[imageIndex] = inFlightFences[currentFrame];




	static VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	//Update render result submitting the command buffers to the compute queue
	static VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &imageAvailableSemaphores[currentFrame];
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &renderFinishedSemaphores[currentFrame];
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &CShaders[0].commandBuffers[imageIndex];
	submitInfo.pWaitDstStageMask = waitStages;

	vkResetFences(graphics.LD, 1, &inFlightFences[currentFrame]);
	TryVk(vkQueueSubmit(graphics.graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame])) Exit("Failed to submit graphics command buffer");


	//Present
	static VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &renderFinishedSemaphores[currentFrame];
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapChain;
	presentInfo.pImageIndices = &imageIndex;

	switch (vkQueuePresentKHR(graphics.presentQueue, &presentInfo)) { 
		case VK_SUCCESS: break;
		case VK_ERROR_OUT_OF_DATE_KHR: case VK_SUBOPTIMAL_KHR: {
			recreateSwapchain_:
			framebufferResized = false;
			recreateSwapChain();
			vkDeviceWaitIdle(graphics.LD);
			luxDebug(printf("Recreated swapchain\n"));
			break;
		}
		default: Exit("Failed to present swapchain image");
	}

	//Update frame number
	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	glfwSwapBuffers(window);
}




void Engine::framebufferResizeCallback(GLFWwindow* window, int32 width, int32 height) {
	auto engine = reinterpret_cast<Engine*>(glfwGetWindowUserPointer(window));
	engine->framebufferResized = true;
}




void Engine::cleanupGraphics() {
	cleanupSwapChain();																//Clear swapchain components
	vkDestroyCommandPool(graphics.LD, graphicsCommandPool, nullptr);				//Destroy graphics command pool

	for (int64 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {								//For every frame
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




VkFormat Engine::findSupportedFormat(LuxArray<VkFormat> candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
	for (VkFormat format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(graphics.PD.device, format, &props);

		if ((tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) ||
			(tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)) {
			return format;
		}
	}
	Exit("Failed to find a supported format");
}




//Returns the index of the memory with the specified type and properties. Exits if not found
uint32 Engine::findMemoryType(uint32 typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;							//Get memory properties
	vkGetPhysicalDeviceMemoryProperties(graphics.PD.device, &memProperties);

	for (uint32 i = 0; i < memProperties.memoryTypeCount; i++) {			//Search for the memory that has the specified properties and type and return its index
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) return i;
	}
	Exit("Failed to find suitable memory type");
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
