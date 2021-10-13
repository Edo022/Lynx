#include "Lynx/Core/Core.hpp"
#include "Lynx/Core/Render/GCommands.hpp"
#include "Lynx/Core/Render/Window/Swapchain.hpp"
#include "Lynx/Core/Render/Shaders/Shader.hpp"
#include "Lynx/Core/Render/Window/Window.hpp"
#include "Lynx/Core/Devices.hpp"
#include "Lynx/Types/Object/Obj_b.hpp"
#include <climits> //TODO
#include <chrono> //TODO
#include <mutex> //TODO
#include <thread> //TODO
//TODO parallelize work from a secondary render thread
//FIXME ADD COPY FROM RAM FUNCTTION TO VRAM ALLOCATIONS








// Init and debug ---------------------------------------------------------------------------------------------------------------------------//








namespace lnx::core::render{
	_lnx_init_var_value_def((std::mutex), graphicsQueueSubmit_m, lnx::core::render){}
	_lnx_init_var_value_def((std::mutex), presentQueueSubmit_m,  lnx::core::render){}



	#ifdef LNX_DBG
		void createDebugMessenger() {
			VkDebugUtilsMessengerCreateInfoEXT createInfo;
			debug::populateDebugMessengerCreateInfo(createInfo);
			dbg::assertCond(VK_SUCCESS == debug::CreateDebugUtilsMessengerEXT(dvc::g_instance(), &createInfo, nullptr, &dvc::g_debugMessenger()), "Failed to set up debug messenger");
		}
	#endif
}








// Recursive updates ------------------------------------------------------------------------------------------------------------------------//








namespace lnx{
	/**
	 * @brief Recursively calls the onSpawn function on pObj and its valid children and switches off their eSpawn update bit
	 *     This function should only be used by the engine
	 * Complexity: O(ΣO(f))
	 *     where f = The pObj onSpawn function and the onSpawn function of each valid children, recursively
	 * @param pObj The object from which to start the recursion
	 */
	void core::RenderCore::recSpawn(obj::obj_bb* pObj){ //FIXME USE RENDER CORE INSTEAS OF WINDOW
		//dbg::assertCond(!(render.parentWindow && thr::self::thr() != render.parentWindow->t.thr), "This function can only be called by the render thread."); //TODO ADD THREAD CHECK
		pObj->updates = pObj->updates & ~obj::eSpawn;					//Clear update bit (prevents redundant updates)
		pObj->w = w;													//Set owner window
		pObj->onSpawn(*this);											//Run user callback
		for(uint32 i = 0; i < pObj->children.count(); ++i){				//For each child
			if(pObj->children.isValid(i)) recSpawn(pObj->children[i]); 		//Run recursive update on it
		}
	}


	/**
	 * @brief Recursively calls the onFlush function on pObj and its valid children and switches off their eFlush update bit
	 *     This function should only be used by the engine
	 * Complexity: O(ΣO(f))
	 *     where f = The pObj onFlush function and the onFlush function of each valid children, recursively
	 * @param pObj The object from which to start the recursion
	 */
	void core::RenderCore::recFlush(obj::obj_bb* pObj, vk::CommandBuffer pCB){
		//dbg::assertCond(!(render.parentWindow && thr::self::thr() != render.parentWindow->t.thr), "This function can only be called by the render thread."); //TODO ADD THREAD CHECK
		pObj->updates = pObj->updates & ~obj::eFlush;
		pObj->onFlush(pCB);
		for(uint32 i = 0; i < pObj->children.count(); ++i){
			if(pObj->children.isValid(i)) recFlush(pObj->children[i], pCB);
		}
	}


