#include <vulkan/vulkan.h>
#include "LuxEngine/Core/Render/Render.hpp"
#include "LuxEngine/Core/Render/GCommands.hpp"
#include "LuxEngine/Core/Render/Window/Swapchain.hpp"
#include "LuxEngine/Core/Render/Shaders/Shader.hpp"
#include "LuxEngine/Core/Devices.hpp"
#include "LuxEngine/Types/LuxObject/LuxObject.hpp"
#include "LuxEngine/Core/Core.hpp"
#include <climits>
#include "LuxEngine/Core/LuxAutoInit.hpp"








// Init and debug ---------------------------------------------------------------------------------------------------------------------------//







namespace lux::core::render{
	alignCache RtArray<VkSemaphore> s_imageAquired   (out::renderMaxFramesInFlight) ;
	alignCache RtArray<VkSemaphore> s_objectsRendered(out::renderMaxFramesInFlight) ;
	alignCache RtArray<VkSemaphore> s_copy           (out::renderMaxFramesInFlight) ;
	alignCache RtArray<VkSemaphore> s_clear          (out::renderMaxFramesInFlight) ;
	alignCache RtArray<VkFence>     imageRendered_f  (out::renderMaxFramesInFlight) ;
	alignCache int32                renderCurrentFrame = 0;
	alignCache RtArray<obj::Base*>  objUpdates2D;
	alignCache FenceDE              objUpdates2D_f;








	void init(const bool vUseVSync) {
		useVSync = vUseVSync;

		_dbg(Failure printf("D E B U G    M O D E"));		MainSeparator;

		//Initialize vulkan
		// dbg::checkVk(glfwCreateWindowSurface(instance, wnd::window, nullptr, &surface), "Failed to create window surface");
		Normal printf("    Searching for physical devices...    \n");
		// dvc::getPhysical(); //FIXME
		cmd::createGraphicsCommandPool();
		Normal printf("    Creating VK swapchain...             ");

		lux::window.swapchain.swapchainCreate(); //FIXME AUTOMATIZE

		_dbg(createDebugMessenger());


		//Create sync objects
		VkSemaphoreCreateInfo semaphoreInfo{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
		};
		VkFenceCreateInfo fenceInfo{
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.flags = VK_FENCE_CREATE_SIGNALED_BIT
		};
		for(int32 i = 0; i < out::renderMaxFramesInFlight; ++i) {
			lux::dbg::checkCond(
				vkCreateSemaphore(dvc::graphics.LD, &semaphoreInfo, nullptr, &s_imageAquired[i])	!= VK_SUCCESS ||
				vkCreateSemaphore(dvc::graphics.LD, &semaphoreInfo, nullptr, &s_objectsRendered[i])	!= VK_SUCCESS ||
				vkCreateSemaphore(dvc::graphics.LD, &semaphoreInfo, nullptr, &s_copy[i])			!= VK_SUCCESS ||
				vkCreateSemaphore(dvc::graphics.LD, &semaphoreInfo, nullptr, &s_clear[i])			!= VK_SUCCESS ||
				vkCreateFence(    dvc::graphics.LD, &fenceInfo, 	nullptr, &imageRendered_f[i])		!= VK_SUCCESS,
				"Failed to create vulkan sync objects"
			);
		}
	}




	#ifdef LUX_DEBUG
	void createDebugMessenger() {
		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		debug::populateDebugMessengerCreateInfo(createInfo);
		dbg::checkVk(debug::CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger), "Failed to set up debug messenger");
	}
	#endif








