#include "LuxEngine/Core/Render/GCommands.hpp"
#include "LuxEngine/Core/Render/Window/Swapchain.hpp"
#include "LuxEngine/Core/Render/Shaders/Shader.hpp"
#include "LuxEngine/Core/Devices.hpp"
#include "LuxEngine/Types/LuxObject/Obj_b.hpp"
#include <climits>
#include <chrono>








// Init and debug ---------------------------------------------------------------------------------------------------------------------------//








namespace lux::core::render{
	alignCache std::mutex graphicsQueueSubmit_m;
	alignCache std::mutex presentQueueSubmit_m;



	#ifdef LUX_DEBUG
	void createDebugMessenger() {
		vk::DebugUtilsMessengerCreateInfoEXT createInfo;
		debug::populateDebugMessengerCreateInfo(createInfo);
		dbg::checkVk(debug::CreateDebugUtilsMessengerEXT(dvc::instance, &createInfo, nullptr, &dvc::debugMessenger), "Failed to set up debug messenger");
	}
	#endif
}








// Draw loop --------------------------------------------------------------------------------------------------------------------------------//








namespace lux{
	void Window::draw() {
		auto last = std::chrono::high_resolution_clock::now();
		running = true;
		while(running) {
		// continue; //BUG REMOVE
			auto start = std::chrono::high_resolution_clock::now();

			sleep(0); //Prevent extra overhead when no object has to be rendered
			// if(swp.shaders.count() <= 1) continue;
			addShaderFence.lock();
			if(swp.shadersCBs.count() <= 1) {
				addShaderFence.unlock();
				continue;
			}
			addShaderFence.unlock();
			core::dvc::graphics.LD.waitForFences(1, &swp.frames[swp.curFrame].f_rendered, false, INT_MAX);


			//Redraw frame if necessary
			redraw:
			if(swp.resized) {
				swp.resized = false;
				swp.recreate();
				goto redraw;
			}


			//Acquire swapchain image
			uint32 imageIndex;
			{
				switch(core::dvc::graphics.LD.acquireNextImageKHR(swp.swapchain, INT_MAX, swp.frames[swp.curFrame].s_aquired, VK_NULL_HANDLE, &imageIndex)) {
					case vk::Result::eSuccess: case vk::Result::eSuboptimalKHR: break;
					case vk::Result::eErrorOutOfDateKHR: swp.recreate();  continue;
					default: Failure printf("Failed to acquire swapchain image");
				}
			}




			//TODO don't recreate the command buffer array every time
			//TODO use a staging buffer
			//Update render result submitting the command buffers to the compute queues
			const vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eComputeShader };
			addShaderFence.lock();




			const vk::SubmitInfo submitInfos[]{
				vk::SubmitInfo() //Draw objects
					.setWaitSemaphoreCount   (1)
					.setPWaitSemaphores      (&swp.frames[swp.curFrame].s_aquired)
					.setPWaitDstStageMask    (waitStages)
					.setCommandBufferCount   (swp.shadersCBs.count())
					.setPCommandBuffers      (swp.shadersCBs.begin())
					.setSignalSemaphoreCount (1)
					.setPSignalSemaphores    (&swp.frames[swp.curFrame].s_objects)
				,
				vk::SubmitInfo() //Convert and clear shader
					.setWaitSemaphoreCount   (1)
					.setPWaitSemaphores      (&swp.frames[swp.curFrame].s_objects)
					.setPWaitDstStageMask    (waitStages)
					.setCommandBufferCount   (1)
					// .setPCommandBuffers   (new vk::CommandBuffer(swp.shaders[0].commandBuffers[0]))
					.setPCommandBuffers      (&sh_clear.commandBuffers[0])
					.setSignalSemaphoreCount (1)
					.setPSignalSemaphores    (&swp.frames[swp.curFrame].s_clear)
				,
				vk::SubmitInfo() //Copy shader
					.setWaitSemaphoreCount   (1)
					.setPWaitSemaphores      (&swp.frames[swp.curFrame].s_clear)
					.setPWaitDstStageMask    (waitStages)
					.setCommandBufferCount   (1)
					.setPCommandBuffers      (&copyCommandBuffers[imageIndex])
					.setSignalSemaphoreCount (1)
					.setPSignalSemaphores    (&swp.frames[swp.curFrame].s_copy)
				,
			};
			addShaderFence.unlock(); //FIXME
			core::dvc::graphics.LD.resetFences(1, &swp.frames[swp.curFrame].f_rendered);
			core::render::graphicsQueueSubmit_m.lock();
				core::dvc::graphics.graphicsQueue.submit(3, submitInfos, swp.frames[swp.curFrame].f_rendered), "Failed to submit graphics command buffer");
			core::render::graphicsQueueSubmit_m.unlock();