	/**
	 * @brief Recursively calls the onLimit function on pObj and its valid children and switches off their eLimit update bit
	 *     This function should only be used by the engine
	 * Complexity: O(ΣO(f))
	 *     where f = The pObj onLimit function and the onLimit function of each valid children, recursively
	 * @param pObj The object from which to start the recursion
	 */
	void core::RenderCore::recLimit(obj::obj_bb* pObj){
		//dbg::assertCond(!(render.parentWindow && thr::self::thr() != render.parentWindow->t.thr), "This function can only be called by the render thread."); //TODO ADD THREAD CHECK
		pObj->updates = pObj->updates & ~obj::eLimit;
		pObj->onLimit();
		for(uint32 i = 0; i < pObj->children.count(); ++i){
			if(pObj->children.isValid(i)) recLimit(pObj->children[i]);
		}
	}








// Inizialize and clear the render core -----------------------------------------------------------------------------------------------------//








	/**
	 * @brief Initializes the render core
	 *     Creates the pipelines and the swapchain, allocates the default buffers and command buffers and creates the clear shader
	 *     This function must be called from lnx::Window::init() only
	 * Complexity: O(n + m)
	 *     where n = core::shaders::pipelineNum, m = number of swapchain images
	 */
	void core::RenderCore::init(){
		pipelines.resize(core::shaders::g_pipelineNum());
		for(uint32 i = 0; i < pipelines.count(); ++i){ //[n]
			core::shaders::createPipeline(i, w->renderCore);
		}

		swp.w = w;
		swp.create(false); //[m]




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
			createDefaultCommandBuffers(); //[m]
		}
		sh_clear.spawn(fOut_g, iOut_g, wSize_g, zBuff_g, { (w->width * w->height) / (32 * 32) + 1, 1u, 1u }, *this);
	}




	/**
	 * @brief Destroyes the swapchain an frees the resources used by the render core
	 *     This function must be called from lnx::Window::clear() only
	 * Complexity: O(n + m) [from Swapchain::clear]
	 *     where n = this->swp.images.count() and m = renderMaxFramesInFlight
	 */
	void core::RenderCore::clear(){
		swp.clear();
		wSize_g.free(); fOut_g.free(); iOut_g.free(); zBuff_g.free();
	}








	/**
	 * @brief Creates the default command buffers used for the render
	 *     This function should only be used by the enigne
	 * Complexity: O(n)
	 *     where n = this->swp.images.count()
	 */
	void core::RenderCore::createDefaultCommandBuffers() { //TODO
		{ //Render command pool
			auto commandPoolCreateInfo = vk::CommandPoolCreateInfo() 					//Create command pool create infos
				.setFlags            (vk::CommandPoolCreateFlagBits::eResetCommandBuffer)	//Command buffers and pool can be reset
				.setQueueFamilyIndex (core::dvc::g_graphics().pd.indices.computeFamilies[0])		//Set the compute family where to bind the command pool
			;
			switch(core::dvc::g_graphics().ld.createCommandPool(&commandPoolCreateInfo, nullptr, &commandPool)){ vkDefaultCases; }
		}




		{ //Copy
			auto commandPoolCreateInfo = vk::CommandPoolCreateInfo() 					//Create command pool
				.setQueueFamilyIndex (core::dvc::g_graphics().pd.indices.computeFamilies[0])	//Set the compute family where to bind the command pool
			; //FIXME
			switch(core::dvc::g_graphics().ld.createCommandPool(&commandPoolCreateInfo, nullptr, &copyCommandPool)){ vkDefaultCases; }

			auto commandBufferAllocateInfo = vk::CommandBufferAllocateInfo() 			//Allocate one command buffer for each swapchain image
				.setCommandPool        (copyCommandPool)									//Set command pool where to allocate the command buffer
				.setLevel              (vk::CommandBufferLevel::ePrimary)					//Set the command buffer as primary level command buffer
				.setCommandBufferCount (swp.images.count())									//Set command buffer count
			;
			switch(core::dvc::g_graphics().ld.allocateCommandBuffers(&commandBufferAllocateInfo, copyCommandBuffers.begin())){ vkDefaultCases; }




			//Record a present command buffers for each swapchain image
			for(uint32 imgIndex = 0; imgIndex < swp.images.count(); imgIndex++) {
				auto beginInfo = vk::CommandBufferBeginInfo() 							//Simultaneous use allows the command buffer to be executed multiple times
					.setFlags (vk::CommandBufferUsageFlagBits::eSimultaneousUse)
				;
				//Start recording commands
				switch(copyCommandBuffers[imgIndex].begin(&beginInfo)){ vkDefaultCases; }
					//Create a barrier to use the swapchain image as an optimal transfer destination to copy the buffer in it
					readToWriteBarrier.image = swp.images[imgIndex].image;					//Set swapchain image
					vk::PipelineStageFlags 													//Create stage flags
						srcStage = vk::PipelineStageFlagBits::eColorAttachmentOutput,			//The swapchain image is in color output stage
						dstStage = vk::PipelineStageFlagBits::eTransfer;						//Change it to transfer stage to copy the buffer in it
					copyCommandBuffers[imgIndex].pipelineBarrier(srcStage, dstStage, vk::DependencyFlagBits::eDeviceGroup, 0, nullptr, 0, nullptr, 1, &readToWriteBarrier);
					//! ^ https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkDependencyFlagBits.html //FIXME dependency flags was 0 but C++ doesn't allow that

					copyRegion.imageExtent = vk::Extent3D{ swp.createInfo.imageExtent.width, swp.createInfo.imageExtent.height, 1 };	//Copy the whole buffer
					copyCommandBuffers[imgIndex].copyBufferToImage(iOut_g.cell->csc.buffer, swp.images[imgIndex].image, vk::ImageLayout::eTransferDstOptimal, 1, &copyRegion);


					//Create a barrier to use the swapchain image as a present source image
					writeToReadBarrier.image = swp.images[imgIndex].image;					//Set swapchain image
					vk::PipelineStageFlags 													//Create stage flags
						srcStage1 = vk::PipelineStageFlagBits::eTransfer,						//The image is in transfer stage from the buffer copy
						dstStage1 = vk::PipelineStageFlagBits::eColorAttachmentOutput;			//Change it to color output to present them
					copyCommandBuffers[imgIndex].pipelineBarrier(srcStage1, dstStage1, vk::DependencyFlagBits::eDeviceGroup, 0, nullptr, 0, nullptr, 1, &writeToReadBarrier);
					//! ^ https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkDependencyFlagBits.html //FIXME dependency flags was 0 but C++ doesn't allow that
				switch(copyCommandBuffers[imgIndex].end()){ vkDefaultCases; }										//End command buffer recording
			}
		}
	}








