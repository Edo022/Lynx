#include "LuxEngine/Core/Render/Window/Swapchain.hpp"
#include "LuxEngine/Core/Render/Shaders/Shader.hpp"
#include "LuxEngine/Core/Render/GCommands.hpp"
#include "LuxEngine/Core/Devices.hpp"
#include "LuxEngine/Core/Core.hpp"
#include "LuxEngine/Core/LuxAutoInit.hpp"








namespace lux::core::wnd{
	VkSurfaceFormatKHR Swapchain::swapchainChooseSurfaceFormat(const RtArray<VkSurfaceFormatKHR>& pAvailableFormats) {
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
	VkPresentModeKHR Swapchain::swapchainChoosePresentMode(const RtArray<VkPresentModeKHR>& pAvailablePresentModes) {
		if(useVSync) return VK_PRESENT_MODE_FIFO_KHR; //FIXME MOVE VSYNC TO WINDOW STRUCT
		for(const auto& availablePresentMode : pAvailablePresentModes) {
			if(availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) return availablePresentMode;
		}
		return VK_PRESENT_MODE_IMMEDIATE_KHR;
	}




	VkExtent2D Swapchain::swapchainChooseExtent(const VkSurfaceCapabilitiesKHR* pCapabilities) {
		int32 width = 0, height = 0;
		glfwGetFramebufferSize(bindedWindow->window, &width, &height);
		return VkExtent2D{
			max(pCapabilities->minImageExtent.width,  min(pCapabilities->maxImageExtent.width , (uint32)width)),
			max(pCapabilities->minImageExtent.height, min(pCapabilities->maxImageExtent.height, (uint32)height))
		};
	}








	//-------------------------------------------------------------------------------------------------------------------------------------------------//








	void Swapchain::swapchainCreate() {
		___init(false);




		//Get swapchain details
		SwapChainSupportDetails swapChainSupport = getSwapchainSupportDetails(dvc::graphics.PD.device, bindedWindow->surface); //FIXME DONT USE GLOBAL SURFACE
		//FIXME ^ USE BINDED WINDOW

		//Choose max image count. Minimum or minimum +1 if supported
		uint32 imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if(swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}


		//swapchain creation infos
		VkSurfaceFormatKHR surfaceFormat{ swapchainChooseSurfaceFormat(swapChainSupport.formats) };
		VkSwapchainCreateInfoKHR createInfo{
			.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.surface          = bindedWindow->surface,
			.minImageCount    = imageCount,
			.imageFormat      = surfaceFormat.format,
			.imageColorSpace  = surfaceFormat.colorSpace,
			.imageExtent      = swapchainChooseExtent(&swapChainSupport.capabilities),
			.imageArrayLayers = 1,
			.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT
		};

		uint32 queueFamilyIndices[] = { dvc::graphics.PD.indices.graphicsFamily, dvc::graphics.PD.indices.presentFamily };
		if(dvc::graphics.PD.indices.graphicsFamily != dvc::graphics.PD.indices.presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = swapchainChoosePresentMode(swapChainSupport.presentModes);
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;


		//Create swapchain
		VkBool32 hasPresentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(dvc::graphics.PD.device, dvc::graphics.PD.indices.presentFamily, bindedWindow->surface, &hasPresentSupport); //SUPPRESS ERROR //FIXME
		dbg::checkVk(vkCreateSwapchainKHR(dvc::graphics.LD, &createInfo, nullptr, &swapchain), "Failed to create swapchain");


		//Save data
		uint32 swapchainImageCount;
		vkGetSwapchainImagesKHR(dvc::graphics.LD, swapchain, &swapchainImageCount, nullptr);					//Get image count
		swapchainImages.resize(swapchainImageCount);
		vkGetSwapchainImagesKHR(dvc::graphics.LD, swapchain, &swapchainImageCount, swapchainImages.begin());	//Save images
		swapchainImageFormat = surfaceFormat.format;															//Save format
		swapchainExtent = createInfo.imageExtent;																//Save extent


		//Create image views
		swapchainImageViews.resize(swapchainImages.count());
		for(uint32 i = 0; i < swapchainImages.count(); ++i) swapchainImageViews[i] = swapchainCreateImageView(swapchainImages[i], swapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);


		createRenderPass(); //FIXME MOVE TO SWAPCHAIN
		createFramebuffers();
	}








	void Swapchain::cleanup() {
		vkDestroyRenderPass(dvc::graphics.LD, renderPass, nullptr);												//Destroy render pass
		for(auto framebuffer : swapchainFramebuffers) vkDestroyFramebuffer(dvc::graphics.LD, framebuffer, nullptr);		//Destroy framebuffers
		for(auto imageView   : swapchainImageViews  ) vkDestroyImageView(  dvc::graphics.LD, imageView  , nullptr);		//Destroy image views
		vkDestroySwapchainKHR(dvc::graphics.LD, swapchain, nullptr);													//destroy swapchain
	}








	void Swapchain::swapchainRecreate(const bool vWindowResized) {
		if(vWindowResized) bindedWindow->windowResizeFence.lock();	//Sync with framebufferResizeCallback

		//TODO dont destroy it every time
		static int32 width, height;	glfwGetFramebufferSize(bindedWindow->window, &width, &height);
		if(width != 0 && height != 0) {			//If the window contains pixels
			vkDeviceWaitIdle(dvc::graphics.LD);		//Wait for the logical device
			cleanup();					//Clean the old swapchain
			swapchainCreate();			//Create a new swapchain


			//Update the window count buffer
			bindedWindow->wSize_g.map();
			bindedWindow->wSize_g[0] = swapchainExtent.width;
			bindedWindow->wSize_g[1] = swapchainExtent.height;
			bindedWindow->wSize_g.unmap();

			{	//Destroy copy command buffers
				vkFreeCommandBuffers(dvc::compute.LD, bindedWindow->copyCommandPool, bindedWindow->copyCommandBuffers.count(), bindedWindow->copyCommandBuffers.begin());
				vkDestroyCommandPool(dvc::compute.LD, bindedWindow->copyCommandPool, nullptr);

				//#LLID CCB0000 Recreate copy command buffers
				bindedWindow->copyCommandBuffers.resize(swapchainImages.count());	//Resize the command buffer array in the shader
				bindedWindow->createDefaultCommandBuffers__();				//Create command buffers and command pool
			}

			//Recreate clear shader
			c::shaders::updateShaderCall(bindedWindow->clearShader, LUX_DEF_SHADER_CLEAR, (swapchainExtent.width * swapchainExtent.height) / (32 * 32) + 1, 1, 1);
		}
		if(vWindowResized) bindedWindow->windowResizeFence.unlock();		//Sync with framebufferResizeCallback
	}







	void Swapchain::createFramebuffers() {
		swapchainFramebuffers.resize(swapchainImageViews.count()); //FIXME DONT DEPEND ON A WINDOW

		for(uint32 i = 0; i < swapchainImageViews.count(); ++i) { //FIXME DONT DEPEND ON A WINDOW
			VkFramebufferCreateInfo framebufferInfo{
				.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
				.renderPass      = renderPass,
				.attachmentCount = 1,
				.pAttachments    = &swapchainImageViews[i], //FIXME DONT DEPEND ON A WINDOW
				.width           = swapchainExtent.width, //FIXME DONT DEPEND ON A WINDOW
				.height          = swapchainExtent.height, //FIXME DONT DEPEND ON A WINDOW
				.layers          = 1
			};
			dbg::checkVk(vkCreateFramebuffer(dvc::graphics.LD, &framebufferInfo, nullptr, &swapchainFramebuffers[i]), "Failed to create framebuffer"); //FIXME DONT DEPEND ON A WINDOW
		}
	}






	void Swapchain::createRenderPass() {
		//Color
		VkAttachmentDescription colorAttachment{
			.format         = swapchainImageFormat,		//Swapchain image format //FIXME DONT DEPEND ON A WINDOW
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














		VkImageView Swapchain::swapchainCreateImageView(const VkImage vImage, const VkFormat vFormat, const VkImageAspectFlags vAspectFlags) {
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
		VkImageView imageView = VK_NULL_HANDLE;
		dbg::checkVk(vkCreateImageView(dvc::graphics.LD, &viewInfo, nullptr, &imageView), "Failed to create texture image view");
		return imageView;
	}



		void Swapchain::___init(const bool vUseVSync) {
		useVSync = vUseVSync;

		s_imageAquired   .resize(__renderMaxFramesInFlight);
s_objectsRendered.resize(__renderMaxFramesInFlight);
s_copy           .resize(__renderMaxFramesInFlight);
s_clear          .resize(__renderMaxFramesInFlight);
imageRendered_f  .resize(__renderMaxFramesInFlight);

		_dbg(Failure printf("D E B U G    M O D E"));		MainSeparator;

		//Initialize vulkan
		// dbg::checkVk(glfwCreateWindowSurface(instance, wnd::window, nullptr, &surface), "Failed to create window surface");
		Normal printf("    Searching for physical devices...    \n");
		// dvc::getPhysical(); //FIXME
		core::render::cmd::createGraphicsCommandPool();
		Normal printf("    Creating VK swapchain...             ");

		// lux::window.swapchain.swapchainCreate(); //FIXME AUTOMATIZE

		_dbg(lux::core::render::createDebugMessenger());


		//Create sync objects
		VkSemaphoreCreateInfo semaphoreInfo{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
		};
		VkFenceCreateInfo fenceInfo{
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.flags = VK_FENCE_CREATE_SIGNALED_BIT
		};
		for(int32 i = 0; i < __renderMaxFramesInFlight; ++i) {
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
}