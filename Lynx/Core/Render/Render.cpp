#include "Lynx/Core/Core.hpp"
#include "Lynx/Core/Render/GCommands.hpp"
#include "Lynx/Core/Render/Window/Swapchain.hpp"
#include "Lynx/Core/Render/Shaders/Shader.hpp"
#include "Lynx/Core/Devices.hpp"
#include "Lynx/Types/Object/2D/Obj2_b.hpp"
#include <climits>
#include <chrono>
//TODO parallelize work from a secondary render thread
//FIXME ADD COPY FROM RAM FUNCTTION TO VRAM ALLOCATIONS





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








// Recursive updates ------------------------------------------------------------------------------------------------------------------------//








namespace lnx{
	void core::RenderCore::recSpawn(obj::Obj_bb* pObj, Window& pWindow){
		//dbg::checkCond(render.parentWindow && thr::self::thr() != render.parentWindow->t.thr, "This function can only be called by the render thread."); //TODO ADD THREAD CHECK
		pObj->render.updates = pObj->render.updates & ~obj::eSpawn;			//Clear update bit (prevents redundant updates)
		pObj->render.parentWindow = &pWindow;								//Set owner window
		pObj->onSpawn(pWindow);												//Run user callback
		for(uint32 i = 0; i < pObj->getChildrenCount(); ++i){				//For each child
			if(pObj->getChildrenIsValid(i)) recSpawn(pObj->getChildren(i), pWindow); //Run recursive update on it
		}
	}

	void core::RenderCore::recUpdateg(obj::Obj_bb* pObj, vk::CommandBuffer pCB){
		//dbg::checkCond(render.parentWindow && thr::self::thr() != render.parentWindow->t.thr, "This function can only be called by the render thread."); //TODO ADD THREAD CHECK
		pObj->render.updates = pObj->render.updates & ~obj::eUpdateg;
		pObj->onUpdateg(pCB);
		for(uint32 i = 0; i < pObj->getChildrenCount(); ++i){
			if(pObj->getChildrenIsValid(i)) recUpdateg(pObj->getChildren(i), pCB);
		}
	}

	void core::RenderCore::recLimit(obj::Obj_bb* pObj){
		//dbg::checkCond(render.parentWindow && thr::self::thr() != render.parentWindow->t.thr, "This function can only be called by the render thread."); //TODO ADD THREAD CHECK
		pObj->render.updates = pObj->render.updates & ~obj::eLimit;
		pObj->onLimit();
		for(uint32 i = 0; i < pObj->getChildrenCount(); ++i){
			if(pObj->getChildrenIsValid(i)) recLimit(pObj->getChildren(i));
		}
	}








// A ----------------------------------------------------------------------------------------------------------------------------------------//







	void core::RenderCore::init(){
		{ //Initialize window buffers and count
			iOut_g. realloc(1920*2 * 1080 * 4);			//A8  R8  G8  B8  UI
			fOut_g. realloc(1920*2 * 1080 * 4 * 4);		//A32 R32 G32 B32 UF
			zBuff_g.realloc(1920*2 * 1080 * 4);			//A8  R8  G8  B8  UI
			//FIXME ^ those allocations use the default maximum window size to prevent the buffer from getting resized too often
			//FIXME detect size at runtime
			wSize_g.realloc(/*4 * 2*/16);				//Create cell for window size
			//FIXME rounded up to a multiple of 16, make it automatic

			u32v2 wSize = { w->renderCore.swp.createInfo.imageExtent.width, w->renderCore.swp.createInfo.imageExtent.height };
			vk::CommandBuffer cb = core::render::cmd::beginSingleTimeCommands();
			cb.updateBuffer(wSize_g.cell->csc.buffer, wSize_g.cell->localOffset, wSize_g.cell->cellSize, &wSize);
			core::render::cmd::endSingleTimeCommands(cb);
			//FIXME AUTOMATIZE BUFFER UPDATE
			//FIXME UPDATE ALL BUFFERS TOGETHER AFTER A FRAME IS RENDERED
		}
		{ //#LLID CCB0000 Create copy command buffers
			copyCommandBuffers.resize(w->renderCore.swp.images.count());	//Resize the command buffer array in the shader
			w->createDefaultCommandBuffers__();
		}
		sh_clear.create(fOut_g, iOut_g, wSize_g, zBuff_g, { (w->width * w->height) / (32 * 32) + 1, 1u, 1u }, *w);
	}