// Render helper functions ------------------------------------------------------------------------------------------------------------------//








	/**
	 * @brief Presents an image
	 *     This function should only be used by the engine
	 * Complexity: O(1)
	 * @param imageIndex The index of the image to present
	 * @return The return value of the vk::Queue::presentKHR call
	 */
	vk::Result core::RenderCore::present(uint32& imageIndex){
		const auto presentInfo = vk::PresentInfoKHR()
			.setWaitSemaphoreCount (1)
			.setPWaitSemaphores    (&swp.frames[swp.curFrame].s_copy)
			.setSwapchainCount     (1)
			.setPSwapchains        (&swp.swapchain)
			.setPImageIndices      (&imageIndex)
		;
		core::render::g_presentQueueSubmit_m().lock();
		auto r = core::dvc::g_graphics().pq.presentKHR(presentInfo); //TODO graphics and present queues could be the same, in some devices. In that case, use the same mutex
		core::render::g_presentQueueSubmit_m().unlock();
		return r;
	}








	/**
	 * @brief Renders a single frame and replaces the image at index vImgIndex with it
	 *     This function should only be used by the engine
	 * Complexity: O(1) [GPU complexity depends on the render]
	 * @param vImgIndex The index of the image to present and replace
	 */
	void core::RenderCore::draw(uint32& vImgIndex){
		switch(core::dvc::g_graphics().ld.waitForFences(1, &swp.frames[swp.curFrame].f_rendered, false, LONG_MAX)){
			case vk::Result::eTimeout:         dbg::logError("Fence timed out"); break;
			case vk::Result::eErrorDeviceLost: dbg::logError("Device lost");     break;
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
			switch(core::dvc::g_graphics().ld.acquireNextImageKHR(swp.swapchain, UINT64_MAX, swp.frames[swp.curFrame].s_aquired, nullptr, &vImgIndex)) {
				case vk::Result::eTimeout:             dbg::logWarn("Timeout");    break;
				case vk::Result::eNotReady:            dbg::logWarn("Not ready");  break;
				case vk::Result::eSuboptimalKHR:       dbg::logWarn("Suboptimal"); break;
				case vk::Result::eErrorOutOfDateKHR:   swp.recreate(); goto redraw;
				case vk::Result::eErrorDeviceLost:     dbg::logError("Device lost");  break;
				case vk::Result::eErrorSurfaceLostKHR: dbg::logError("Surface lost"); break;
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
				.setPCommandBuffers      (&copyCommandBuffers[vImgIndex])
				.setSignalSemaphoreCount (1)
				.setPSignalSemaphores    (&swp.frames[swp.curFrame].s_copy)
			,
		};
		addObject_m.unlock(); //FIXME

		switch(core::dvc::g_graphics().ld.resetFences(1, &swp.frames[swp.curFrame].f_rendered)){
			case vk::Result::eSuccess: break;
			case vk::Result::eErrorOutOfDeviceMemory: dbg::logError("Out of device memory"); break;
			// case vk::Result::eErrorOutOfHostMemory: dbg::logError("Out of host memory"); break;
			//!^ Not an error. This value is not returned
			default: dbg::logError("Unknown result");
		}

		core::render::g_graphicsQueueSubmit_m().lock();
		switch(core::dvc::g_graphics().cqs[0].submit(3, submitInfos, swp.frames[swp.curFrame].f_rendered)){ vkDefaultCases; }
		core::render::g_graphicsQueueSubmit_m().unlock();
	}








	/**
	 * @brief Updates the objects that got queued during the last frame render
	 *     This function should only be used by the engine
	 * Complexity: O(n)
	 *     where n = this->requests.count()
	 */
	void core::RenderCore::updateObjects(){
		vk::CommandBuffer cb = core::render::cmd::beginSingleTimeCommands(); //FIXME USE RENDER QUEUE
		requests_m.lock();
		if(!requests.empty()) for(auto r : requests){
			if(r->updates & obj::UpdateBits::eSpawn) recSpawn(r);
			if(r->updates & obj::UpdateBits::eLimit) recLimit(r);
			if(r->updates & obj::UpdateBits::eFlush) recFlush(r, cb);
			_dbg(if(r->updates != obj::eNone) dbg::logWarn("Non-0 value detected for render.updates after update loop. This may indicate a race condition or a bug in the engine"));
		}
		requests.clear();
		requests_m.unlock();
		core::render::cmd::endSingleTimeCommands(cb);
	}








	/**
	 * @brief Sends the respective input callbacks to each of the objects in the input callbacks lists
	 *     This function should only be used by the engine
	 * Complexity: O(Σn)
	 *     where n = number of elements in each input queue
	 */
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








	/**
	 * @brief Starts the render core
	 *     This function should only be used by the engine
	 * Complexity: Unknown [function may not return during program execution]
	 */
	void core::RenderCore::run(){
		_dbg(thr::self::setName("App | Render"));
		renderLoop();
		w->clear();
		pthread_exit(nullptr);
	}




	/**
	 * @brief Until the core is terminated, looping between the swapchain images:
	 *     Presents the image and renders a new one, then updates the objects and sends the input callbacks
	 *     This function should only be used by the engine
	 * Complexity: Unknown [function may not return during program execution]
	 */
	void core::RenderCore::renderLoop() {
		auto last = std::chrono::high_resolution_clock::now();
		// running = true;
		w->running = true; //FIXME check if this is the correct variable
		while(g_running()) {
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
				case vk::Result::eSuboptimalKHR: dbg::logWarn("Suboptimal"); break;
				// case vk::Result::eErrorOutOfDateKHR:   swp.recreate(); goto redraw;
				case vk::Result::eErrorOutOfDateKHR:   swp.recreate(); goto redraw;
				case vk::Result::eErrorDeviceLost:     dbg::logError("Device lost");  break;
				case vk::Result::eErrorSurfaceLostKHR: dbg::logError("Surface lost"); break;
				#ifdef _WIN64 //This error is unique to windows
					case vk::Result::eErrorFullScreenExclusiveModeLostEXT: //FIXME
				#endif
				vkDefaultCases;
			}


			//Update frame number and flush the window data
			swp.curFrame = (swp.curFrame + 1) % lnx::core::wnd::renderMaxFramesInFlight;
			glfwSwapBuffers(w->window);

			updateObjects();
			sendInputCallbacks();


			//Calculate render duration
			//TODO add detailed timings output
			auto end = std::chrono::high_resolution_clock::now();
			auto duration = duration_cast<std::chrono::microseconds>(end - start);
			if(duration_cast<std::chrono::seconds>(end - last).count() >= 1){
				dbg::logInfo("%6.1f fps, %.2f ms", 1.0f/(((float)duration.count())/1000/1000), (((float)duration.count())/1000));
				last = end;
			}


			//Stop render if window was closed
			// if(glfwWindowShouldClose(w->window)) running = false;
			if(glfwWindowShouldClose(w->window)) w->running = false; //FIXME check if this is the correct variable
		}
	}
}








