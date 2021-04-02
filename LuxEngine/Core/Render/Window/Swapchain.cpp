﻿#include "LuxEngine/Core/Render/Window/Window.hpp"
#include "LuxEngine/Core/Render/Window/Swapchain.hpp"
#include "LuxEngine/Core/Render/Shaders/Shader.hpp"
#include "LuxEngine/Core/Render/GCommands.hpp"
#include "LuxEngine/Core/Devices.hpp"
#include "LuxEngine/Core/Core.hpp"
#define w (*(Window*)(((char*)this) - offsetof(lux::Window, lux::Window::swp))) //TODO







namespace lux::core::wnd{
	Swapchain::Swapchain(){
		frames.resize(__renderMaxFramesInFlight);

		//Create sync objects
		auto semaphoreInfo = vk::SemaphoreCreateInfo();
		auto fenceInfo = vk::FenceCreateInfo().setFlags(vk::FenceCreateFlagBits::eSignaled);
		for(uint32 i = 0; i < __renderMaxFramesInFlight; ++i) {
			dvc::graphics.LD.createSemaphore(&semaphoreInfo, nullptr, &frames[i].s_aquired );
			dvc::graphics.LD.createSemaphore(&semaphoreInfo, nullptr, &frames[i].s_objects );
			dvc::graphics.LD.createSemaphore(&semaphoreInfo, nullptr, &frames[i].s_copy    );
			dvc::graphics.LD.createSemaphore(&semaphoreInfo, nullptr, &frames[i].s_clear   );
			dvc::graphics.LD.createFence    (&fenceInfo,     nullptr, &frames[i].f_rendered);
		}
	}








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
		uint32 queueFamilyIndices[] = { dvc::graphics.PD.indices.graphicsFamily, dvc::graphics.PD.indices.presentFamily };
		createInfo = vk::SwapchainCreateInfoKHR()
			.setSurface               (bindedWindow->surface)
			.setMinImageCount         (minImageCount)
			.setImageFormat           (surfaceFormat.format)
			.setImageColorSpace       (surfaceFormat.colorSpace)
			.setImageExtent           (chooseSwapchainExtent(&capabilities))
			.setImageArrayLayers      (1)
			.setImageUsage            (vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst)

			.setImageSharingMode      ((dvc::graphics.PD.indices.graphicsFamily != dvc::graphics.PD.indices.presentFamily) ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive)
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
		dvc::graphics.PD.device.getSurfaceSupportKHR(dvc::graphics.PD.indices.presentFamily, bindedWindow->surface, &hasPresentSupport); //! SUPPRESS ERROR
		//FIXME hasPresentSupport is unused
		//FIXME ^ SKIP THIS IN RELEASE MODE (It's probably already checked by another function)
		dvc::graphics.LD.createSwapchainKHR(&createInfo, nullptr, &swapchain);


		//Create render pass
		createRenderPass();

		//Get images
		uint32 imageCount;
		dvc::graphics.LD.getSwapchainImagesKHR(swapchain, &imageCount, nullptr);
		images.resize(imageCount); vk::Image _[imageCount];
		dvc::graphics.LD.getSwapchainImagesKHR(swapchain, &imageCount, _);