// Draw loop --------------------------------------------------------------------------------------------------------------------------------//








	//TODO multithreaded submit and command creation
	void drawFrame() {
		//FIXME __
		if(c::shaders::CShaders.count() <= 1) return;
		vkWaitForFences(dvc::graphics.LD, 1, &imageRendered_f[renderCurrentFrame], false, INT_MAX);


		//Redraw frame if necessary
		redraw:
		if(out::renderFramebufferResized) {
			lux::window.windowResizeFence.lock();
			out::renderFramebufferResized = false;
			lux::window.windowResizeFence.unlock();
			lux::window.swapchain.swapchainRecreate(true); //FIXME DONT DEPEND ON A WINDOW
			goto redraw;
		}


		//Acquire swapchain image
		uint32 imageIndex;
		{
			switch(vkAcquireNextImageKHR(dvc::graphics.LD, lux::window.swapchain.swapchain, INT_MAX, s_imageAquired[renderCurrentFrame], VK_NULL_HANDLE, &imageIndex)) { //FIXME DONT DEPEND ON A WINDOW
				case VK_SUCCESS: case VK_SUBOPTIMAL_KHR: break;
				case VK_ERROR_OUT_OF_DATE_KHR: lux::window.swapchain.swapchainRecreate(false);  return; //FIXME DONT DEPEND ON A WINDOW
				default: Failure printf("Failed to aquire swapchain image");
			}
		}




		//TODO don't recreate the command buffer array every time
		//TODO use a staging buffer
		//Update render result submitting the command buffers to the compute queues
		static VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT };
		{
			c::shaders::addShaderFence.lock();
			//FIXME __
			c::shaders::CShadersCBs.resize(c::shaders::CShaders.count());
			for(uint32 i = 0; i < c::shaders::CShaders.count(); ++i) {
				//FIXME __
				c::shaders::CShadersCBs[i] = c::shaders::CShaders[i].commandBuffers[0];
			}
			c::shaders::addShaderFence.unlock();

			static VkSubmitInfo submitInfo{
				.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
				.pNext                = nullptr,
				.waitSemaphoreCount   = 1,
				.pWaitDstStageMask    = waitStages,
				.signalSemaphoreCount = 1
			};
			submitInfo.pWaitSemaphores   = &s_imageAquired   [renderCurrentFrame];
			submitInfo.pSignalSemaphores = &s_objectsRendered[renderCurrentFrame];
			submitInfo.commandBufferCount = c::shaders::CShadersCBs.count();
			submitInfo.pCommandBuffers    = c::shaders::CShadersCBs.begin();
			dbg::checkVk(vkQueueSubmit(dvc::graphics.graphicsQueue, 1, &submitInfo, nullptr), "Failed to submit graphics command buffer");
		}



		{ //Convert and clear shader
			static VkSubmitInfo submitInfo{
				.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
				.waitSemaphoreCount   = 1,
				.pWaitDstStageMask    = waitStages,
				.commandBufferCount   = 1,
				.signalSemaphoreCount = 1
			};
			submitInfo.pCommandBuffers = &c::shaders::CShaders[0].commandBuffers[0];
			submitInfo.pWaitSemaphores = &s_objectsRendered[renderCurrentFrame];
			submitInfo.pSignalSemaphores = &s_clear[renderCurrentFrame];
			dbg::checkVk(vkQueueSubmit(dvc::graphics.graphicsQueue, 1, &submitInfo, nullptr), "Failed to submit graphics command buffer");
		}




		{ //Copy shader
			static VkSubmitInfo submitInfo{
				.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
				.waitSemaphoreCount   = 1,
				.pWaitDstStageMask    = waitStages,
				.commandBufferCount   = 1,
				.signalSemaphoreCount = 1
			};
			submitInfo.pWaitSemaphores   = &s_clear[renderCurrentFrame];
			submitInfo.pSignalSemaphores = &s_copy [renderCurrentFrame];
			submitInfo.pCommandBuffers   = &buffers::copyCommandBuffers  [imageIndex];

			vkResetFences(dvc::graphics.LD, 1, &imageRendered_f[renderCurrentFrame]);
			dbg::checkVk(vkQueueSubmit(dvc::graphics.graphicsQueue, 1, &submitInfo, imageRendered_f[renderCurrentFrame]), "Failed to submit graphics command buffer");
		}




		{ //Present frame
			static VkPresentInfoKHR presentInfo{
				.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
				.waitSemaphoreCount = 1,
				.swapchainCount     = 1,
				.pSwapchains        = &lux::window.swapchain.swapchain //FIXME DONT DEPEND ON A WINDOW
			};
			presentInfo.pWaitSemaphores = &s_copy[renderCurrentFrame];
			presentInfo.pImageIndices   = &imageIndex;

			switch(vkQueuePresentKHR(dvc::graphics.presentQueue, &presentInfo)) {
				case VK_SUCCESS:  break;
				//TODO maybe suboptimal can still be used
				case VK_ERROR_OUT_OF_DATE_KHR: case VK_SUBOPTIMAL_KHR: {
					lux::window.swapchain.swapchainRecreate(false); //FIXME DONT DEPEND ON A WINDOW
					vkDeviceWaitIdle(dvc::graphics.LD);
					goto redraw;
				}
				default:  dbg::printError("Failed to present swapchain image");
			}

		}

		//Update frame number and flush the window data
		renderCurrentFrame = (renderCurrentFrame + 1) % out::renderMaxFramesInFlight;
		glfwSwapBuffers(lux::window.window);




		//TODO parallelize work from a secondary render thread
		//Fix objects update requests
		if(objUpdates2D.count() > 0) {
			objUpdates2D_f.startFirst();
			VkCommandBuffer cb = core::render::cmd::beginSingleTimeCommands();
			for(uint32 i = 0; i < objUpdates2D.count(); i++) {
				objUpdates2D[i]->render.updated = true;
				vkCmdUpdateBuffer(
					cb,
					objUpdates2D[i]->render.localData.cell->csc.buffer,
					objUpdates2D[i]->render.localData.cell->localOffset,
					objUpdates2D[i]->cellSize,
					(void*)objUpdates2D[i]->render.data
				);
			}
			core::render::cmd::endSingleTimeCommands(cb);
			objUpdates2D.clear();
			objUpdates2D_f.endFirst();
		}
	}








