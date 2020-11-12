

#include <vulkan/vulkan.h>
#include "LuxEngine/Core/Compute/CShader_t.h"
#include "LuxEngine/Core/Graphics/Graphics.h"
#include "LuxEngine/Core/Graphics/GCommands.h"
#include "LuxEngine/Core/Graphics/GSwapchain.h"
#include "LuxEngine/Core/Compute/CShader.h"
#include "LuxEngine/Core/Devices.h"
#include "LuxEngine/Types/LuxObject/LuxObject.h"
#include "LuxEngine/Core/Core.h"
#include <limits.h>
#include "LuxEngine/Core/LuxAutoInit.hpp"




// #pragma optimize("", off)
// PostInitializer(LUX_H_GRAPHICS);
// #pragma optimize("", on)
namespace lux::core::g{
	// DynArray<VkSemaphore>		NoInitLux(drawFrameImageAquiredSemaphore);
	// DynArray<VkSemaphore>		NoInitLux(drawFrameObjectsRenderedSemaphore);
	// DynArray<VkSemaphore>		NoInitLux(drawFrameCopySemaphore);
	// DynArray<VkSemaphore>		NoInitLux(drawFrameClearSemaphore);
	// DynArray<VkFence>			NoInitLux(drawFrameImageRenderedFence);
	// int32					NoInitVar(renderCurrentFrame);
	// DynArray<obj::Base*>	NoInitLux(objUpdates2D);
	// FenceDE					NoInitLux(pendingObjectUpdatesFence);
	DynArray<VkSemaphore>		drawFrameImageAquiredSemaphore;
	DynArray<VkSemaphore>		drawFrameObjectsRenderedSemaphore;
	DynArray<VkSemaphore>		drawFrameCopySemaphore;
	DynArray<VkSemaphore>		drawFrameClearSemaphore;
	DynArray<VkFence>			drawFrameImageRenderedFence;
	int32					renderCurrentFrame;
	DynArray<obj::Base*>	objUpdates2D;
	FenceDE					pendingObjectUpdatesFence;





	// void preInit( ) {
	AutoInit(LUX_H_GRAPHICS) {
		// drawFrameImageAquiredSemaphore.DynArray::DynArray( );
		// drawFrameObjectsRenderedSemaphore.DynArray::DynArray( );
		// drawFrameCopySemaphore.DynArray::DynArray( );
		// drawFrameClearSemaphore.DynArray::DynArray( );
		// drawFrameImageRenderedFence.DynArray::DynArray( );
		// renderCurrentFrame = 0;
		// objUpdates2D.DynArray::DynArray( );
		// pendingObjectUpdatesFence.FenceDE::FenceDE( );
		drawFrameImageAquiredSemaphore = DynArray<VkSemaphore>( );
		drawFrameObjectsRenderedSemaphore = DynArray<VkSemaphore>( );
		drawFrameCopySemaphore = DynArray<VkSemaphore>( );
		drawFrameClearSemaphore = DynArray<VkSemaphore>( );
		drawFrameImageRenderedFence = DynArray<VkFence>( );
		renderCurrentFrame = 0;
		objUpdates2D = DynArray<obj::Base*>( );
		pendingObjectUpdatesFence = FenceDE( );
	}



