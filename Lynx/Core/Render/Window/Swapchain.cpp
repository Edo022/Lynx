#include "Lynx/Core/Render/Window/Window.hpp"
#include "Lynx/Core/Render/Window/Swapchain.hpp"
#include "Lynx/Core/Render/Shaders/Shader.hpp"
#include "Lynx/Core/Render/GCommands.hpp"
#include "Lynx/Core/Devices.hpp"
#include "Lynx/Core/Core.hpp"








namespace lnx::core::wnd{
	/**
	 * @brief Constructor. Resizes the frames array and creates the synchronization objects
	 *     This function should only be used by the engine
	 * Complexity: O(n)
	 *     where n = __renderMaxFramesInFlight
	 */
	Swapchain::Swapchain(){
		frames.resize(__renderMaxFramesInFlight);

		//Create sync objects
		auto semaphoreInfo = vk::SemaphoreCreateInfo();
		auto fenceInfo = vk::FenceCreateInfo().setFlags(vk::FenceCreateFlagBits::eSignaled);
		for(uint32 i = 0; i < __renderMaxFramesInFlight; ++i) {
			switch(dvc::graphics.ld.createSemaphore(&semaphoreInfo, nullptr, &frames[i].s_aquired )){ vkDefaultCases; }
			switch(dvc::graphics.ld.createSemaphore(&semaphoreInfo, nullptr, &frames[i].s_objects )){ vkDefaultCases; }
			switch(dvc::graphics.ld.createSemaphore(&semaphoreInfo, nullptr, &frames[i].s_copy    )){ vkDefaultCases; }
			switch(dvc::graphics.ld.createSemaphore(&semaphoreInfo, nullptr, &frames[i].s_clear   )){ vkDefaultCases; }
			switch(dvc::graphics.ld.createFence    (&fenceInfo,     nullptr, &frames[i].f_rendered)){ vkDefaultCases; }
		}
	}








	/**
	 * @brief Initializes the swapchain
	 *     Creates the swapchain and the render pass objects, gets the swapchain images and creates an image views and a framebuffer for each of them
	 *     This function should only be used by the engine
	 * Complexity: O(n)
	 *     where n = number of swapchain images
	 * @param vUseVSync //TODO
	 */
	void Swapchain::create(bool vUseVSync) {
		useVSync = vUseVSync;

		//Choose max image count. Minimum or minimum +1 if supported
		auto capabilities = getCapabilities();
		uint32 minImageCount = capabilities.minImageCount + 1;
		if(capabilities.maxImageCount > 0 && minImageCount > capabilities.maxImageCount) {
			minImageCount = capabilities.maxImageCount;
		}

		//swapchain creation infos
		vk::SurfaceFormatKHR surfaceFormat{ chooseSurfaceFormat(getSurfaceFormats()) };
		uint32 queueFamilyIndices[] = { dvc::graphics.pd.indices.graphicsFamily, dvc::graphics.pd.indices.presentFamily };
		createInfo = vk::SwapchainCreateInfoKHR()
			.setSurface               (w->surface)
			.setMinImageCount         (minImageCount)
			.setImageFormat           (surfaceFormat.format)
			.setImageColorSpace       (surfaceFormat.colorSpace)
			.setImageExtent           (chooseSwapchainExtent(&capabilities))
			.setImageArrayLayers      (1)
			.setImageUsage            (vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst)

			.setImageSharingMode      ((dvc::graphics.pd.indices.graphicsFamily != dvc::graphics.pd.indices.presentFamily) ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive)
			.setQueueFamilyIndexCount (2)
			.setPQueueFamilyIndices   (queueFamilyIndices)

			.setPreTransform          (capabilities.currentTransform)
			.setCompositeAlpha        (vk::CompositeAlphaFlagBitsKHR::eOpaque)
			.setPresentMode           (choosePresentMode(getPresentModes()))
			.setClipped               (VK_TRUE)
			.setOldSwapchain          (nullptr)
		;


		//Create swapchain
		vk::Bool32 hasPresentSupport = false;
		switch(dvc::graphics.pd.device.getSurfaceSupportKHR(dvc::graphics.pd.indices.presentFamily, w->surface, &hasPresentSupport)){ //! SUPPRESS ERROR
			case vk::Result::eErrorSurfaceLostKHR:    dbg::printError("Surface lost");         break;
			vkDefaultCases;
		}

		//FIXME hasPresentSupport is unused
		//FIXME ^ SKIP THIS IN RELEASE MODE (It's probably already checked by another function)
		switch(dvc::graphics.ld.createSwapchainKHR(&createInfo, nullptr, &swapchain)){
			case vk::Result::eErrorInitializationFailed: dbg::printError("Initialization failed"); break;
			case vk::Result::eErrorNativeWindowInUseKHR: dbg::printError("Native window in use");  break;
			case vk::Result::eErrorSurfaceLostKHR:       dbg::printError("Surface lost");          break;
			case vk::Result::eErrorDeviceLost:           dbg::printError("Device lost");           break;
			vkDefaultCases;
		}



		//Create render pass
		createRenderPass();

		//Get images
		uint32 imageCount;
		switch(dvc::graphics.ld.getSwapchainImagesKHR(swapchain, &imageCount, nullptr)){
			case vk::Result::eIncomplete: dbg::printError("Incomplete swapchains"); break;
			vkDefaultCases;
		}

		images.resize(imageCount); vk::Image _[imageCount];
		switch(dvc::graphics.ld.getSwapchainImagesKHR(swapchain, &imageCount, _)){
			case vk::Result::eIncomplete: dbg::printError("Incomplete swapchains"); break;
			vkDefaultCases;
		}


		//Initialize images, image views and framebuffers
		for(uint32 i = 0; i < imageCount; ++i) {
			images[i].image = _[i];
			images[i].view  = createImageView(images[i].image, createInfo.imageFormat, vk::ImageAspectFlagBits::eColor);
			images[i].fbuffer = createFramebuffer(renderPass, images[i].view, createInfo.imageExtent.width, createInfo.imageExtent.height);
			//FIXME check if framebuffers are really necessary
		}
	}








