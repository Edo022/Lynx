#include "Lynx/Core/Core.hpp"
#include "Lynx/Core/Render/GCommands.hpp"
#include "Lynx/Core/Render/Window/Swapchain.hpp"
#include "Lynx/Core/Render/Shaders/Shader.hpp"
#include "Lynx/Core/Devices.hpp"
#include "Lynx/Types/Object/2D/Obj2_b.hpp"
#include <climits>
#include <chrono>








// Init and debug ---------------------------------------------------------------------------------------------------------------------------//








namespace lnx::core::render{
	alignCache std::mutex graphicsQueueSubmit_m;
	alignCache std::mutex presentQueueSubmit_m;



	#ifdef LNX_DEBUG
	void createDebugMessenger() {
		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		debug::populateDebugMessengerCreateInfo(createInfo);
		dbg::checkCond(VK_SUCCESS != debug::CreateDebugUtilsMessengerEXT(dvc::instance, &createInfo, nullptr, &dvc::debugMessenger), "Failed to set up debug messenger");
	}
	#endif
}








// Draw loop --------------------------------------------------------------------------------------------------------------------------------//




//TODO ADD FULL BACKTRACE



namespace lnx{
	void recSpawn(obj::Obj_bb* pObj, Window& pWindow){
		pObj->render.updates = pObj->render.updates & ~obj::eSpawn;			//Clear update bit (prevents redundant updates)
		pObj->render.parentWindow = &pWindow;								//Set owner window
		pObj->onSpawn(pWindow);												//Run user callback
		for(uint32 i = 0; i < pObj->getChildrenCount(); ++i){							//For each child
			// if((*ch)[i]->render.updates & obj::spawn){					//If it has the same update
			// (*ch)[i]->render.updates = (*ch)[i]->render.updates & ~obj::spawn;	//Clear update bit (prevents redundant updates)
			if(pObj->getChildrenIsValid(i)) recSpawn(pObj->getChildren(i), pWindow);										//Run recursive update on it
			// }
		}
	}

	void recUpdateg(obj::Obj_bb* pObj, vk::CommandBuffer& pCB){ //FIXME PASS command buffer BY VALUE
		pObj->render.updates = pObj->render.updates & ~obj::eUpdateg;
		pObj->onUpdateg(pCB);
		for(uint32 i = 0; i < pObj->getChildrenCount(); ++i){
			// if((*ch)[i]->render.updates & obj::limit){
			// (*ch)[i]->render.updates = (*ch)[i]->render.updates & ~obj::limit;
			if(pObj->getChildrenIsValid(i)) recUpdateg(pObj->getChildren(i), pCB);
			// }
		}
	}

	void recLimit(obj::Obj_bb* pObj){
		pObj->render.updates = pObj->render.updates & ~obj::eLimit;
		pObj->onLimit();
		for(uint32 i = 0; i < pObj->getChildrenCount(); ++i){
			// if((*ch)[i]->render.updates & obj::updateg){
			// (*ch)[i]->render.updates = (*ch)[i]->render.updates & ~obj::updateg;
			if(pObj->getChildrenIsValid(i)) recLimit(pObj->getChildren(i));
			// }
		}
	}



	void Window::draw() {
		auto last = std::chrono::high_resolution_clock::now();
		running = true;
		while(running) {
		// continue; //BUG REMOVE
			auto start = std::chrono::high_resolution_clock::now();

			sleep(0); //Prevent extra overhead when no object has to be rendered
			addObject_m.lock();
			// if(swp.shadersCBs.count() <= 1) {
			if(swp.shadersCBs.count() <= 0) {
				addObject_m.unlock();
				continue;
			}
			addObject_m.unlock();
			switch(core::dvc::graphics.ld.waitForFences(1, &swp.frames[swp.curFrame].f_rendered, false, LONG_MAX)){
				case vk::Result::eTimeout:         dbg::printError("Fence timed out"); break;
				case vk::Result::eErrorDeviceLost: dbg::printError("Device lost");     break;
				vkDefaultCases;
			}

			//BUG ^ THIS. CHECK TIMEOUT. CHECK RETURN VALUES
			//BUG [drm:amdgpu_dm_atomic_commit_tail [amdgpu]] *ERROR* Waiting for fences timed out!



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
				switch(core::dvc::graphics.ld.acquireNextImageKHR(swp.swapchain, UINT64_MAX, swp.frames[swp.curFrame].s_aquired, nullptr, &imageIndex)) {
					case vk::Result::eTimeout:       dbg::printWarning("Timeout");    break;
					case vk::Result::eNotReady:      dbg::printWarning("Not ready");  break;
					case vk::Result::eSuboptimalKHR: dbg::printWarning("Suboptimal"); break;
					case vk::Result::eErrorOutOfDateKHR:   swp.recreate(); goto redraw;
					case vk::Result::eErrorDeviceLost:     dbg::printError("Device lost");  break;
					case vk::Result::eErrorSurfaceLostKHR: dbg::printError("Surface lost"); break;
					#ifdef _WIN64 //This error is unique to windows
						case vk::Result::eErrorFullScreenExclusiveModeLostEXT: //FIXME
					#endif
					vkDefaultCases;
				}
			}




			//TODO don't recreate the command buffer array every time
			//TODO use a staging buffer
			//Update render result submitting the command buffers to the compute queues
			const vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eComputeShader };
			addObject_m.lock();




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
			addObject_m.unlock(); //FIXME