	void init(const bool vUseVSync, const float vFOV){
		useVSync = vUseVSync;
		FOV = vFOV;

		luxDebug(Failure printf("D E B U G    M O D E"));		MainSeparator;

		//Initialize vulkan
		TryVk(glfwCreateWindowSurface(instance, wnd::window, nullptr, &surface)) printError("Failed to create window surface", true, -1);
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
		TryVk(debug::CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger)) printError("Failed to set up debug messenger", false, -1);
	}
	#endif




	void createSyncObjs( ) {
		drawFrameImageAquiredSemaphore.resize(out::renderMaxFramesInFlight);
		drawFrameObjectsRenderedSemaphore.resize(out::renderMaxFramesInFlight);
		drawFrameCopySemaphore.resize(out::renderMaxFramesInFlight);
		drawFrameClearSemaphore.resize(out::renderMaxFramesInFlight);
		drawFrameImageRenderedFence.resize(out::renderMaxFramesInFlight);

		VkSemaphoreCreateInfo semaphoreInfo{ };
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{ };
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for(int32 i = 0; i < out::renderMaxFramesInFlight; ++i) {
			if(vkCreateSemaphore(dvc::graphics.LD, &semaphoreInfo, nullptr, &drawFrameImageAquiredSemaphore[i]) != VK_SUCCESS ||
				vkCreateSemaphore(dvc::graphics.LD, &semaphoreInfo, nullptr, &drawFrameObjectsRenderedSemaphore[i]) != VK_SUCCESS ||
				vkCreateSemaphore(dvc::graphics.LD, &semaphoreInfo, nullptr, &drawFrameCopySemaphore[i]) != VK_SUCCESS ||
				vkCreateSemaphore(dvc::graphics.LD, &semaphoreInfo, nullptr, &drawFrameClearSemaphore[i]) != VK_SUCCESS ||
				vkCreateFence(dvc::graphics.LD, &fenceInfo, nullptr, &drawFrameImageRenderedFence[i]) != VK_SUCCESS
				){
				printError("Failed to create vulkan sync objects", true, -1);
			}
		}
	}








	//TODO multithreaded submit and command creation
	void drawFrame( ) {
		if(c::shaders::CShaders.usedCount( ) <= 1) return;
		vkWaitForFences(dvc::graphics.LD, 1, &drawFrameImageRenderedFence[renderCurrentFrame], false, INT_MAX);


		redraw:
		if(out::renderFramebufferResized) {
			out::renderFramebufferResized = false;
			swapchain::swapchainRecreate(true);
			goto redraw;
		}





		uint32 imageIndex;
		{ //Acquire swapchain image
			switch(vkAcquireNextImageKHR(dvc::graphics.LD, swapchain::swapchain, /*1000*1000*5*/INT_MAX /*5s*/, drawFrameImageAquiredSemaphore[renderCurrentFrame], VK_NULL_HANDLE, &imageIndex)) {
				case VK_SUCCESS: case VK_SUBOPTIMAL_KHR: break;
				case VK_ERROR_OUT_OF_DATE_KHR: swapchain::swapchainRecreate(false);  return;
				default: Failure printf("Failed to aquire swapchain image");
			}
		}




		//TODO don't recreate the command buffer array every time
		//TODO use a staging buffer
		static VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT };
		{ //Update render result submitting the command buffers to the compute queues
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
			TryVk(vkQueueSubmit(dvc::graphics.graphicsQueue, 1, &submitInfo, nullptr)) printError("Failed to submit graphics command buffer", false, -1);
		}



		{ //Convert and clear
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
			TryVk(vkQueueSubmit(dvc::graphics.graphicsQueue, 1, &submitInfo, nullptr)) printError("Failed to submit graphics command buffer", false, -1);
		}




		{ //Copy
			static VkSubmitInfo submitInfo{
				.sType{ VK_STRUCTURE_TYPE_SUBMIT_INFO },
				.waitSemaphoreCount{ 1 },
				.pWaitDstStageMask{ waitStages },
				.commandBufferCount{ 1 },
				.signalSemaphoreCount{ 1 },
			};
			//TODO maybe the normal array is bugged
			submitInfo.pWaitSemaphores = &drawFrameClearSemaphore[renderCurrentFrame];
			submitInfo.pSignalSemaphores = &drawFrameCopySemaphore[renderCurrentFrame];
			submitInfo.pCommandBuffers = &c::copyCommandBuffers[imageIndex];

			vkResetFences(dvc::graphics.LD, 1, &drawFrameImageRenderedFence[renderCurrentFrame]);
			TryVk(vkQueueSubmit(dvc::graphics.graphicsQueue, 1, &submitInfo, drawFrameImageRenderedFence[renderCurrentFrame])) printError("Failed to submit graphics command buffer", false, -1);
		}




		{ //Present
			static VkPresentInfoKHR presentInfo{
				.sType{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR },
				.waitSemaphoreCount{ 1 },
				.swapchainCount{ 1 },
				.pSwapchains{ &swapchain::swapchain },
			};
			presentInfo.pWaitSemaphores = &drawFrameCopySemaphore[renderCurrentFrame];
			presentInfo.pImageIndices = &imageIndex;

			switch(vkQueuePresentKHR(dvc::graphics.presentQueue, &presentInfo)) {
				case VK_SUCCESS:  break;
				//TODO maybe suboptimal can still be used
				case VK_ERROR_OUT_OF_DATE_KHR: case VK_SUBOPTIMAL_KHR: {
					swapchain::swapchainRecreate(false);
					vkDeviceWaitIdle(dvc::graphics.LD);
					goto redraw;
				}
				default:  printError("Failed to present swapchain image", false, -1);
			}

		}

		//Update frame number and flush the window data
		renderCurrentFrame = (renderCurrentFrame + 1) % out::renderMaxFramesInFlight;
		glfwSwapBuffers(wnd::window);


		//TODO fences
		//calls      |                    A----.                   |    void A(){                 |   void B(){
		//           |      A  B---.  B  B  B  | B----.            |        fence.startFirst();   |       fence.startSecond();
		//           |      |      |  |  |  |  |      |            |        ...                   |       ...
		//unordered  |      AAAAAA-BB-BB-BB-BB-AAAAAA-BB           |        fence.endFirst();     |       fence.endSecond();
		//ordered    |      AAAAAA-BB----------AAAAAA-BB           |    }                         |   }
		//once		 |      AAAAAA-BB-------------------           |                              |
		//           |                                             |                              |
		//           -----------------------------------> t        |                              |



		//TODO parallelize work from a secondary render thread
		//Fix objects update requests
		if(objUpdates2D.count( ) > 0){
			pendingObjectUpdatesFence.startFirst( );
			// sleep(500);
			VkCommandBuffer cb = core::g::cmd::beginSingleTimeCommands( );
			// sleep(500);
			// sleep(500);
			// sleep(500);
			for(uint32 i = 0; i < objUpdates2D.count( ); i++){ //BUG found ya
			// sleep(500);
				objUpdates2D[i]->render.updated = true;
				//TODO remove debug junk
				//TODO add check. maybe. idk
				//if(objUpdates2D[i]->common.objectType > 0)
				// try{
					vkCmdUpdateBuffer(
						cb, objUpdates2D[i]->render.localData->buffer->buffer,
						rem::getCellOffset(objUpdates2D[i]->render.localData),
						//TODO test if this works with a normal program
						//BUG
						//BUG THE BUG ONLY OCCURS WITH THE FIRST OR SECOND 2D_LINE OBJECT UPDATED
						//BUG AND ONLY WHEN THE OTHER SYNCHRONIZED THREAD ENDS ITS FENCE
						//BUG PART OF THE VTABLE IS MODIFIED BY SOME THREAD. BUT JUST THE FIRST 2 BYTES
						// objUpdates2D[i]->getCellSize( ),
						objUpdates2D[i]->cellSize,
						(void*)objUpdates2D[i]->render.data
					);
				// }
				// catch(...){}
			}
			core::g::cmd::endSingleTimeCommands(cb);
			//objUpdates2D.resize(0);
			objUpdates2D.clear( );
			pendingObjectUpdatesFence.endFirst( );
		}
	}








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








	VkFormat findSupportedFormat(const DynArray<VkFormat>* pCandidates, const VkImageTiling vTiling, const VkFormatFeatureFlags vFeatures) {
		for(VkFormat format : *pCandidates) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(dvc::graphics.PD.device, format, &props);

			if((vTiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & vFeatures) == vFeatures) ||
				(vTiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & vFeatures) == vFeatures)) {
				return format;
			}
		}
		printError("Failed to find a supported format", true, -1);
		return VK_FORMAT_UNDEFINED;
	}




	//Returns the index of the memory with the specified type and properties
	uint32 findMemoryType(const uint32 vTypeFilter, const VkMemoryPropertyFlags vProperties) {
		VkPhysicalDeviceMemoryProperties memProperties;							//Get memory vProperties
		vkGetPhysicalDeviceMemoryProperties(dvc::graphics.PD.device, &memProperties);

		for(uint32 i = 0; i < memProperties.memoryTypeCount; ++i) {				//Search for the memory that has the specified properties and type and return its index
			if((vTypeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & vProperties) == vProperties) return i;
		}
		printError("Failed to find suitable memory type", true, -1);
		return -1;
	}
}