	/**
	 * @brief Destroyes and recreates the swapchain
	 *     This function should only be used by the engine
	 * Complexity: O(n)
	 *     where n = number of swapchain images
	 */
	void Swapchain::recreate() {
		int32 width, height;	glfwGetFramebufferSize(w->window, &width, &height);
		if(width != 0 && height != 0) {			//If the window contains pixels
			destroy();								//Clean the old swapchain
			switch(dvc::graphics.ld.waitIdle()){	//Wait for the logical device
				case vk::Result::eErrorDeviceLost: dbg::printError("Device lost"); break;
				vkDefaultCases;
			}


			{ //swapchain creation infos
				//Recalculate swapchain extent
				vk::SurfaceCapabilitiesKHR capabilities;
				switch(dvc::graphics.pd.device.getSurfaceCapabilitiesKHR(w->surface, &capabilities)){
					case vk::Result::eErrorSurfaceLostKHR: dbg::printError("Surface lost"); break;
					vkDefaultCases;
				}

				createInfo.imageExtent = chooseSwapchainExtent(&capabilities);

				//Recalculate queue family indices
				uint32 queueFamilyIndices[] = { dvc::graphics.pd.indices.graphicsFamily, dvc::graphics.pd.indices.presentFamily };
				createInfo.pQueueFamilyIndices = queueFamilyIndices;

				//Set oldSwapchain member to allow the driver to reuse resources
				vk::SwapchainKHR oldSwapchain = swapchain;
				createInfo.oldSwapchain  = oldSwapchain;

				//Create swapchain
				switch(dvc::graphics.ld.createSwapchainKHR(&createInfo, nullptr, &swapchain)){
					case vk::Result::eErrorInitializationFailed: dbg::printError("Initialization failed"); break;
					case vk::Result::eErrorNativeWindowInUseKHR: dbg::printError("Native window in use");  break;
					case vk::Result::eErrorSurfaceLostKHR:       dbg::printError("Surface lost");          break;
					case vk::Result::eErrorDeviceLost:           dbg::printError("Device lost");           break;
					vkDefaultCases;
				}

				dvc::graphics.ld.destroySwapchainKHR(oldSwapchain, nullptr);
			}

			//Create images
			uint32 imageCount;
				switch(dvc::graphics.ld.getSwapchainImagesKHR(swapchain, &imageCount, nullptr)){
				case vk::Result::eIncomplete: dbg::printError("Incomplete devices"); break;
				vkDefaultCases;
			}

			//TODO ^ Vulkan validation layers complain about not calling this function with nullptr before getting the images,
			//TODO   but the number of images remains the same, so there is not need to call it twice.
			//TODO   If it's the case, remove those lines from release mode.

			//Get images
			vk::Image _[imageCount];
				switch(dvc::graphics.ld.getSwapchainImagesKHR(swapchain, &imageCount, _)){
				case vk::Result::eIncomplete: dbg::printError("Incomplete devices"); break;
				vkDefaultCases;
			}


			//Re initialize images, image views and framebuffer
			for(uint32 i = 0; i < imageCount; ++i) {
				images[i].image = _[i];
				images[i].view  = createImageView(images[i].image, createInfo.imageFormat, vk::ImageAspectFlagBits::eColor);
				images[i].fbuffer = createFramebuffer(renderPass, images[i].view, createInfo.imageExtent.width, createInfo.imageExtent.height);
			}


			//Update the window size buffer
			u32v2 wSize = { createInfo.imageExtent.width, createInfo.imageExtent.height };
			vk::CommandBuffer cb = core::render::cmd::beginSingleTimeCommands();
			cb.updateBuffer(w->renderCore.wSize_g.cell->csc.buffer, w->renderCore.wSize_g.cell->localOffset, w->renderCore.wSize_g.cell->cellSize, &wSize);
			core::render::cmd::endSingleTimeCommands(cb);
			//FIXME AUTOMATIZE BUFFER UPDATE
			//FIXME UPDATE ALL BUFFERS TOGETHER AFTER A FRAME IS RENDERED


			{	//Destroy copy command buffers
				dvc::graphics.ld.freeCommandBuffers(w->renderCore.copyCommandPool, w->renderCore.copyCommandBuffers.count(), w->renderCore.copyCommandBuffers.begin());
				dvc::graphics.ld.destroyCommandPool(w->renderCore.copyCommandPool, nullptr);

				//#LLID CCB0000 Recreate copy command buffers
				w->renderCore.copyCommandBuffers.resize(images.count());	//Resize the command buffer array in the shader
				w->renderCore.createDefaultCommandBuffers__();				//Create command buffers and command pool
			}

			//Recreate clear shader
			w->renderCore.sh_clear.updateCommandBuffers({ (createInfo.imageExtent.width * createInfo.imageExtent.height) / (32 * 32) + 1, 1u, 1u }, *w);
		}
	}