			switch(core::dvc::graphics.ld.resetFences(1, &swp.frames[swp.curFrame].f_rendered)){
				case vk::Result::eSuccess: break;
				case vk::Result::eErrorOutOfDeviceMemory: dbg::printError("Out of device memory"); break;
				// case vk::Result::eErrorOutOfHostMemory: dbg::printError("Out of host memory"); break;
				//!^ Not an error. This value is not returned
				default: dbg::printError("Unknown result");
			}

			core::render::graphicsQueueSubmit_m.lock();
			switch(core::dvc::graphics.gq.submit(3, submitInfos, swp.frames[swp.curFrame].f_rendered)){ vkDefaultCases; } //BUG UNCOMMENT
			// switch(core::dvc::graphics.gq.submit(1, submitInfos, swp.frames[swp.curFrame].f_rendered)){ vkDefaultCases; }
			// switch(core::dvc::graphics.gq.submit(2, submitInfos, swp.frames[swp.curFrame].f_rendered)){ vkDefaultCases; }
			core::render::graphicsQueueSubmit_m.unlock();




			{ //Present frame
				const auto presentInfo = vk::PresentInfoKHR()
					.setWaitSemaphoreCount (1)
					.setPWaitSemaphores    (&swp.frames[swp.curFrame].s_copy)
					.setSwapchainCount     (1)
					.setPSwapchains        (&swp.swapchain)
					.setPImageIndices      (&imageIndex)
				;
				core::render::presentQueueSubmit_m.lock();
				auto r = core::dvc::graphics.pq.presentKHR(presentInfo); //TODO graphics and present queues could be the same, in some devices. In that case, use the same mutex //BUG UNCOMMENT
				core::render::presentQueueSubmit_m.unlock();

				switch(r){ //BUG UNCOMMENT
					case vk::Result::eSuboptimalKHR: dbg::printWarning("Suboptimal"); break; //BUG UNCOMMENT
					case vk::Result::eErrorOutOfDateKHR:   swp.recreate(); goto redraw; //BUG UNCOMMENT
					case vk::Result::eErrorDeviceLost:     dbg::printError("Device lost");  break; //BUG UNCOMMENT
					case vk::Result::eErrorSurfaceLostKHR: dbg::printError("Surface lost"); break; //BUG UNCOMMENT
					#ifdef _WIN64 //This error is unique to windows //BUG UNCOMMENT
						case vk::Result::eErrorFullScreenExclusiveModeLostEXT: //FIXME //BUG UNCOMMENT
					#endif //BUG UNCOMMENT
					vkDefaultCases; //BUG UNCOMMENT
				} //BUG UNCOMMENT

			}

			//Update frame number and flush the window data
			swp.curFrame = (swp.curFrame + 1) % lnx::core::wnd::__renderMaxFramesInFlight;
			glfwSwapBuffers(window);




			//TODO parallelize work from a secondary render thread


			//TODO SEPARATE FUNCTIONS
			vk::CommandBuffer cb = core::render::cmd::beginSingleTimeCommands();
			requests_m.lock();
			if(!requests.empty()) for(auto r : requests){
				if(r->render.updates & obj::UpdateBits::eSpawn){
					// r->onSpawn(*this); //BUG, probably
					recSpawn(r, *this);
					// // CRenderSpaces.add((obj::RenderSpace2*)r); //FIXME REMOVE probably useless
				}
				if(r->render.updates & obj::UpdateBits::eLimit){
					// r->onLimit();                       //UG UNCOMMENT
					recLimit(r);
				}
				if(r->render.updates & obj::UpdateBits::eUpdateg){
					// cb.updateBuffer(
					// 	r->getShVData().cell->csc.buffer,
					// 	r->getShVData().cell->localOffset,
					// 	r->getShVData().cell->cellSize,
					// 	(void*)r->getShData()
					// );
					recUpdateg(r, cb);
				}
				// r->render.updates = obj::UpdateBits::none;
				_dbg(if(r->render.updates != obj::eNone) dbg::printWarning("Non-0 value detected for render.updates after update loop. This may indicate a race condition or a bug in the engine"));
			}
			requests.clear();
			requests_m.unlock();
			core::render::cmd::endSingleTimeCommands(cb);












