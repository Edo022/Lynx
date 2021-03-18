#include "LuxEngine/Core/Render/Window/Window.hpp"
#include "LuxEngine/Core/Render/Window/Swapchain.hpp"
#include "LuxEngine/Core/Render/Shaders/Shader.hpp"
#include "LuxEngine/Core/Devices.hpp"
#include "LuxEngine/Core/Core.hpp"
#define w (*(Window*)(((char*)this) - offsetof(lux::Window, lux::Window::swp)))







namespace lux::core::wnd{
	Swapchain::Swapchain(){
		frames.resize(__renderMaxFramesInFlight);

		//Create sync objects
		VkSemaphoreCreateInfo semaphoreInfo{ .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, /*.flags = VK_SEMAPHORE_*/ };
		VkFenceCreateInfo fenceInfo{ .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, .flags = VK_FENCE_CREATE_SIGNALED_BIT };
		for(uint32 i = 0; i < __renderMaxFramesInFlight; ++i) {
			vkCreateSemaphore(dvc::graphics.LD, &semaphoreInfo, nullptr, &frames[i].s_aquired);
			vkCreateSemaphore(dvc::graphics.LD, &semaphoreInfo, nullptr, &frames[i].s_objects);
			vkCreateSemaphore(dvc::graphics.LD, &semaphoreInfo, nullptr, &frames[i].s_copy);
			vkCreateSemaphore(dvc::graphics.LD, &semaphoreInfo, nullptr, &frames[i].s_clear);
			vkCreateFence(    dvc::graphics.LD, &fenceInfo, 	nullptr, &frames[i].f_rendered);
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
		VkSurfaceFormatKHR surfaceFormat{ chooseSurfaceFormat(getSurfaceFormats()) };
		uint32 queueFamilyIndices[] = { dvc::graphics.PD.indices.graphicsFamily, dvc::graphics.PD.indices.presentFamily };
		createInfo = {
			.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.surface          = w.surface,
			.minImageCount    = minImageCount,
			.imageFormat      = surfaceFormat.format,
			.imageColorSpace  = surfaceFormat.colorSpace,
			.imageExtent      = chooseSwapchainExtent(&capabilities),
			.imageArrayLayers = 1,
			.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,

			.imageSharingMode      = (dvc::graphics.PD.indices.graphicsFamily != dvc::graphics.PD.indices.presentFamily) ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 2,
			.pQueueFamilyIndices   = queueFamilyIndices,

			.preTransform   = capabilities.currentTransform,
			.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			.presentMode    = choosePresentMode(getPresentModes()),
			.clipped        = VK_TRUE,
			.oldSwapchain   = VK_NULL_HANDLE,
		};


		//Create swapchain
		VkBool32 hasPresentSupport = false; //FIXME
		vkGetPhysicalDeviceSurfaceSupportKHR(dvc::graphics.PD.device, dvc::graphics.PD.indices.presentFamily, bindedWindow->surface, &hasPresentSupport); //SUPPRESS ERROR //FIXME
		dbg::checkVk(vkCreateSwapchainKHR(dvc::graphics.LD, &createInfo, nullptr, &swapchain), "Failed to create swapchain");


		createRenderPass();


		//Create images
		uint32 imageCount;
		vkGetSwapchainImagesKHR(dvc::graphics.LD, swapchain, &imageCount, nullptr);			//Get image count
		// images.resize(imageCount);
		images.resize(imageCount);
		VkImage __[imageCount];
		vkGetSwapchainImagesKHR(dvc::graphics.LD, swapchain, &imageCount, __);	//Save images
		for(uint32 i = 0; i < imageCount; ++i) {
			images[i].image = __[i];
			images[i].view  = createImageView(images[i].image, createInfo.imageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
			VkFramebufferCreateInfo framebufferInfo{
					.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
					.renderPass      = renderPass,
					.attachmentCount = 1,
					.pAttachments    = &images[i].view,
					.width           = createInfo.imageExtent.width,
					.height          = createInfo.imageExtent.height,
					.layers          = 1
				};
				dbg::checkVk(vkCreateFramebuffer(dvc::graphics.LD, &framebufferInfo, nullptr, &images[i].fbuffer), "Failed to create framebuffer");
		}

		//Create image views
		// imageViews.resize(images.count());
		// for(uint32 i = 0; i < images.count(); ++i) images[i].imageViews = createImageView(images[i], createInfo.imageFormat, VK_IMAGE_ASPECT_COLOR_BIT);


		// createFramebuffers();
	}








	// void Swapchain::recreate(const bool vWindowResized) {
	void Swapchain::recreate() {
		int32 width, height;	glfwGetFramebufferSize(bindedWindow->window, &width, &height);
		if(width != 0 && height != 0) {			//If the window contains pixels
			destroy();								//Clean the old swapchain
			vkDeviceWaitIdle(dvc::graphics.LD);		//Wait for the logical device

			{ //swapchain creation infos
				VkSurfaceCapabilitiesKHR capabilities;
				vkGetPhysicalDeviceSurfaceCapabilitiesKHR(dvc::graphics.PD.device, bindedWindow->surface, &capabilities);
				createInfo.imageExtent = chooseSwapchainExtent(&capabilities);

				uint32 queueFamilyIndices[] = { dvc::graphics.PD.indices.graphicsFamily, dvc::graphics.PD.indices.presentFamily };
				createInfo.pQueueFamilyIndices = queueFamilyIndices;

				VkSwapchainKHR oldSwapchain = swapchain;
				createInfo.oldSwapchain  = oldSwapchain;

				//Create swapchain
				dbg::checkVk(vkCreateSwapchainKHR(dvc::graphics.LD, &createInfo, nullptr, &swapchain), "Failed to create swapchain");
				vkDestroySwapchainKHR(dvc::graphics.LD, oldSwapchain, nullptr);
			}

			//Create images
			uint32 imageCount;
			vkGetSwapchainImagesKHR(dvc::graphics.LD, swapchain, &imageCount, nullptr);			//Get image count
			// images.resize(imageCount);
			images.resize(imageCount);
			VkImage __[imageCount];
			vkGetSwapchainImagesKHR(dvc::graphics.LD, swapchain, &imageCount, __);	//Save images
			for(uint32 i = 0; i < imageCount; ++i) {
				images[i].image = __[i];
				images[i].view  = createImageView(images[i].image, createInfo.imageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
				VkFramebufferCreateInfo framebufferInfo{
					.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
					.renderPass      = renderPass,
					.attachmentCount = 1,
					.pAttachments    = &images[i].view,
					.width           = createInfo.imageExtent.width,
					.height          = createInfo.imageExtent.height,
					.layers          = 1
				};
				dbg::checkVk(vkCreateFramebuffer(dvc::graphics.LD, &framebufferInfo, nullptr, &images[i].fbuffer), "Failed to create framebuffer");
			}

			//Create image views
			// imageViews.resize(images.count());
			// for(uint32 i = 0; i < images.count(); ++i) images[i].imageViews = createImageView(images[i], createInfo.imageFormat, VK_IMAGE_ASPECT_COLOR_BIT);

			// createFramebuffers();


//FIXME check if framebuffer is necessary
			//Update the window size buffer
			bindedWindow->wSize_g.map();
			bindedWindow->wSize_g[0] = createInfo.imageExtent.width;
			bindedWindow->wSize_g[1] = createInfo.imageExtent.height;
			bindedWindow->wSize_g.unmap();

			{	//Destroy copy command buffers
				vkFreeCommandBuffers(dvc::compute.LD, bindedWindow->copyCommandPool, bindedWindow->copyCommandBuffers.count(), bindedWindow->copyCommandBuffers.begin());
				vkDestroyCommandPool(dvc::compute.LD, bindedWindow->copyCommandPool, nullptr);

				//#LLID CCB0000 Recreate copy command buffers
				// bindedWindow->copyCommandBuffers.resize(images.count());	//Resize the command buffer array in the shader
				bindedWindow->copyCommandBuffers.resize(images.count());	//Resize the command buffer array in the shader
				bindedWindow->createDefaultCommandBuffers__();				//Create command buffers and command pool
			}

			//Recreate clear shader
			c::shaders::updateShaderCall(bindedWindow->clearShader, LUX_DEF_SHADER_CLEAR, (createInfo.imageExtent.width * createInfo.imageExtent.height) / (32 * 32) + 1, 1, 1, *bindedWindow);
		}
	}








	void Swapchain::destroy() {
		// for(auto framebuffer : framebuffers) vkDestroyFramebuffer(dvc::graphics.LD, framebuffer, nullptr);		//Destroy framebuffers
		// for(auto imageView   : imageViews  ) vkDestroyImageView(  dvc::graphics.LD, imageView,   nullptr);		//Destroy image views
		for(auto img   : images  ) {
			vkDestroyFramebuffer(dvc::graphics.LD, img.fbuffer, nullptr);		//Destroy framebuffers
			vkDestroyImageView(  dvc::graphics.LD, img.view,    nullptr);		//Destroy image views
		}
	}




	Swapchain::~Swapchain(){
		vkDeviceWaitIdle(core::dvc::graphics.LD);
		vkDestroyRenderPass(dvc::graphics.LD, renderPass, nullptr);												//Destroy render pass
		destroy();
		vkDestroySwapchainKHR(dvc::graphics.LD, swapchain, nullptr);											//destroy swapchain
		for(uint32 i = 0; i < __renderMaxFramesInFlight; ++i) {
			vkDestroySemaphore(dvc::graphics.LD, frames[i].s_aquired,   nullptr);
			vkDestroySemaphore(dvc::graphics.LD, frames[i].s_objects, nullptr);
			vkDestroySemaphore(dvc::graphics.LD, frames[i].s_copy,            nullptr);
			vkDestroySemaphore(dvc::graphics.LD, frames[i].s_clear,           nullptr);
			vkDestroyFence(    dvc::graphics.LD, frames[i].f_rendered,   nullptr);
		}
	}








	VkImageView Swapchain::createImageView(const VkImage vImage, const VkFormat vFormat, const VkImageAspectFlags vAspectFlags) {
		VkImageViewCreateInfo viewInfo{
			.sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image    = vImage,
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format   = vFormat,
			.components{
				.r = VK_COMPONENT_SWIZZLE_IDENTITY,
				.g = VK_COMPONENT_SWIZZLE_IDENTITY,
				.b = VK_COMPONENT_SWIZZLE_IDENTITY,
				.a = VK_COMPONENT_SWIZZLE_IDENTITY
			},
			.subresourceRange{
				.aspectMask     = vAspectFlags,
				.baseMipLevel   = 0,
				.levelCount     = 1,
				.baseArrayLayer = 0,
				.layerCount     = 1
			}
		};
		VkImageView imageView;
		dbg::checkVk(vkCreateImageView(dvc::graphics.LD, &viewInfo, nullptr, &imageView), "Failed to create texture image view");
		return imageView;
	}








	//-------------------------------------------------------------------------------------------------------------------------------------------------//








	VkSurfaceFormatKHR Swapchain::chooseSurfaceFormat(const RtArray<VkSurfaceFormatKHR>& pAvailableFormats) {
		for(auto& availableFormat : pAvailableFormats) {
			//TODO use best format available when not specified
			//TODO use RGBA8 format in shaders when better formats are not available
			if(availableFormat.format == VK_FORMAT_R8G8B8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}
		return pAvailableFormats[0];
	}




	//Returns the presentation mode that will be used. Use immediate or mailbox (causes tearing), FIFO if using VSync
	VkPresentModeKHR Swapchain::choosePresentMode(const RtArray<VkPresentModeKHR>& pAvailablePresentModes) {
		if(useVSync) return VK_PRESENT_MODE_FIFO_KHR; //FIXME MOVE VSYNC TO WINDOW STRUCT
		for(const auto& availablePresentMode : pAvailablePresentModes) {
			if(availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) return availablePresentMode;
		}
		return VK_PRESENT_MODE_IMMEDIATE_KHR;
	}




	VkExtent2D Swapchain::chooseSwapchainExtent(const VkSurfaceCapabilitiesKHR* pCapabilities) {
		int32 width = 0, height = 0;
		glfwGetFramebufferSize(bindedWindow->window, &width, &height);
		return VkExtent2D{
			max(pCapabilities->minImageExtent.width,  min(pCapabilities->maxImageExtent.width , (uint32)width)),
			max(pCapabilities->minImageExtent.height, min(pCapabilities->maxImageExtent.height, (uint32)height))
		};
	}








	//-------------------------------------------------------------------------------------------------------------------------------------------------//








	// void Swapchain::createFramebuffers() {
	// 	// // framebuffers.resize(imageViews.count());
	// 	// framebuffers.resize(images.count());

	// 	// // for(uint32 i = 0; i < imageViews.count(); ++i) {
	// 	// for(uint32 i = 0; i < images.count(); ++i) {
	// 	// 	VkFramebufferCreateInfo framebufferInfo{
	// 	// 		.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
	// 	// 		.renderPass      = renderPass,
	// 	// 		.attachmentCount = 1,
	// 	// 		// .pAttachments    = &imageViews[i],
	// 	// 		.pAttachments    = &images[i].view,
	// 	// 		.width           = createInfo.imageExtent.width,
	// 	// 		.height          = createInfo.imageExtent.height,
	// 	// 		.layers          = 1
	// 	// 	};
	// 	// 	dbg::checkVk(vkCreateFramebuffer(dvc::graphics.LD, &framebufferInfo, nullptr, &framebuffers[i]), "Failed to create framebuffer");
	// 	// }
	// }






	void Swapchain::createRenderPass() {
		VkAttachmentDescription colorAttachment{
			.format         = createInfo.imageFormat,				//Swapchain image format
			.samples        = VK_SAMPLE_COUNT_1_BIT,				//Multisampling samples
			.loadOp         = VK_ATTACHMENT_LOAD_OP_DONT_CARE,		//Don't clear for better performance
			.storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE,		//Don't save rendered image
			.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,		//Discard stencil
			.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,		//Discard stencil
			.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,			//Default layout
			.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR		//Present layout
		};


		//create attachment reference
		VkAttachmentReference colorAttachmentRef{ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
		//Create subpass description
		VkSubpassDescription subpass{
			.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS,	//Set structure type
			.colorAttachmentCount    = 1,								//Set number of attachments
			.pColorAttachments       = &colorAttachmentRef,				//Previously created color attachment
			.pDepthStencilAttachment = VK_NULL_HANDLE					//Previously created depth attachment
		};


		//Dependencies for implicit convertion
		VkSubpassDependency dependencies[2]{
			{	//From undefined to color
				.srcSubpass      = VK_SUBPASS_EXTERNAL,
				.dstSubpass      = 0,
				.srcStageMask    = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
				.dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				.srcAccessMask   = VK_ACCESS_MEMORY_READ_BIT,
				.dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				.dependencyFlags = 0
			}, { //From color to undefined
				.srcSubpass      = 0,
				.dstSubpass      = VK_SUBPASS_EXTERNAL,
				.srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				.dstStageMask    = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
				.srcAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				.dstAccessMask   = VK_ACCESS_MEMORY_READ_BIT,
				.dependencyFlags = 0
			}
		};


		//Render pass
		VkRenderPassCreateInfo renderPassInfo{ 								//Create render pass infos
			.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,	//Set structure type
			.attachmentCount = 1,											//Set number of attachments
			.pAttachments    = &colorAttachment,							//Set attachments
			.subpassCount    = 1,											//Set number of subpasses
			.pSubpasses      = &subpass,									//Set subpass
			.dependencyCount = 2,											//Set number of dependencies
			.pDependencies   = dependencies									//Set dependencies
		};

		//Create render pass. Exit if an error occurs
		dbg::checkVk(vkCreateRenderPass(dvc::graphics.LD, &renderPassInfo, nullptr, &renderPass), "Failed to create render pass");
	}




	VkSurfaceCapabilitiesKHR Swapchain::getCapabilities(){
		VkSurfaceCapabilitiesKHR capabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(core::dvc::graphics.PD.device, bindedWindow->surface, &capabilities);
		return capabilities;
	}


	RtArray<VkSurfaceFormatKHR> Swapchain::getSurfaceFormats(){
		uint32 count;
		RtArray<VkSurfaceFormatKHR>	formats;
		vkGetPhysicalDeviceSurfaceFormatsKHR(core::dvc::graphics.PD.device, bindedWindow->surface, &count, nullptr);
		if(count != 0) {
			formats.resize(count);
			vkGetPhysicalDeviceSurfaceFormatsKHR(core::dvc::graphics.PD.device, bindedWindow->surface, &count, formats.begin());
		}
		return formats;
	}


	RtArray<VkPresentModeKHR> Swapchain::getPresentModes(){
		uint32 count; RtArray<VkPresentModeKHR> presentModes;
		vkGetPhysicalDeviceSurfacePresentModesKHR(dvc::graphics.PD.device, bindedWindow->surface, &count, nullptr);
		if(count != 0) {
			presentModes.resize(count);
			vkGetPhysicalDeviceSurfacePresentModesKHR(dvc::graphics.PD.device, bindedWindow->surface, &count, presentModes.begin());
		}
		return presentModes;
	}
}