	void core::RenderCore::clear(){
		wSize_g.free(); fOut_g.free(); iOut_g.free(); zBuff_g.free();
	}









// Render helper functions ------------------------------------------------------------------------------------------------------------------//








	vk::Result Window::present(uint32& imageIndex){
		const auto presentInfo = vk::PresentInfoKHR()
			.setWaitSemaphoreCount (1)
			.setPWaitSemaphores    (&renderCore.swp.frames[renderCore.swp.curFrame].s_copy)
			.setSwapchainCount     (1)
			.setPSwapchains        (&renderCore.swp.swapchain)
			.setPImageIndices      (&imageIndex)
		;
		core::render::presentQueueSubmit_m.lock();
		auto r = core::dvc::graphics.pq.presentKHR(presentInfo); //TODO graphics and present queues could be the same, in some devices. In that case, use the same mutex
		core::render::presentQueueSubmit_m.unlock();
		return r;
	}







	void Window::draw(uint32& imageIndex){
		switch(core::dvc::graphics.ld.waitForFences(1, &renderCore.swp.frames[renderCore.swp.curFrame].f_rendered, false, LONG_MAX)){
			case vk::Result::eTimeout:         dbg::printError("Fence timed out"); break;
			case vk::Result::eErrorDeviceLost: dbg::printError("Device lost");     break;
			vkDefaultCases;
		}


		//Redraw frame if necessary
		redraw:
		if(renderCore.swp.resized) {
			renderCore.swp.resized = false;
			renderCore.swp.recreate();
			goto redraw;
		}


		//Acquire swapchain image
		{
			switch(core::dvc::graphics.ld.acquireNextImageKHR(renderCore.swp.swapchain, UINT64_MAX, renderCore.swp.frames[renderCore.swp.curFrame].s_aquired, nullptr, &imageIndex)) {
				case vk::Result::eTimeout:             dbg::printWarning("Timeout");    break;
				case vk::Result::eNotReady:            dbg::printWarning("Not ready");  break;
				case vk::Result::eSuboptimalKHR:       dbg::printWarning("Suboptimal"); break;
				case vk::Result::eErrorOutOfDateKHR:   renderCore.swp.recreate(); goto redraw;
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
				.setPWaitSemaphores      (&renderCore.swp.frames[renderCore.swp.curFrame].s_aquired)
				.setPWaitDstStageMask    (waitStages)
				.setCommandBufferCount   (renderCore.swp.shadersCBs.count())
				.setPCommandBuffers      (renderCore.swp.shadersCBs.begin())
				.setSignalSemaphoreCount (1)
				.setPSignalSemaphores    (&renderCore.swp.frames[renderCore.swp.curFrame].s_objects)
			,
			vk::SubmitInfo() //Convert and clear shader
				.setWaitSemaphoreCount   (1)
				.setPWaitSemaphores      (&renderCore.swp.frames[renderCore.swp.curFrame].s_objects)
				.setPWaitDstStageMask    (waitStages)
				.setCommandBufferCount   (1)
				.setPCommandBuffers      (&renderCore.sh_clear.commandBuffers[0])
				.setSignalSemaphoreCount (1)
				.setPSignalSemaphores    (&renderCore.swp.frames[renderCore.swp.curFrame].s_clear)
			,
			vk::SubmitInfo() //Copy shader
				.setWaitSemaphoreCount   (1)
				.setPWaitSemaphores      (&renderCore.swp.frames[renderCore.swp.curFrame].s_clear)
				.setPWaitDstStageMask    (waitStages)
				.setCommandBufferCount   (1)
				.setPCommandBuffers      (&renderCore.copyCommandBuffers[imageIndex])
				.setSignalSemaphoreCount (1)
				.setPSignalSemaphores    (&renderCore.swp.frames[renderCore.swp.curFrame].s_copy)
			,
		};
		addObject_m.unlock(); //FIXME

		switch(core::dvc::graphics.ld.resetFences(1, &renderCore.swp.frames[renderCore.swp.curFrame].f_rendered)){
			case vk::Result::eSuccess: break;
			case vk::Result::eErrorOutOfDeviceMemory: dbg::printError("Out of device memory"); break;
			// case vk::Result::eErrorOutOfHostMemory: dbg::printError("Out of host memory"); break;
			//!^ Not an error. This value is not returned
			default: dbg::printError("Unknown result");
		}

		core::render::graphicsQueueSubmit_m.lock();
		switch(core::dvc::graphics.cqs[0].submit(3, submitInfos, renderCore.swp.frames[renderCore.swp.curFrame].f_rendered)){ vkDefaultCases; }
		core::render::graphicsQueueSubmit_m.unlock();
	}






	void Window::updateObjects(){
		vk::CommandBuffer cb = core::render::cmd::beginSingleTimeCommands(); //FIXME USE RENDER QUEUE
		requests_m.lock();
		if(!requests.empty()) for(auto r : requests){
			if(r->render.updates & obj::UpdateBits::eSpawn) renderCore.recSpawn(r, *this);
			if(r->render.updates & obj::UpdateBits::eLimit) renderCore.recLimit(r);
			if(r->render.updates & obj::UpdateBits::eUpdateg) renderCore.recUpdateg(r, cb);
			_dbg(if(r->render.updates != obj::eNone) dbg::printWarning("Non-0 value detected for render.updates after update loop. This may indicate a race condition or a bug in the engine"));
		}
		requests.clear();
		requests_m.unlock();
		core::render::cmd::endSingleTimeCommands(cb);
	}






	void Window::sendInputCallbacks(){
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
				if(icQueues.onAxis.list.isValid(i)) icQueues.onAxis.list[i]->onAxis(1); //FIXME
			}
			icQueues.onAxis.m.unlock();
			icQueues.onAxis.queued = false;
		}
	}