			//FIXME REQUESTS SENT TO NON SPAWNED OBJECTS FROM INPUT CALLBACKS ARE EXECUTED DURING SPAWN
			//Input callbacks
			if(icQueues.onClick.queued){
				icQueues.onClick.m.lock();
				for(uint32 i = 0; i < icQueues.onClick.list.count(); ++i){
					if(icQueues.onClick.list.isValid(i)) icQueues.onClick.list[i]->onClick(icQueues.onClick.pos, icQueues.lastMouseButton);
				}
				icQueues.onClick.m.unlock();
				icQueues.onClick.queued = false;
			}

			if(icQueues.onEnter.queued){
				icQueues.onEnter.m.lock();
				for(uint32 i = 0; i < icQueues.onEnter.list.count(); ++i){
					if(icQueues.onEnter.list.isValid(i)) icQueues.onEnter.list[i]->onEnter(icQueues.onEnter.pos);
				}
				icQueues.onEnter.m.unlock();
				icQueues.onEnter.queued = false;
			}

			if(icQueues.onExit.queued){
				icQueues.onExit.m.lock();
				for(uint32 i = 0; i < icQueues.onExit.list.count(); ++i){
					if(icQueues.onExit.list.isValid(i)) icQueues.onExit.list[i]->onExit(icQueues.onExit.pos);
				}
				icQueues.onExit.m.unlock();
				icQueues.onExit.queued = false;
			}

			if(icQueues.onMove.queued){
				icQueues.onMove.m.lock();
				for(uint32 i = 0; i < icQueues.onMove.list.count(); ++i){
					if(icQueues.onMove.list.isValid(i)) icQueues.onMove.list[i]->onMove(icQueues.onMove.pos); //FIXME
				}
				icQueues.onMove.m.unlock();
				icQueues.onMove.queued = false;
			}

			if(icQueues.onAxis.queued){
				icQueues.onAxis.m.lock();
				for(uint32 i = 0; i < icQueues.onAxis.list.count(); ++i){
					// icQueues.onAxis.list.isValid(i)elmicQueues.onAxis.list[i]->Axis(icQueues.onAxis.pos);
					if(icQueues.onAxis.list.isValid(i)) icQueues.onAxis.list[i]->onAxis(1); //FIXME
				}
				icQueues.onAxis.m.unlock();
				icQueues.onAxis.queued = false;
			}













			// //Fix object spawn requests
			// spawn_m.lock();
			// if(spawn_q.count()){
			// 	for(uint32 i = 0; i < spawn_q.count(); ++i){
			// 		if(spawn_q.isValid(i)) spawn_q[i]->onSpawn(*this); //BUG
			// 		// if(spawn_q.isValid(i)) spawn_q[i];
			// 	}
			// 	spawn_q.clear();
			// }
			// spawn_m.unlock();




			// //Fix objects update requests
			// if(objUpdates.count() > 0) {
			// 	objUpdates_m.lock();
			// 	vk::CommandBuffer cb = core::render::cmd::beginSingleTimeCommands();
			// 	for(uint32 i = 0; i < objUpdates.count(); i++) {
			// 		objUpdates[i]->render.updated = true;
			// 		cb.updateBuffer(
			// 			objUpdates[i]->getShVData().cell->csc.buffer,
			// 			objUpdates[i]->getShVData().cell->localOffset,
			// 			objUpdates[i]->getShVData().cell->cellSize,
			// 			(void*)objUpdates[i]->getShData()
			// 		);
			// 	}
			// 	core::render::cmd::endSingleTimeCommands(cb);
			// 	objUpdates.clear();
			// 	objUpdates_m.unlock();
			// }
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








namespace lnx::core::render{
	void cleanup() {
		dvc::graphics.ld.destroyCommandPool(cmd::singleTimeCommandPool, nullptr);	//Destroy graphics command pool

		//If the compute and the graphics devices are not the same, destroy the graphics device
		// if(dvc::graphics.pd.properties.deviceID != dvc::compute.pd.properties.deviceID) dvc::graphics.ld.destroy(nullptr);
		dvc::graphics.ld.destroy(nullptr);													//Destroy the compute device
		//for (auto& device : secondary) vkDestroyDevice(device.ld, nullptr);						//Destroy all the secondary devices

		_dbg(debug::DestroyDebugUtilsMessengerEXT(dvc::instance, dvc::debugMessenger, nullptr));	//Destroy the debug messenger if present
	}








// Other ------------------------------------------------------------------------------------------------------------------------------------//







//TODO REMOVE FUNCTION. unused.
	vk::Format findSupportedFormat(const RtArray<vk::Format>* pCandidates, const vk::ImageTiling vTiling, const vk::FormatFeatureFlags vFeatures) {
		for(vk::Format format : *pCandidates) {
			auto props = dvc::graphics.pd.device.getFormatProperties(format); //Get format properties

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
		auto memProperties = dvc::graphics.pd.device.getMemoryProperties();//Get memory vProperties

		for(uint32 i = 0; i < memProperties.memoryTypeCount; ++i) {				//Search for the memory that has the specified properties and type and return its index
			if((vTypeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & vProperties) == vProperties) return i;
		}
		dbg::printError("Failed to find suitable memory type");
		return (uint32)-1;
	}
}