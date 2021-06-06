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
	void core::RenderCore::recSpawn(obj::Obj_bb* pObj, Window& pWindow){ //FIXME USE RENDER CORE INSTEAS OF WINDOW
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
		pipelines.resize(core::shaders::pipelineNum);
		for(uint32 i = 0; i < pipelines.count(); ++i){
			core::shaders::createPipeline(i, *w);
		}

		swp.bindedWindow = w;
		swp.create(false);




		{ //Initialize window buffers and count
			iOut_g. realloc(1920*2 * 1080 * 4);			//A8  R8  G8  B8  UI
			fOut_g. realloc(1920*2 * 1080 * 4 * 4);		//A32 R32 G32 B32 UF
			zBuff_g.realloc(1920*2 * 1080 * 4);			//A8  R8  G8  B8  UI
			//FIXME ^ those allocations use the default maximum window size to prevent the buffer from getting resized too often
			//FIXME detect size at runtime
			wSize_g.realloc(/*4 * 2*/16);				//Create cell for window size
			//FIXME rounded up to a multiple of 16, make it automatic

			u32v2 wSize = { swp.createInfo.imageExtent.width, swp.createInfo.imageExtent.height };
			vk::CommandBuffer cb = core::render::cmd::beginSingleTimeCommands();
			cb.updateBuffer(wSize_g.cell->csc.buffer, wSize_g.cell->localOffset, wSize_g.cell->cellSize, &wSize);
			core::render::cmd::endSingleTimeCommands(cb);
			//FIXME AUTOMATIZE BUFFER UPDATE
			//FIXME UPDATE ALL BUFFERS TOGETHER AFTER A FRAME IS RENDERED
		}
		{ //#LLID CCB0000 Create copy command buffers
			copyCommandBuffers.resize(swp.images.count());	//Resize the command buffer array in the shader
			createDefaultCommandBuffers__();
		}
		sh_clear.create(fOut_g, iOut_g, wSize_g, zBuff_g, { (w->width * w->height) / (32 * 32) + 1, 1u, 1u }, *w);
	}

	void core::RenderCore::clear(){
		swp.clear();
		wSize_g.free(); fOut_g.free(); iOut_g.free(); zBuff_g.free();
	}








	//Creates the default command buffers used for the render
	void core::RenderCore::createDefaultCommandBuffers__() { //TODO
		{ //Render command pool
			auto commandPoolCreateInfo = vk::CommandPoolCreateInfo() 					//Create command pool create infos
				.setFlags            (vk::CommandPoolCreateFlagBits::eResetCommandBuffer)	//Command buffers and pool can be reset
				.setQueueFamilyIndex (core::dvc::graphics.pd.indices.computeFamilies[0])		//Set the compute family where to bind the command pool
			;
			switch(core::dvc::graphics.ld.createCommandPool(&commandPoolCreateInfo, nullptr, &commandPool)){ vkDefaultCases; }
		}




		{ //Copy
			auto commandPoolCreateInfo = vk::CommandPoolCreateInfo() 					//Create command pool
				.setQueueFamilyIndex (core::dvc::graphics.pd.indices.computeFamilies[0])	//Set the compute family where to bind the command pool
			; //FIXME
			switch(core::dvc::graphics.ld.createCommandPool(&commandPoolCreateInfo, nullptr, &copyCommandPool)){ vkDefaultCases; }

			auto commandBufferAllocateInfo = vk::CommandBufferAllocateInfo() 			//Allocate one command buffer for each swapchain image
				.setCommandPool        (copyCommandPool)									//Set command pool where to allocate the command buffer
				.setLevel              (vk::CommandBufferLevel::ePrimary)					//Set the command buffer as primary level command buffer
				.setCommandBufferCount (swp.images.count())									//Set command buffer count
			;
			switch(core::dvc::graphics.ld.allocateCommandBuffers(&commandBufferAllocateInfo, copyCommandBuffers.begin())){ vkDefaultCases; }




			//Record a present command buffers for each swapchain images
			for(uint32 imgIndex = 0; imgIndex < swp.images.count(); imgIndex++) {
				auto beginInfo = vk::CommandBufferBeginInfo() 							//Simultaneous use allows the command buffer to be executed multiple times
					.setFlags (vk::CommandBufferUsageFlagBits::eSimultaneousUse)
				;
				//Start recording commands
				switch(copyCommandBuffers[imgIndex].begin(&beginInfo)){ vkDefaultCases; }
					//Create a barrier to use the swapchain image as an optimal transfer destination to copy the buffer in it
					w->readToWriteBarrier.image = swp.images[imgIndex].image;					//Set swapchain image
					vk::PipelineStageFlags 													//Create stage flags
						srcStage = vk::PipelineStageFlagBits::eColorAttachmentOutput,			//The swapchain image is in color output stage
						dstStage = vk::PipelineStageFlagBits::eTransfer;						//Change it to transfer stage to copy the buffer in it
					copyCommandBuffers[imgIndex].pipelineBarrier(srcStage, dstStage, vk::DependencyFlagBits::eDeviceGroup, 0, nullptr, 0, nullptr, 1, &w->readToWriteBarrier);
					//! ^ https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkDependencyFlagBits.html //FIXME dependency flags was 0 but C++ doesn't allow that

					w->copyRegion.imageExtent = vk::Extent3D{ swp.createInfo.imageExtent.width, swp.createInfo.imageExtent.height, 1 };	//Copy the whole buffer
					copyCommandBuffers[imgIndex].copyBufferToImage(iOut_g.cell->csc.buffer, swp.images[imgIndex].image, vk::ImageLayout::eTransferDstOptimal, 1, &w->copyRegion);


					//Create a barrier to use the swapchain image as a present source image
					w->writeToReadBarrier.image = swp.images[imgIndex].image;					//Set swapchain image
					vk::PipelineStageFlags 													//Create stage flags
						srcStage1 = vk::PipelineStageFlagBits::eTransfer,						//The image is in transfer stage from the buffer copy
						dstStage1 = vk::PipelineStageFlagBits::eColorAttachmentOutput;			//Change it to color output to present them
					copyCommandBuffers[imgIndex].pipelineBarrier(srcStage1, dstStage1, vk::DependencyFlagBits::eDeviceGroup, 0, nullptr, 0, nullptr, 1, &w->writeToReadBarrier);
					//! ^ https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkDependencyFlagBits.html //FIXME dependency flags was 0 but C++ doesn't allow that
				switch(copyCommandBuffers[imgIndex].end()){ vkDefaultCases; }										//End command buffer recording
			}
		}
	}