	/**
	 * @brief Destroys the swapchain
	 *     This function should only be used by the engine
	 * Complexity: O(n)
	 *     where n = this->images.count()
	 */
	void Swapchain::destroy() {
		for(auto img : images) {
			dvc::graphics.ld.destroyFramebuffer(img.fbuffer, nullptr);
			dvc::graphics.ld.destroyImageView  (img.view,    nullptr);
		}
	}




	/**
	 * @brief Destroys the swapchain object and frees its resources
	 *     This function should only be used by the engine
	 * Complexity: O(n + m)
	 *     where n = this->images.count() and m = __renderMaxFramesInFlight
	 */
	void Swapchain::clear(){
		switch(core::dvc::graphics.ld.waitIdle()){
			case vk::Result::eErrorDeviceLost: dbg::printError("Device lost"); break;
			vkDefaultCases;
		}
		destroy();
		dvc::graphics.ld.destroyRenderPass  (renderPass, nullptr);
		dvc::graphics.ld.destroySwapchainKHR(swapchain,  nullptr);
		for(uint32 i = 0; i < __renderMaxFramesInFlight; ++i) {
			dvc::graphics.ld.destroySemaphore(frames[i].s_aquired,  nullptr);
			dvc::graphics.ld.destroySemaphore(frames[i].s_objects,  nullptr);
			dvc::graphics.ld.destroySemaphore(frames[i].s_copy,     nullptr);
			dvc::graphics.ld.destroySemaphore(frames[i].s_clear,    nullptr);
			dvc::graphics.ld.destroyFence    (frames[i].f_rendered, nullptr);
		}
	}