// Clean memory -----------------------------------------------------------------------------------------------------------------------------//








/**
 * @brief
 *
 */
namespace lnx::core::render{
	/**
	 * @brief //TODO
	 */
	void cleanup() { //FIXME MOVE  COMMAND POOL TO RENDER CORE
		dvc::g_graphics().ld.destroyCommandPool(cmd::g_singleTimeCommandPool(), nullptr);	//Destroy graphics command pool //FIXME MOVE TO RENDER CORE
		dvc::g_graphics().ld.destroy(nullptr);													//Destroy the compute device //FIXME ONLY DESTROY WHEN CLOSING THE ENGINE
		//for (auto& device : secondary) vkDestroyDevice(device.ld, nullptr);						//Destroy all the secondary devices

		_dbg(debug::DestroyDebugUtilsMessengerEXT(dvc::g_instance(), dvc::g_debugMessenger(), nullptr));	//Destroy the debug messenger if present //FIXME ONLY DESTROY WHEN CLOSING THE ENGINE
	}








// Other ------------------------------------------------------------------------------------------------------------------------------------//








	/**
	 * @brief Returns the index of the memory type with the specified properties
	 *     This function should only be used by the engine
	 * Complexity:
	 *     Best:  O(1)
	 *     Worst: O(n)
	 *     where n = number of available memory types
	 * @param vType The type of the memory
	 * @param vProp The required memory properties
	 * @return The index of the memory type if one with the specified properties was found, (uint32)-1 otherwise
	 */
	uint32 findMemoryType(const uint32 vType, const vk::MemoryPropertyFlags vProp) {
		auto memProperties = dvc::g_graphics().pd.device.getMemoryProperties();		//Get memory vProperties

		for(uint32 i = 0; i < memProperties.memoryTypeCount; ++i) {				//Search for the memory that has the specified properties and type and return its index
			if((vType & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & vProp) == vProp) return i;
		}
		return (uint32)-1;
	}
}