// Render helper functions ------------------------------------------------------------------------------------------------------------------//








	vk::Result core::RenderCore::present(uint32& imageIndex){
		const auto presentInfo = vk::PresentInfoKHR()
			.setWaitSemaphoreCount (1)
			.setPWaitSemaphores    (&swp.frames[swp.curFrame].s_copy)
			.setSwapchainCount     (1)
			.setPSwapchains        (&swp.swapchain)
			.setPImageIndices      (&imageIndex)
		;
		core::render::presentQueueSubmit_m.lock();
		auto r = core::dvc::graphics.pq.presentKHR(presentInfo); //TODO graphics and present queues could be the same, in some devices. In that case, use the same mutex
		core::render::presentQueueSubmit_m.unlock();
		return r;
	}







	void core::RenderCore::draw(uint32& imageIndex){
		switch(core::dvc::graphics.ld.waitForFences(1, &swp.frames[swp.curFrame].f_rendered, false, LONG_MAX)){
			case vk::Result::eTimeout:         dbg::printError("Fence timed out"); break;
			case vk::Result::eErrorDeviceLost: dbg::printError("Device lost");     break;
			vkDefaultCases;
		}


		//Redraw frame if necessary
		redraw:
		if(swp.resized) {
			swp.resized = false;
			swp.recreate();
			goto redraw;
		}


		//Acquire swapchain image
		{
			switch(core::dvc::graphics.ld.acquireNextImageKHR(swp.swapchain, UINT64_MAX, swp.frames[swp.curFrame].s_aquired, nullptr, &imageIndex)) {
				case vk::Result::eTimeout:             dbg::printWarning("Timeout");    break;
				case vk::Result::eNotReady:            dbg::printWarning("Not ready");  break;
				case vk::Result::eSuboptimalKHR:       dbg::printWarning("Suboptimal"); break;
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
		switch(core::dvc::graphics.cqs[0].submit(3, submitInfos, swp.frames[swp.curFrame].f_rendered)){ vkDefaultCases; }
		core::render::graphicsQueueSubmit_m.unlock();
	}






	void core::RenderCore::updateObjects(){
		vk::CommandBuffer cb = core::render::cmd::beginSingleTimeCommands(); //FIXME USE RENDER QUEUE
		requests_m.lock();
		if(!requests.empty()) for(auto r : requests){
			if(r->render.updates & obj::UpdateBits::eSpawn) recSpawn(r, *w);
			if(r->render.updates & obj::UpdateBits::eLimit) recLimit(r);
			if(r->render.updates & obj::UpdateBits::eUpdateg) recUpdateg(r, cb);
			_dbg(if(r->render.updates != obj::eNone) dbg::printWarning("Non-0 value detected for render.updates after update loop. This may indicate a race condition or a bug in the engine"));
		}
		requests.clear();
		requests_m.unlock();
		core::render::cmd::endSingleTimeCommands(cb);
	}






	void core::RenderCore::sendInputCallbacks(){
		//FIXME REQUESTS SENT TO NON SPAWNED OBJECTS FROM INPUT CALLBACKS ARE EXECUTED DURING SPAWN
		//Input callbacks
		if(w->icQueues.onClick.queued){
			w->icQueues.onClick.m.lock();
			for(uint32 i = 0; i < w->icQueues.onClick.list.count(); ++i){
				if(w->icQueues.onClick.list.isValid(i)) w->icQueues.onClick.list[i]->onClick(w->icQueues.onClick.pos, w->icQueues.lastMouseButton);
			}
			w->icQueues.onClick.m.unlock();
			w->icQueues.onClick.queued = false;
		}

		if(w->icQueues.onEnter.queued){
			w->icQueues.onEnter.m.lock();
			for(uint32 i = 0; i < w->icQueues.onEnter.list.count(); ++i){
				if(w->icQueues.onEnter.list.isValid(i)) w->icQueues.onEnter.list[i]->onEnter(w->icQueues.onEnter.pos);
			}
			w->icQueues.onEnter.m.unlock();
			w->icQueues.onEnter.queued = false;
		}

		if(w->icQueues.onExit.queued){
			w->icQueues.onExit.m.lock();
			for(uint32 i = 0; i < w->icQueues.onExit.list.count(); ++i){
				if(w->icQueues.onExit.list.isValid(i)) w->icQueues.onExit.list[i]->onExit(w->icQueues.onExit.pos);
			}
			w->icQueues.onExit.m.unlock();
			w->icQueues.onExit.queued = false;
		}

		if(w->icQueues.onMove.queued){
			w->icQueues.onMove.m.lock();
			for(uint32 i = 0; i < w->icQueues.onMove.list.count(); ++i){
				if(w->icQueues.onMove.list.isValid(i)) w->icQueues.onMove.list[i]->onMove(w->icQueues.onMove.pos); //FIXME
			}
			w->icQueues.onMove.m.unlock();
			w->icQueues.onMove.queued = false;
		}

		if(w->icQueues.onAxis.queued){
			w->icQueues.onAxis.m.lock();
			for(uint32 i = 0; i < w->icQueues.onAxis.list.count(); ++i){
				if(w->icQueues.onAxis.list.isValid(i)) w->icQueues.onAxis.list[i]->onAxis(1); //FIXME
			}
			w->icQueues.onAxis.m.unlock();
			w->icQueues.onAxis.queued = false;
		}
	}








// Main render loop -------------------------------------------------------------------------------------------------------------------------//








	void core::RenderCore::run(){
		_dbg(thr::self::setName("App | Render"));
		renderLoop();
		w->clear();
		pthread_exit(nullptr);
	}



	void core::RenderCore::renderLoop() {
		auto last = std::chrono::high_resolution_clock::now();
		running = true;
		while(running) {
			sleep(0); //Prevent extra overhead when no object has to be rendered
			auto start = std::chrono::high_resolution_clock::now();
			addObject_m.lock();
			if(swp.shadersCBs.count() <= 0) {
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
				case vk::Result::eErrorOutOfDateKHR:   swp.recreate(); goto redraw;
				case vk::Result::eErrorDeviceLost:     dbg::printError("Device lost");  break;
				case vk::Result::eErrorSurfaceLostKHR: dbg::printError("Surface lost"); break;
				#ifdef _WIN64 //This error is unique to windows
					case vk::Result::eErrorFullScreenExclusiveModeLostEXT: //FIXME
				#endif
				vkDefaultCases;
			}


			//Update frame number and flush the window data
			swp.curFrame = (swp.curFrame + 1) % lnx::core::wnd::__renderMaxFramesInFlight;
			glfwSwapBuffers(w->window);

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
			if(glfwWindowShouldClose(w->window)) running = false;
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