	/**
	 * @brief Creates an image view from a vk::Image object
	 *     This function should only be used by the engine
	 * Complexity: O(1)
	 * @param vImage The image to create the view from
	 * @param vFormat The format of the image
	 * @param vAspectFlags //TODO
	 * @return The created image view
	 */
	inline vk::ImageView Swapchain::createImageView(const vk::Image vImage, const vk::Format vFormat, const vk::ImageAspectFlags vAspectFlags) {
		auto viewInfo = vk::ImageViewCreateInfo()
			.setImage    (vImage)
			.setViewType (vk::ImageViewType::e2D)
			.setFormat   (vFormat)
			.setComponents(vk::ComponentMapping()
				.setR (vk::ComponentSwizzle::eIdentity)
				.setG (vk::ComponentSwizzle::eIdentity)
				.setB (vk::ComponentSwizzle::eIdentity)
				.setA (vk::ComponentSwizzle::eIdentity)
			)
			.setSubresourceRange(vk::ImageSubresourceRange()
				.setAspectMask     (vAspectFlags)
				.setBaseMipLevel   (0)
				.setLevelCount     (1)
				.setBaseArrayLayer (0)
				.setLayerCount     (1)
			)
		;
		vk::ImageView imageView;
		switch(dvc::graphics.ld.createImageView(&viewInfo, nullptr, &imageView)){ vkDefaultCases; }

		return imageView;
	}








	/**
	 * @brief Creates a framebuffer
	 *     This function should only be used by the engine
	 * Complexity: O(1)
	 * @param vRenderPass The render pass to use in the framebuffer //TODO
	 * @param vAttachment //TODO
	 * @param vWidth Width of the framebuffer
	 * @param vHeight Height of the freamebuffer
	 * @return The created framebuffer
	 */
	inline vk::Framebuffer Swapchain::createFramebuffer(vk::RenderPass vRenderPass, vk::ImageView& vAttachment, uint32 vWidth, uint32 vHeight) {
		auto framebufferInfo = vk::FramebufferCreateInfo()
			.setRenderPass      (vRenderPass)
			.setAttachmentCount (1)
			.setPAttachments    (&vAttachment)
			.setWidth           (vWidth)
			.setHeight          (vHeight)
			.setLayers          (1)
		;
		vk::Framebuffer framebuffer;
		switch(dvc::graphics.ld.createFramebuffer(&framebufferInfo, nullptr, &framebuffer)){ vkDefaultCases; }

		return framebuffer;
	}








	// Helper functions -------------------------------------------------------------------------------------------------------------------------------//








	//TODO use best format available when not specified
	//TODO use RGBA8 format in shaders when better formats are not available
	/**
	 * @brief Chooses the best vk::SurfaceFormatKHR based on the available formats
	 *     This function should only be used by the engine
	 * Complexity:
	 *     Best:  O(1)
	 *     Worst: O(n)
	 *     where n = pAvailableFormats.count()
	 * @param pAvailableFormats An array containing the available formats
	 * @return The chosen format
	 */
	vk::SurfaceFormatKHR Swapchain::chooseSurfaceFormat(const RtArray<vk::SurfaceFormatKHR>& pAvailableFormats) {
		for(auto& fmt : pAvailableFormats) {
			if(
				dvc::graphics.pd.device.getFormatProperties(fmt.format).optimalTilingFeatures & vk::FormatFeatureFlagBits::eTransferDst &&
				/*fmt.format == vk::Format::eR8G8B8A8Srgb && */fmt.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear
			) {
				return fmt;
			}
		}
		dbg::printError("No supported surface format found");
		return pAvailableFormats[0]; //FIXME
	}
	//FIXME add tiling to parameters ^^
	// vk::Format findSupportedFormat(const RtArray<vk::Format>* pCandidates, const vk::ImageTiling vTiling, const vk::FormatFeatureFlags vFeatures) {
	// 	for(vk::Format format : *pCandidates) {
	// 		auto props = dvc::graphics.pd.device.getFormatProperties(format); //Get format properties