		//Initialize images, image views and framebuffers
		for(uint32 i = 0; i < imageCount; ++i) {
			images[i].image = _[i];
			images[i].view  = createImageView(images[i].image, createInfo.imageFormat, vk::ImageAspectFlagBits::eColor);
			images[i].fbuffer = createFramebuffer(renderPass, images[i].view, createInfo.imageExtent.width, createInfo.imageExtent.height);
			//FIXME check if framebuffers are really necessary
		}
	}








	void Swapchain::recreate() {
		int32 width, height;	glfwGetFramebufferSize(bindedWindow->window, &width, &height);
		if(width != 0 && height != 0) {			//If the window contains pixels
			destroy();								//Clean the old swapchain
			dvc::graphics.LD.waitIdle();			//Wait for the logical device

			{ //swapchain creation infos
				//Recalculate swapchain extent
				vk::SurfaceCapabilitiesKHR capabilities;
				dvc::graphics.PD.device.getSurfaceCapabilitiesKHR(bindedWindow->surface, &capabilities);
				createInfo.imageExtent = chooseSwapchainExtent(&capabilities);

				//Recalculate queue family indices
				uint32 queueFamilyIndices[] = { dvc::graphics.PD.indices.graphicsFamily, dvc::graphics.PD.indices.presentFamily };
				createInfo.pQueueFamilyIndices = queueFamilyIndices;

				//Set oldSwapchain member to allow the driver to reuse resources
				vk::SwapchainKHR oldSwapchain = swapchain;
				createInfo.oldSwapchain  = oldSwapchain;

				//Create swapchain
				dvc::graphics.LD.createSwapchainKHR(&createInfo, nullptr, &swapchain);
				dvc::graphics.LD.destroySwapchainKHR(oldSwapchain, nullptr);
			}

			//Create images
			uint32 imageCount;
			dvc::graphics.LD.getSwapchainImagesKHR(swapchain, &imageCount, nullptr);
			//TODO ^ Vulkan validation layers complain about not calling this function with nullptr before getting the images,
			//TODO   but the number of images remains the same, so there is not need to call it twice.
			//TODO   If it's the case, remove those lines from release mode.

			//Get images
			vk::Image _[imageCount];
			dvc::graphics.LD.getSwapchainImagesKHR(swapchain, &imageCount, _);

			//Re initialize images, image views and framebuffer
			for(uint32 i = 0; i < imageCount; ++i) {
				images[i].image = _[i];
				images[i].view  = createImageView(images[i].image, createInfo.imageFormat, vk::ImageAspectFlagBits::eColor);
				images[i].fbuffer = createFramebuffer(renderPass, images[i].view, createInfo.imageExtent.width, createInfo.imageExtent.height);
			}


			//Update the window size buffer
			u32 wSize[2] = { createInfo.imageExtent.width, createInfo.imageExtent.height };
			vk::CommandBuffer cb = core::render::cmd::beginSingleTimeCommands();
			cb.updateBuffer(bindedWindow->wSize_g.cell->csc.buffer, bindedWindow->wSize_g.cell->localOffset, bindedWindow->wSize_g.cell->cellSize, wSize);
			core::render::cmd::endSingleTimeCommands(cb);
			//FIXME AUTOMATIZE BUFFER UPDATE
			//FIXME UPDATE ALL BUFFERS TOGETHER AFTER A FRAME IS RENDERED


			{	//Destroy copy command buffers
				dvc::compute.LD.freeCommandBuffers(bindedWindow->copyCommandPool, bindedWindow->copyCommandBuffers.count(), bindedWindow->copyCommandBuffers.begin());
				dvc::compute.LD.destroyCommandPool(bindedWindow->copyCommandPool, nullptr);

				//#LLID CCB0000 Recreate copy command buffers
				bindedWindow->copyCommandBuffers.resize(images.count());	//Resize the command buffer array in the shader
				bindedWindow->createDefaultCommandBuffers__();				//Create command buffers and command pool
			}

			//Recreate clear shader
			bindedWindow->sh_clear.updateCommandBuffers(LUX_DEF_SHADER_CLEAR, (createInfo.imageExtent.width * createInfo.imageExtent.height) / (32 * 32) + 1, 1, 1, *bindedWindow);
		}
	}








	void Swapchain::destroy() {
		for(auto img : images) {
			dvc::graphics.LD.destroyFramebuffer(img.fbuffer, nullptr);
			dvc::graphics.LD.destroyImageView  (img.view,    nullptr);
		}
	}




	Swapchain::~Swapchain(){
		core::dvc::graphics.LD.waitIdle();
		destroy();
		dvc::graphics.LD.destroyRenderPass  (renderPass, nullptr);
		dvc::graphics.LD.destroySwapchainKHR(swapchain,  nullptr);
		for(uint32 i = 0; i < __renderMaxFramesInFlight; ++i) {
			dvc::graphics.LD.destroySemaphore(frames[i].s_aquired,  nullptr);
			dvc::graphics.LD.destroySemaphore(frames[i].s_objects,  nullptr);
			dvc::graphics.LD.destroySemaphore(frames[i].s_copy,     nullptr);
			dvc::graphics.LD.destroySemaphore(frames[i].s_clear,    nullptr);
			dvc::graphics.LD.destroyFence    (frames[i].f_rendered, nullptr);
		}
	}








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
		dvc::graphics.LD.createImageView(&viewInfo, nullptr, &imageView);
		return imageView;
	}








	inline vk::Framebuffer Swapchain::createFramebuffer(vk::RenderPass vRenderPass, vk::ImageView& vAttachment, uint32 vWith, uint32 vHeight) {
		auto framebufferInfo = vk::FramebufferCreateInfo()
			.setRenderPass      (vRenderPass)
			.setAttachmentCount (1)
			.setPAttachments    (&vAttachment)
			.setWidth           (vWith)
			.setHeight          (vHeight)
			.setLayers          (1)
		;
		vk::Framebuffer framebuffer;
		dvc::graphics.LD.createFramebuffer(&framebufferInfo, nullptr, &framebuffer);
		return framebuffer;
	}








	//-------------------------------------------------------------------------------------------------------------------------------------------------//








	vk::SurfaceFormatKHR Swapchain::chooseSurfaceFormat(const RtArray<vk::SurfaceFormatKHR>& pAvailableFormats) {
		for(auto& availableFormat : pAvailableFormats) {
			//TODO use best format available when not specified
			//TODO use RGBA8 format in shaders when better formats are not available
			if(availableFormat.format == vk::Format::eR8G8B8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
				return availableFormat;
			}
		}
		return pAvailableFormats[0];
	}




	//Returns the presentation mode that will be used. Use immediate or mailbox (causes tearing), FIFO if using VSync
	vk::PresentModeKHR Swapchain::choosePresentMode(const RtArray<vk::PresentModeKHR>& pAvailablePresentModes) {
		if(useVSync) return vk::PresentModeKHR::eFifo; //FIXME MOVE VSYNC TO WINDOW STRUCT
		for(const auto& availablePresentMode : pAvailablePresentModes) {
			if(availablePresentMode == vk::PresentModeKHR::eMailbox) return availablePresentMode;
		}
		return vk::PresentModeKHR::eImmediate;
	}




	vk::Extent2D Swapchain::chooseSwapchainExtent(const vk::SurfaceCapabilitiesKHR* pCapabilities) {
		int32 width = 0, height = 0;
		glfwGetFramebufferSize(bindedWindow->window, &width, &height);
		return vk::Extent2D{
			max(pCapabilities->minImageExtent.width,  min(pCapabilities->maxImageExtent.width , (uint32)width)),
			max(pCapabilities->minImageExtent.height, min(pCapabilities->maxImageExtent.height, (uint32)height))
		};
	}








	//-------------------------------------------------------------------------------------------------------------------------------------------------//







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


		vk::SubpassDependency dependencies[2]{							//Dependencies for implicit convertion
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
		dvc::graphics.LD.createRenderPass(&renderPassInfo, nullptr, &renderPass);
	}




	vk::SurfaceCapabilitiesKHR Swapchain::getCapabilities(){
		vk::SurfaceCapabilitiesKHR capabilities;
		core::dvc::graphics.PD.device.getSurfaceCapabilitiesKHR(bindedWindow->surface, &capabilities);
		return capabilities;
	}


	RtArray<vk::SurfaceFormatKHR> Swapchain::getSurfaceFormats(){
		uint32 count;
		RtArray<vk::SurfaceFormatKHR>	formats;
		core::dvc::graphics.PD.device.getSurfaceFormatsKHR(bindedWindow->surface, &count, nullptr);
		if(count != 0) {
			formats.resize(count);
			core::dvc::graphics.PD.device.getSurfaceFormatsKHR(bindedWindow->surface, &count, formats.begin());
		}
		return formats;
	}


	RtArray<vk::PresentModeKHR> Swapchain::getPresentModes(){
		uint32 count; RtArray<vk::PresentModeKHR> presentModes;
		dvc::graphics.PD.device.getSurfacePresentModesKHR(bindedWindow->surface, &count, nullptr);
		if(count != 0) {
			presentModes.resize(count);
			dvc::graphics.PD.device.getSurfacePresentModesKHR(bindedWindow->surface, &count, presentModes.begin());
		}
		return presentModes;
	}
}