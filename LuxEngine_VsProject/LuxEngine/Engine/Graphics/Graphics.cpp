

#include <vulkan/vulkan.h>
#include "LuxEngine/Engine/Engine.h"
#include "LuxEngine/Engine/Compute/CShader_t.h"





	void Engine::runGraphics(const bool vUseVSync, const float vFOV) {
		useVSync = vUseVSync;
		FOV = vFOV;

		luxDebug(Failure printf("D E B U G    M O D E"));													MainSeparator;
		Normal  printf("Initializing Vulkan");							graphicsInitVulkan( );				MainSeparator;
	}




	void Engine::graphicsInitVulkan( ) {
		//Initialize vulkan
		Normal printf("    Creating VK Surface...               ");		graphicsCreateSurface( );			SuccessNoNl printf("ok");	NewLine;
		Normal printf("    Searching for physical devices...    ");		deviceGetPhysical( );											NewLine;
		Normal printf("    Creating VK command pool...          ");		createGraphicsCommandPool( );		SuccessNoNl printf("ok");
		Normal printf("    Creating VK swapchain...             ");		swapchainCreate( );					SuccessNoNl printf("ok");

		luxDebug(graphicsCreateDebugMessenger( ));
		graphicsCreateFences( );
	}




	inline void Engine::graphicsCreateSurface( ) {
		TryVk(glfwCreateWindowSurface(instance, window, nullptr, &surface)) Exit("Failed to create window surface");
	}


	void Engine::graphicsCreateDebugMessenger( ) {
		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		lux::_engine::populateDebugMessengerCreateInfo(createInfo);
		TryVk(lux::_engine::CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger)) Exit("Failed to set up debug messenger");
	}





	void Engine::graphicsCreateFences( ) {
		drawFrameImageAquiredSemaphore.resize(renderMaxFramesInFlight);
		drawFrameObjectsRenderedSemaphore.resize(renderMaxFramesInFlight);
		drawFrameCopySemaphore.resize(renderMaxFramesInFlight);
		drawFrameClearSemaphore.resize(renderMaxFramesInFlight);
		drawFrameImageRenderedFence.resize(renderMaxFramesInFlight);
		//renderFencesImagesInFlight.resize(swapchainImages.size( ), VK_NULL_HANDLE);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for(int32 i = 0; i < renderMaxFramesInFlight; ++i) {
			if(vkCreateSemaphore(graphics.LD, &semaphoreInfo, nullptr, &drawFrameImageAquiredSemaphore[i]) != VK_SUCCESS ||
				vkCreateSemaphore(graphics.LD, &semaphoreInfo, nullptr, &drawFrameObjectsRenderedSemaphore[i]) != VK_SUCCESS ||
				vkCreateSemaphore(graphics.LD, &semaphoreInfo, nullptr, &drawFrameCopySemaphore[i]) != VK_SUCCESS ||
				vkCreateSemaphore(graphics.LD, &semaphoreInfo, nullptr, &drawFrameClearSemaphore[i]) != VK_SUCCESS ||
				vkCreateFence(graphics.LD, &fenceInfo, nullptr, &drawFrameImageRenderedFence[i]) != VK_SUCCESS
				){
				Exit("Failed to create synchronization objects for a frame");
			}
		//VkSemaphoreSignalInfo signalInfo{ };
		//signalInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO;
		//signalInfo.semaphore = drawFrameClearSemaphore[i];
		//signalInfo.value = 1;
		//vkSignalSemaphore(graphics.LD, new VkSemaphoreSignalInfo(signalInfo));
		}
	}






	//TODO multithreaded submit and command creation
	void Engine::graphicsDrawFrame( ) {
			if(CShaders.usedSize( ) <= 1) return;
		vkWaitForFences(graphics.LD, 1, &drawFrameImageRenderedFence[renderCurrentFrame], false, INT_MAX);
		redraw:

		//Wait fences
		//sleep(500);
		//VkSemaphoreWaitInfo waitInfos{ };
		//waitInfos.semaphoreCount = 1;
		//waitInfos.pSemaphores = &drawFrameClearSemaphore[renderCurrentFrame];
		//waitInfos.flags = 0;
		//uint64 value = 1;
		//waitInfos.pValues = &value;
		//vkWaitSemaphores(graphics.LD, &waitInfos, 1000*1000*5);
		if(renderFramebufferResized) {
			renderFramebufferResized = false;
			swapchainRecreate(true);
			goto redraw;
		}
		//sleep(2000);

		uint32 imageIndex;
		{ //Acquire swapchain image
			switch(vkAcquireNextImageKHR(graphics.LD, swapchain, /*1000*1000*5*/INT_MAX /*5s*/, drawFrameImageAquiredSemaphore[renderCurrentFrame], VK_NULL_HANDLE, &imageIndex)) {
				case VK_SUCCESS: case VK_SUBOPTIMAL_KHR: break;
				case VK_ERROR_OUT_OF_DATE_KHR: swapchainRecreate(false);  return;
				default: Failure printf("Failed to aquire swapchain image");
			}
			//if(renderFencesImagesInFlight[imageIndex] != VK_NULL_HANDLE) vkWaitForFences(graphics.LD, 1, &renderFencesImagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
			//renderFencesImagesInFlight[imageIndex] = drawFrameImageRenderedFence[renderCurrentFrame];
		}
		//sleep(2000);



		//TODO don't recreate the command buffer array every time
		static VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_ALL_COMMANDS_BIT };
		//static VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT };
		{ //Update render result submitting the command buffers to the compute queues
			//static VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
			addShaderFence.startFirst( );
			lux::Array<VkCommandBuffer> commandBuffers(CShaders.usedSize( ) - 1);
			//sleep(2000);
			//lux::Array<VkCommandBuffer> commandBuffers(CShaders.usedSize( ) + 2);
			{
				//commandBuffers[0] = clearCommandBuffer;
				for(uint32 i = 0; i < CShaders.size( ) - 1; ++i) {
					//TODO dont use __lp_data
					if(CShaders.isValid(i)) commandBuffers[i] = CShaders[i].commandBuffers.__lp_data[0];
				}
				//commandBuffers[commandBuffers.size( ) - 1] = copyCommandBuffers[imageIndex];
			}
				addShaderFence.endFirst( );
			static VkSubmitInfo submitInfo{ };
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = &drawFrameImageAquiredSemaphore[renderCurrentFrame];
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &drawFrameObjectsRenderedSemaphore[renderCurrentFrame];
			submitInfo.commandBufferCount = commandBuffers.size( );
			submitInfo.pCommandBuffers = commandBuffers.begin( );
			submitInfo.pWaitDstStageMask = waitStages;

			//vkResetFences(graphics.LD, 1, &drawFrameImageRenderedFence[renderCurrentFrame]);
			TryVk(vkQueueSubmit(graphics.graphicsQueue, 1, &submitInfo, nullptr)) Exit("Failed to submit graphics command buffer");
			//TryVk(vkQueueSubmit(graphics.graphicsQueue, 1, &submitInfo, drawFrameImageRenderedFence[renderCurrentFrame])) Exit("Failed to submit graphics command buffer");
		}
		//vkWaitForFences(graphics.LD, 1, &drawFrameImageRenderedFence[renderCurrentFrame], VK_TRUE, UINT64_MAX);
		//sleep(50);




		{ //Convert and clear
		//vkWaitForFences(graphics.LD, 1, &drawFrameImageRenderedFence[renderCurrentFrame], VK_TRUE, UINT64_MAX);
			//static VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
			//static VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT };

			static VkSubmitInfo submitInfo{ };
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = &drawFrameObjectsRenderedSemaphore[renderCurrentFrame];
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &drawFrameClearSemaphore[renderCurrentFrame];
			submitInfo.commandBufferCount = 1;
			//submitInfo.pCommandBuffers = &clearCommandBuffer;
			submitInfo.pCommandBuffers = &CShaders[0].commandBuffers[0];
			submitInfo.pWaitDstStageMask = waitStages;

			//vkResetFences(graphics.LD, 1, &drawFrameImageRenderedFence[renderCurrentFrame]);
			TryVk(vkQueueSubmit(graphics.graphicsQueue, 1, &submitInfo, nullptr)) Exit("Failed to submit graphics command buffer");
			//TryVk(vkQueueSubmit(graphics.graphicsQueue, 1, &submitInfo, drawFrameImageRenderedFence[renderCurrentFrame])) Exit("Failed to submit graphics command buffer");
		}




		//TODO don't recreate the command buffer array every time
		{ //Copy
			//static VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
			//static VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT };

			static VkSubmitInfo submitInfo{ };
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = &drawFrameClearSemaphore[renderCurrentFrame];
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &drawFrameCopySemaphore[renderCurrentFrame];
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &copyCommandBuffers[imageIndex];
			submitInfo.pWaitDstStageMask = waitStages;

			vkResetFences(graphics.LD, 1, &drawFrameImageRenderedFence[renderCurrentFrame]);
			//TryVk(vkQueueSubmit(graphics.graphicsQueue, 1, &submitInfo, nullptr)) Exit("Failed to submit graphics command buffer");
			TryVk(vkQueueSubmit(graphics.graphicsQueue, 1, &submitInfo, drawFrameImageRenderedFence[renderCurrentFrame])) Exit("Failed to submit graphics command buffer");
		}
				//sleep(50);



		//TODO don't recreate the command buffer array every time
				//sleep(50);




		{ //Present
			static VkPresentInfoKHR presentInfo{};

			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentInfo.waitSemaphoreCount = 1;
			//presentInfo.pWaitSemaphores = &drawFrameObjectsRenderedSemaphore[renderCurrentFrame];
			//presentInfo.pWaitSemaphores = &drawFrameClearSemaphore[renderCurrentFrame];
			presentInfo.pWaitSemaphores = &drawFrameCopySemaphore[renderCurrentFrame];
			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = &swapchain;
			presentInfo.pImageIndices = &imageIndex;

			switch(vkQueuePresentKHR(graphics.presentQueue, &presentInfo)) {
				case VK_SUCCESS:  break;
				case VK_ERROR_OUT_OF_DATE_KHR: case VK_SUBOPTIMAL_KHR: {
					swapchainRecreate(false);
					vkDeviceWaitIdle(graphics.LD);
					break;
				}
				default:  Exit("Failed to present swapchain image");
			}

		}

		//Update frame number, flush window and wait for the image fence
		renderCurrentFrame = (renderCurrentFrame + 1) % (renderMaxFramesInFlight);
		glfwSwapBuffers(window);
	}




	void Engine::framebufferResizeCallback(GLFWwindow* pWindow, int32 vWidth, int32 vHeight) {
		lux::getEngine().windowResizeFence.wait(0);  //from the last call of this function

		lux::getEngine().renderFramebufferResized = true;
		lux::getEngine().windowResizeFence.set(1);

		lux::getEngine().windowResizeFence.wait(2);  //from RecreateSwapchain()
		lux::getEngine().windowResizeFence.set(0);
	}




	void Engine::graphicsCleanup( ) {
		swapchainCleanup( );																//Clear swapchain components
		vkDestroyCommandPool(graphics.LD, singleTimeCommandPool, nullptr);				//Destroy graphics command pool

		for(int32 i = 0; i < renderMaxFramesInFlight; ++i) {								//For every frame
			vkDestroySemaphore(graphics.LD, drawFrameObjectsRenderedSemaphore[i], nullptr);			//Destroy his render semaphore
			vkDestroySemaphore(graphics.LD, drawFrameImageAquiredSemaphore[i], nullptr);			//Destroy his image  semaphore
			//vkDestroyFence(graphics.LD, drawFrameImageRenderedFence[i], nullptr);						//Destroy his fence
		}


		if(graphics.PD.properties.deviceID != compute.PD.properties.deviceID) vkDestroyDevice(graphics.LD, nullptr);	//If the compute and the graphics devices are not the same, destroy the graphics device
		vkDestroyDevice(compute.LD, nullptr);																			//Destroy the compute device
		//for (auto& device : secondary) vkDestroyDevice(device.LD, nullptr);											//Destroy all the secondary devices

		luxDebug(lux::_engine::DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr));						//Destroy the debug messenger if present
		vkDestroySurfaceKHR(instance, surface, nullptr);																//Destroy the vulkan surface
	}








	// Other ------------------------------------------------------------------------------------------------------------------------------------//








	VkFormat Engine::graphicsFindSupportedFormat(const lux::Array<VkFormat>* pCandidates, const VkImageTiling vTiling, const VkFormatFeatureFlags vFeatures) {
		for(VkFormat format : *pCandidates) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(graphics.PD.device, format, &props);

			if((vTiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & vFeatures) == vFeatures) ||
				(vTiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & vFeatures) == vFeatures)) {
				return format;
			}
		}
		Exit("Failed to find a supported format");
	}




	//Returns the index of the memory with the specified type and properties. Exits if not found
	uint32 Engine::graphicsFindMemoryType(const uint32 vTypeFilter, const VkMemoryPropertyFlags vProperties) {
		VkPhysicalDeviceMemoryProperties memProperties;							//Get memory vProperties
		vkGetPhysicalDeviceMemoryProperties(graphics.PD.device, &memProperties);

		for(uint32 i = 0; i < memProperties.memoryTypeCount; ++i) {			//Search for the memory that has the specified properties and type and return its index
			if((vTypeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & vProperties) == vProperties) return i;
		}
		Exit("Failed to find suitable memory type");
	}




	VKAPI_ATTR VkBool32 VKAPI_CALL Engine::graphicsDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
		if(messageSeverity | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) Failure
		else if(messageSeverity | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) Warning
		printf("Validation layer error\n %s", pCallbackData->pMessage);
		return VK_FALSE;
	}