	// 		if(( vTiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & vFeatures) == vFeatures) ||
	// 			(vTiling == vk::ImageTiling::eLinear  && (props.linearTilingFeatures  & vFeatures) == vFeatures)) {
	// 			return format;
	// 		}
	// 	}
	// 	dbg::printError("Failed to find a supported format");
	// 	return vk::Format::eUndefined;
	// }




	//TODO Use immediate or mailbox (causes tearing), FIFO if using VSync
	/**
	 * @brief Chooses the best vk::PresentModeKHR based on the available present modes
	 *     This function should only be used by the engine
	 * Complexity:
	 *     Best:  O(1)
	 *     Worst: O(n)
	 *     where n = pAvailablePresentModes.count()
	 * @param pAvailablePresentModes An array containing the available present modes
	 * @return The cosen present mode
	 */
	vk::PresentModeKHR Swapchain::choosePresentMode(const RtArray<vk::PresentModeKHR>& pAvailablePresentModes) {
		if(useVSync) return vk::PresentModeKHR::eFifo; //FIXME MOVE VSYNC TO WINDOW STRUCT
		for(const auto& availablePresentMode : pAvailablePresentModes) {
			if(availablePresentMode == vk::PresentModeKHR::eMailbox) return availablePresentMode;
		}
		return vk::PresentModeKHR::eImmediate;
	}




	/**
	 * @brief Chooses the best swapchain extent based on the surface capabilities
	 *     This function should only be used by the engine
	 * Complexity: O(1)
	 * @param pCapabilities The Vulkan object containing the surface capabilities
	 * @return The chosen extent
	 */
	vk::Extent2D Swapchain::chooseSwapchainExtent(const vk::SurfaceCapabilitiesKHR* pCapabilities) {
		int32 width = 0, height = 0;
		glfwGetFramebufferSize(w->window, &width, &height);
		return vk::Extent2D{
			max(pCapabilities->minImageExtent.width,  min(pCapabilities->maxImageExtent.width , (uint32)width)),
			max(pCapabilities->minImageExtent.height, min(pCapabilities->maxImageExtent.height, (uint32)height))
		};
	}








	// Render pass creation ---------------------------------------------------------------------------------------------------------------------------//








	/**
	 * @brief Creates the render pass of the swapchain
	 *     This function should only be used by the engine
	 * Complexity: O(1)
	 */
	void Swapchain::createRenderPass() {
		auto colorAttachment = vk::AttachmentDescription()				//Create attachment
			.setFormat         (createInfo.imageFormat)						//Swapchain image format
			.setSamples        (vk::SampleCountFlagBits::e1)				//Multisampling samples
			.setLoadOp         (vk::AttachmentLoadOp::eDontCare)			//Don't clear for better performance
			.setStoreOp        (vk::AttachmentStoreOp::eDontCare)			//Don't save rendered image
			.setStencilLoadOp  (vk::AttachmentLoadOp::eDontCare)			//Discard stencil
			.setStencilStoreOp (vk::AttachmentStoreOp::eDontCare)			//Discard stencil
			.setInitialLayout  (vk::ImageLayout::eUndefined)				//Default layout
			.setFinalLayout    (vk::ImageLayout::ePresentSrcKHR)			//Present layout
		;


		auto colorAttachmentRef = vk::AttachmentReference()				//create attachment reference
			.setAttachment(0)												//Use first attachment
			.setLayout(vk::ImageLayout::eColorAttachmentOptimal)			//Optimized layout
		;
		auto subpass = vk::SubpassDescription()							//Create subpass description
			.setPipelineBindPoint       (vk::PipelineBindPoint::eGraphics)	//Set structure type
			.setColorAttachmentCount    (1)									//Set number of attachments
			.setPColorAttachments       (&colorAttachmentRef)				//Previously created color attachment
			.setPDepthStencilAttachment (nullptr)							//Previously created depth attachment
		;


		vk::SubpassDependency dependencies[2]{							//Dependencies for implicit conversion
			vk::SubpassDependency()											//From undefined to color
				.setSrcSubpass      (VK_SUBPASS_EXTERNAL)
				.setDstSubpass      (0)
				.setSrcStageMask    (vk::PipelineStageFlagBits::eBottomOfPipe)
				.setDstStageMask    (vk::PipelineStageFlagBits::eColorAttachmentOutput)
				.setSrcAccessMask   (vk::AccessFlagBits::eMemoryRead)
				.setDstAccessMask   (vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite)
			,
			vk::SubpassDependency() 										//From color to undefined
				.setSrcSubpass      (0)
				.setDstSubpass      (VK_SUBPASS_EXTERNAL)
				.setSrcStageMask    (vk::PipelineStageFlagBits::eColorAttachmentOutput)
				.setDstStageMask    (vk::PipelineStageFlagBits::eBottomOfPipe)
				.setSrcAccessMask   (vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite)
				.setDstAccessMask   (vk::AccessFlagBits::eMemoryRead)
			,
		};



		auto renderPassInfo = vk::RenderPassCreateInfo() 				//Create render pass infos
			.setAttachmentCount (1)											//Set number of attachments
			.setPAttachments    (&colorAttachment)							//Set attachments
			.setSubpassCount    (1)											//Set number of subpasses
			.setPSubpasses      (&subpass)									//Set subpass
			.setDependencyCount (2)											//Set number of dependencies
			.setPDependencies   (dependencies)								//Set dependencies
		;

		//Create render pass. Exit if an error occurs
		switch(dvc::graphics.ld.createRenderPass(&renderPassInfo, nullptr, &renderPass)){ vkDefaultCases; }
	}




