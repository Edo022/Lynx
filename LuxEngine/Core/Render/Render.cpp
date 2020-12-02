#include <vulkan/vulkan.h>
#include "LuxEngine/Core/Render/CShader_t.hpp"
#include "LuxEngine/Core/Render/Render.hpp"
#include "LuxEngine/Core/Render/GCommands.hpp"
#include "LuxEngine/Core/Render/GSwapchain.hpp"
#include "LuxEngine/Core/Render/CShader.hpp"
#include "LuxEngine/Core/Devices.hpp"
#include "LuxEngine/Types/LuxObject/LuxObject.hpp"
#include "LuxEngine/Core/Core.hpp"
#include <limits.h>
#include "LuxEngine/Core/LuxAutoInit.hpp"








// Init and debug ---------------------------------------------------------------------------------------------------------------------------//








namespace lux::core::render{
	RtArray<VkSemaphore>	drawFrameImageAquiredSemaphore;
	RtArray<VkSemaphore>	drawFrameObjectsRenderedSemaphore;
	RtArray<VkSemaphore>	drawFrameCopySemaphore;
	RtArray<VkSemaphore>	drawFrameClearSemaphore;
	RtArray<VkFence>		drawFrameImageRenderedFence;
	int32					renderCurrentFrame = 0;
	RtArray<obj::Base*>		objUpdates2D;
	FenceDE					pendingObjectUpdatesFence;








	void init(const bool vUseVSync){
		useVSync = vUseVSync;

		luxDebug(Failure printf("D E B U G    M O D E"));		MainSeparator;

		//Initialize vulkan
		luxCheckVk(glfwCreateWindowSurface(instance, wnd::window, nullptr, &surface), "Failed to create window surface");
		Normal printf("    Searching for physical devices...    \n");
		dvc::deviceGetPhysical( );
		cmd::createGraphicsCommandPool( );
		Normal printf("    Creating VK swapchain...             ");
		swapchain::swapchainCreate( );

		luxDebug(createDebugMessenger( ));
		createSyncObjs( );
	}




	#ifdef LUX_DEBUG
	void createDebugMessenger( ) {
		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		debug::populateDebugMessengerCreateInfo(createInfo);
		luxCheckVk(debug::CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger), "Failed to set up debug messenger");
	}
	#endif




	void createSyncObjs( ) {
		drawFrameImageAquiredSemaphore		.resize(out::renderMaxFramesInFlight);
		drawFrameObjectsRenderedSemaphore	.resize(out::renderMaxFramesInFlight);
		drawFrameCopySemaphore				.resize(out::renderMaxFramesInFlight);
		drawFrameClearSemaphore				.resize(out::renderMaxFramesInFlight);
		drawFrameImageRenderedFence			.resize(out::renderMaxFramesInFlight);

		VkSemaphoreCreateInfo semaphoreInfo{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
		};
		VkFenceCreateInfo fenceInfo{
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.flags = VK_FENCE_CREATE_SIGNALED_BIT
		};
		for(int32 i = 0; i < out::renderMaxFramesInFlight; ++i) {
			luxCheckCond(
				vkCreateSemaphore(dvc::graphics.LD, &semaphoreInfo, nullptr, &drawFrameImageAquiredSemaphore[i])	!= VK_SUCCESS ||
				vkCreateSemaphore(dvc::graphics.LD, &semaphoreInfo, nullptr, &drawFrameObjectsRenderedSemaphore[i])	!= VK_SUCCESS ||
				vkCreateSemaphore(dvc::graphics.LD, &semaphoreInfo, nullptr, &drawFrameCopySemaphore[i])			!= VK_SUCCESS ||
				vkCreateSemaphore(dvc::graphics.LD, &semaphoreInfo, nullptr, &drawFrameClearSemaphore[i])			!= VK_SUCCESS ||
				vkCreateFence(    dvc::graphics.LD, &fenceInfo, 	nullptr, &drawFrameImageRenderedFence[i])		!= VK_SUCCESS,
				"Failed to create vulkan sync objects"
			);
		}
	}