// Clean memory -----------------------------------------------------------------------------------------------------------------------------//








	void cleanup() {
		lux::window.swapchain.cleanup();																	//Clear swapchain components //FIXME DONT DEPEND ON A WINDOW
		vkDestroyCommandPool(dvc::graphics.LD, cmd::singleTimeCommandPool, nullptr);			//Destroy graphics command pool

		for(int32 i = 0; i < out::renderMaxFramesInFlight; ++i) {								//Destroy sync objects
			vkDestroySemaphore(dvc::graphics.LD, s_imageAquired[i],    nullptr);
			vkDestroySemaphore(dvc::graphics.LD, s_objectsRendered[i], nullptr);
			vkDestroySemaphore(dvc::graphics.LD, s_copy[i],            nullptr);
			vkDestroySemaphore(dvc::graphics.LD, s_clear[i],           nullptr);
			vkDestroyFence    (dvc::graphics.LD, imageRendered_f[i],       nullptr);
		}

		//If the compute and the graphics devices are not the same, destroy the graphics device
		if(dvc::graphics.PD.properties.deviceID != dvc::compute.PD.properties.deviceID) vkDestroyDevice(dvc::graphics.LD, nullptr);
		vkDestroyDevice(dvc::compute.LD, nullptr);												//Destroy the compute device
		//for (auto& device : secondary) vkDestroyDevice(device.LD, nullptr);					//Destroy all the secondary devices

		_dbg(debug::DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr));		//Destroy the debug messenger if present
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
		dbg::printError("Failed to find a supported format");
		return VK_FORMAT_UNDEFINED;
	}




	//Returns the index of the memory with the specified type and properties
	uint32 findMemoryType(const uint32 vTypeFilter, const VkMemoryPropertyFlags vProperties) {
		VkPhysicalDeviceMemoryProperties memProperties;							//Get memory vProperties
		vkGetPhysicalDeviceMemoryProperties(dvc::graphics.PD.device, &memProperties);

		for(uint32 i = 0; i < memProperties.memoryTypeCount; ++i) {				//Search for the memory that has the specified properties and type and return its index
			if((vTypeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & vProperties) == vProperties) return i;
		}
		dbg::printError("Failed to find suitable memory type");
		return -1;
	}
}