	/**
	 * @brief Returns the surface capabilities of the owner window
	 *     This function should only be used by the engine
	 * Complexity: O(1)
	 * @return The surface capabilities
	 */
	vk::SurfaceCapabilitiesKHR Swapchain::getCapabilities(){
		vk::SurfaceCapabilitiesKHR capabilities;
		switch(core::dvc::graphics.pd.device.getSurfaceCapabilitiesKHR(w->surface, &capabilities)){
			case vk::Result::eErrorSurfaceLostKHR: dbg::printError("Surface lost"); break;
			vkDefaultCases;
		}

		return capabilities;
	}




	/**
	 * @brief Returns the surface formats of the owner window
	 *     This function should only be used by the engine
	 * Complexity: O(1)
	 * @return An array containing the surface formats
	 */
	RtArray<vk::SurfaceFormatKHR> Swapchain::getSurfaceFormats(){
		uint32 count;
		RtArray<vk::SurfaceFormatKHR>	formats;

		switch(core::dvc::graphics.pd.device.getSurfaceFormatsKHR(w->surface, &count, nullptr)){
			case vk::Result::eIncomplete:          dbg::printError("Incomplete formats"); break;
			case vk::Result::eErrorSurfaceLostKHR: dbg::printError("Surface lost");       break;
			vkDefaultCases;
		}

		formats.resize(count);
		switch(core::dvc::graphics.pd.device.getSurfaceFormatsKHR(w->surface, &count, formats.begin())){
			case vk::Result::eIncomplete:          dbg::printError("Incomplete formats");   break;
			case vk::Result::eErrorSurfaceLostKHR: dbg::printError("Surface lost");         break;
			vkDefaultCases;
		}

		return formats;
	}




	/**
	 * @brief Returns the surface presents modes of the owner window
	 *     This function should only be used by the engine
	 * Complexity: O(1)
	 * @return An array containing the present modes
	 */
	RtArray<vk::PresentModeKHR> Swapchain::getPresentModes(){
		uint32 count; RtArray<vk::PresentModeKHR> presentModes;

		switch(dvc::graphics.pd.device.getSurfacePresentModesKHR(w->surface, &count, nullptr)){
			case vk::Result::eIncomplete:             dbg::printError("Incomplete formats");   break;
			case vk::Result::eErrorSurfaceLostKHR:    dbg::printError("Surface lost");         break;
			vkDefaultCases;
		}

		presentModes.resize(count);
		switch(dvc::graphics.pd.device.getSurfacePresentModesKHR(w->surface, &count, presentModes.begin())){
			case vk::Result::eIncomplete:             dbg::printError("Incomplete formats");   break;
			case vk::Result::eErrorSurfaceLostKHR:    dbg::printError("Surface lost");         break;
			vkDefaultCases;
		}

		return presentModes;
	}
}