// Draw loop --------------------------------------------------------------------------------------------------------------------------------//








	//TODO multithreaded submit and command creation
	void drawFrame( ) {
		if(c::shaders::CShaders.usedCount( ) <= 1) return;
		vkWaitForFences(dvc::graphics.LD, 1, &drawFrameImageRenderedFence[renderCurrentFrame], false, INT_MAX);


		//Redraw frame if necessary
		redraw:
		if(out::renderFramebufferResized) {
			out::renderFramebufferResized = false;
			swapchain::swapchainRecreate(true);
			goto redraw;
		}


		//Acquire swapchain image
		uint32 imageIndex;
		{
			switch(vkAcquireNextImageKHR(dvc::graphics.LD, swapchain::swapchain, INT_MAX, drawFrameImageAquiredSemaphore[renderCurrentFrame], VK_NULL_HANDLE, &imageIndex)) {
				case VK_SUCCESS: case VK_SUBOPTIMAL_KHR: break;
				case VK_ERROR_OUT_OF_DATE_KHR: swapchain::swapchainRecreate(false);  return;
				default: Failure printf("Failed to aquire swapchain image");
			}
		}




		//TODO don't recreate the command buffer array every time
		//TODO use a staging buffer
		//Update render result submitting the command buffers to the compute queues
		static VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT };
		{
			c::shaders::addShaderFence.startFirst( );
			c::shaders::CShadersCBs.resize(c::shaders::CShaders.usedCount( ));
			for(uint32 i = 0; i < c::shaders::CShaders.count( ); ++i) {
				if(c::shaders::CShaders.isValid(i)) c::shaders::CShadersCBs[i] = c::shaders::CShaders[i].commandBuffers[0];
			}
			c::shaders::addShaderFence.endFirst( );

			static VkSubmitInfo submitInfo{
				.sType{ VK_STRUCTURE_TYPE_SUBMIT_INFO },
				.pNext{ nullptr },
				.waitSemaphoreCount{ 1 },
				.pWaitDstStageMask{ waitStages },
				.signalSemaphoreCount{ 1 },
			};
			submitInfo.pWaitSemaphores = &drawFrameImageAquiredSemaphore[renderCurrentFrame];
			submitInfo.pSignalSemaphores = &drawFrameObjectsRenderedSemaphore[renderCurrentFrame];
			submitInfo.commandBufferCount = c::shaders::CShadersCBs.count( );
			submitInfo.pCommandBuffers = c::shaders::CShadersCBs.begin( );
			luxCheckVk(vkQueueSubmit(dvc::graphics.graphicsQueue, 1, &submitInfo, nullptr), "Failed to submit graphics command buffer");
		}



		{ //Convert and clear shader
			static VkSubmitInfo submitInfo{
				.sType{ VK_STRUCTURE_TYPE_SUBMIT_INFO },
				.waitSemaphoreCount{ 1 },
				.pWaitDstStageMask{ waitStages },
				.commandBufferCount{ 1 },
				.signalSemaphoreCount{ 1 },
			};
			submitInfo.pCommandBuffers = &c::shaders::CShaders[0].commandBuffers[0];
			submitInfo.pWaitSemaphores = &drawFrameObjectsRenderedSemaphore[renderCurrentFrame];
			submitInfo.pSignalSemaphores = &drawFrameClearSemaphore[renderCurrentFrame];
			luxCheckVk(vkQueueSubmit(dvc::graphics.graphicsQueue, 1, &submitInfo, nullptr), "Failed to submit graphics command buffer");
		}




		{ //Copy shader
			static VkSubmitInfo submitInfo{
				.sType{ VK_STRUCTURE_TYPE_SUBMIT_INFO },
				.waitSemaphoreCount{ 1 },
				.pWaitDstStageMask{ waitStages },
				.commandBufferCount{ 1 },
				.signalSemaphoreCount{ 1 },
			};
			submitInfo.pWaitSemaphores   = &drawFrameClearSemaphore[renderCurrentFrame];
			submitInfo.pSignalSemaphores = &drawFrameCopySemaphore [renderCurrentFrame];
			submitInfo.pCommandBuffers   = &c::copyCommandBuffers  [imageIndex];

			vkResetFences(dvc::graphics.LD, 1, &drawFrameImageRenderedFence[renderCurrentFrame]);
			luxCheckVk(vkQueueSubmit(dvc::graphics.graphicsQueue, 1, &submitInfo, drawFrameImageRenderedFence[renderCurrentFrame]), "Failed to submit graphics command buffer");
		}




		{ //Present frame
			static VkPresentInfoKHR presentInfo{
				.sType{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR },
				.waitSemaphoreCount{ 1 },
				.swapchainCount{ 1 },
				.pSwapchains{ &swapchain::swapchain },
			};
			presentInfo.pWaitSemaphores = &drawFrameCopySemaphore[renderCurrentFrame];
			presentInfo.pImageIndices   = &imageIndex;

			switch(vkQueuePresentKHR(dvc::graphics.presentQueue, &presentInfo)) {
				case VK_SUCCESS:  break;
				//TODO maybe suboptimal can still be used
				case VK_ERROR_OUT_OF_DATE_KHR: case VK_SUBOPTIMAL_KHR: {
					swapchain::swapchainRecreate(false);
					vkDeviceWaitIdle(dvc::graphics.LD);
					goto redraw;
				}
				default:  luxPrintError("Failed to present swapchain image");
			}

		}

		//Update frame number and flush the window data
		renderCurrentFrame = (renderCurrentFrame + 1) % out::renderMaxFramesInFlight;
		glfwSwapBuffers(wnd::window);




		//TODO parallelize work from a secondary render thread
		//Fix objects update requests
		if(objUpdates2D.count( ) > 0){
			pendingObjectUpdatesFence.startFirst( );
			VkCommandBuffer cb = core::render::cmd::beginSingleTimeCommands( );
			for(uint32 i = 0; i < objUpdates2D.count( ); i++){
				objUpdates2D[i]->render.updated = true;
				vkCmdUpdateBuffer(
					cb, objUpdates2D[i]->render.localData->buffer->buffer,
					rem::getCellOffset(objUpdates2D[i]->render.localData),
					objUpdates2D[i]->cellSize,
					(void*)objUpdates2D[i]->render.data
				);
			}
			core::render::cmd::endSingleTimeCommands(cb);
			objUpdates2D.clear( );
			pendingObjectUpdatesFence.endFirst( );
		}
	}