// Main render loop -------------------------------------------------------------------------------------------------------------------------//








	void Window::renderLoop() {
		auto last = std::chrono::high_resolution_clock::now();
		running = true;
		while(running) {
			sleep(0); //Prevent extra overhead when no object has to be rendered
			auto start = std::chrono::high_resolution_clock::now();
			addObject_m.lock();
			if(renderCore.swp.shadersCBs.count() <= 0) {
				addObject_m.unlock();
				continue;
			}
			addObject_m.unlock();

			uint32 imageIndex;
			redraw:
			draw(imageIndex);
			switch(present(imageIndex)){
				case vk::Result::eSuboptimalKHR: dbg::printWarning("Suboptimal"); break;
				// case vk::Result::eErrorOutOfDateKHR:   swp.recreate(); goto redraw;
				case vk::Result::eErrorOutOfDateKHR:   renderCore.swp.recreate(); goto redraw;
				case vk::Result::eErrorDeviceLost:     dbg::printError("Device lost");  break;
				case vk::Result::eErrorSurfaceLostKHR: dbg::printError("Surface lost"); break;
				#ifdef _WIN64 //This error is unique to windows
					case vk::Result::eErrorFullScreenExclusiveModeLostEXT: //FIXME
				#endif
				vkDefaultCases;
			}


			//Update frame number and flush the window data
			renderCore.swp.curFrame = (renderCore.swp.curFrame + 1) % lnx::core::wnd::__renderMaxFramesInFlight;
			glfwSwapBuffers(window);

			updateObjects();
			sendInputCallbacks();


			//Calculate render duration
			auto end = std::chrono::high_resolution_clock::now();
			auto duration = duration_cast<std::chrono::microseconds>(end - start);
			if(duration_cast<std::chrono::seconds>(end - last).count() >= 1){
				std::cout << "\nFPS: " << 1.0f/(((float)duration.count())/(1000*1000));
				std::cout << "\n" << duration.count();
				last = end;
			}


			//Stop render if window was closed
			if(glfwWindowShouldClose(window)) running = false;
		}
	}
}








// Clean memory -----------------------------------------------------------------------------------------------------------------------------//








namespace lnx::core::render{
	void cleanup() { //FIXME MOVE  COMMAND POOL TO RENDER CORE
		dvc::graphics.ld.destroyCommandPool(cmd::singleTimeCommandPool, nullptr);	//Destroy graphics command pool //FIXME MOVE TO RENDER CORE
		dvc::graphics.ld.destroy(nullptr);													//Destroy the compute device //FIXME ONLY DESTROY WHEN CLOSING THE ENGINE
		//for (auto& device : secondary) vkDestroyDevice(device.ld, nullptr);						//Destroy all the secondary devices

		_dbg(debug::DestroyDebugUtilsMessengerEXT(dvc::instance, dvc::debugMessenger, nullptr));	//Destroy the debug messenger if present //FIXME ONLY DESTROY WHEN CLOSING THE ENGINE
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