			{ //Present frame
				auto presentInfo = const vk::PresentInfoKHR()
					.setWaitSemaphoreCount (1)
					.setPWaitSemaphores    (&swp.frames[swp.curFrame].s_copy)
					.setSwapchainCount     (1)
					.setPSwapchains        (&swp.swapchain)
					.setPImageIndices      (&imageIndex)
				;

				core::render::presentQueueSubmit_m.lock();
					auto presentResult = core::dvc::graphics.presentQueue.presentKHR(&presentInfo); //TODO graphics and present queues could be the same, in some devices. In that case, use the same mutex
				core::render::presentQueueSubmit_m.unlock();
				switch(presentResult){
					case vk::Result::eSuccess:  break;
					case vk::Result::eErrorOutOfDateKHR: case vk::Result::eSuboptimalKHR: { //TODO maybe suboptimal can still be used
						swp.recreate();
						goto redraw;
					}
					default: dbg::printError("Failed to present swapchain image");
				}

			}

			//Update frame number and flush the window data
			swp.curFrame = (swp.curFrame + 1) % lux::core::wnd::__renderMaxFramesInFlight;
			glfwSwapBuffers(window);




			//TODO parallelize work from a secondary render thread
			//Fix objects update requests
			if(objUpdates2D.count() > 0) {
				objUpdates2D_f.lock();
				vk::CommandBuffer cb = core::render::cmd::beginSingleTimeCommands();
				for(uint32 i = 0; i < objUpdates2D.count(); i++) {
					objUpdates2D[i]->render.updated = true;
					cb.updateBuffer(
						objUpdates2D[i]->getShVData().cell->csc.buffer,
						objUpdates2D[i]->getShVData().cell->localOffset,
						objUpdates2D[i]->getShVData().cell->cellSize,
						(void*)objUpdates2D[i]->getShData()
					);
				}
				core::render::cmd::endSingleTimeCommands(cb);
				objUpdates2D.clear();
				objUpdates2D_f.unlock();
			}
			//FIXME ADD COPY FROM RAM FUNCTTION TO VRAM ALLOCATIONS
			if(glfwWindowShouldClose(window)) return;


			auto end = std::chrono::high_resolution_clock::now();
			auto duration = duration_cast<std::chrono::microseconds>(end - start);
			if(duration_cast<std::chrono::seconds>(end - last).count() >= 1){
				std::cout << "\nFPS: " << 1.0f/(((float)duration.count())/(1000*1000));
				std::cout << "\n" << duration.count();
				last = end;
			}
		}
	}
}








// Clean memory -----------------------------------------------------------------------------------------------------------------------------//








namespace lux::core::render{
	void cleanup() {
		dvc::graphics.LD.destroyCommandPool(cmd::singleTimeCommandPool, nullptr);	//Destroy graphics command pool

		//If the compute and the graphics devices are not the same, destroy the graphics device
		if(dvc::graphics.PD.properties.deviceID != dvc::compute.PD.properties.deviceID) dvc::graphics.LD.destroy(nullptr);
		dvc::compute.LD.destroy(nullptr);													//Destroy the compute device
		//for (auto& device : secondary) vkDestroyDevice(device.LD, nullptr);						//Destroy all the secondary devices

		_dbg(debug::DestroyDebugUtilsMessengerEXT(dvc::instance, dvc::debugMessenger, nullptr));	//Destroy the debug messenger if present
	}








// Other ------------------------------------------------------------------------------------------------------------------------------------//








	vk::Format findSupportedFormat(const RtArray<vk::Format>* pCandidates, const vk::ImageTiling vTiling, const vk::FormatFeatureFlags vFeatures) {
		for(vk::Format format : *pCandidates) {
			auto props = dvc::graphics.PD.device.getFormatProperties(format); //Get format properties

			if(( vTiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & vFeatures) == vFeatures) ||
				(vTiling == vk::ImageTiling::eLinear  && (props.linearTilingFeatures  & vFeatures) == vFeatures)) {
				return format;
			}
		}
		dbg::printError("Failed to find a supported format");
		return vk::Format::eUndefined;
	}




	//Returns the index of the memory with the specified type and properties
	uint32 findMemoryType(const uint32 vTypeFilter, const vk::MemoryPropertyFlags vProperties) {
		auto memProperties = dvc::graphics.PD.device.getMemoryProperties();//Get memory vProperties

		for(uint32 i = 0; i < memProperties.memoryTypeCount; ++i) {				//Search for the memory that has the specified properties and type and return its index
			if((vTypeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & vProperties) == vProperties) return i;
		}
		dbg::printError("Failed to find suitable memory type");
		return (uint32)-1;
	}
}