// Clean memory -----------------------------------------------------------------------------------------------------------------------------//








	void cleanup( ) {
		swapchain::cleanup( );																	//Clear swapchain components
		vkDestroyCommandPool(dvc::graphics.LD, cmd::singleTimeCommandPool, nullptr);			//Destroy graphics command pool

		for(int32 i = 0; i < out::renderMaxFramesInFlight; ++i) {								//Destroy sync objects
			vkDestroySemaphore(dvc::graphics.LD, drawFrameImageAquiredSemaphore[i], nullptr);
			vkDestroySemaphore(dvc::graphics.LD, drawFrameObjectsRenderedSemaphore[i], nullptr);
			vkDestroySemaphore(dvc::graphics.LD, drawFrameCopySemaphore[i], nullptr);
			vkDestroySemaphore(dvc::graphics.LD, drawFrameClearSemaphore[i], nullptr);
			vkDestroyFence(dvc::graphics.LD, drawFrameImageRenderedFence[i], nullptr);
		}

		//If the compute and the graphics devices are not the same, destroy the graphics device
		if(dvc::graphics.PD.properties.deviceID != dvc::compute.PD.properties.deviceID) vkDestroyDevice(dvc::graphics.LD, nullptr);
		vkDestroyDevice(dvc::compute.LD, nullptr);												//Destroy the compute device
		//for (auto& device : secondary) vkDestroyDevice(device.LD, nullptr);					//Destroy all the secondary devices

		luxDebug(debug::DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr));		//Destroy the debug messenger if present
		vkDestroySurfaceKHR(instance, surface, nullptr);										//Destroy the vulkan surface
	}








// Other ------------------------------------------------------------------------------------------------------------------------------------//








	VkFormat findSupportedFormat(const RtArray<VkFormat>* pCandidates, const VkImageTiling vTiling, const VkFormatFeatureFlags vFeatures) {
		for(VkFormat format : *pCandidates) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(dvc::graphics.PD.device, format, &props);

			if(( vTiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & vFeatures) == vFeatures) ||
				(vTiling == VK_IMAGE_TILING_LINEAR  && (props.linearTilingFeatures  & vFeatures) == vFeatures)) {
				return format;
			}
		}
		luxPrintError("Failed to find a supported format");
		return VK_FORMAT_UNDEFINED;
	}




	//Returns the index of the memory with the specified type and properties
	uint32 findMemoryType(const uint32 vTypeFilter, const VkMemoryPropertyFlags vProperties) {
		VkPhysicalDeviceMemoryProperties memProperties;							//Get memory vProperties
		vkGetPhysicalDeviceMemoryProperties(dvc::graphics.PD.device, &memProperties);

		for(uint32 i = 0; i < memProperties.memoryTypeCount; ++i) {				//Search for the memory that has the specified properties and type and return its index
			if((vTypeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & vProperties) == vProperties) return i;
		}
		luxPrintError("Failed to find suitable memory type");
		return -1;
	}
}