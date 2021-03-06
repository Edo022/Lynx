#include "LuxEngine/Core/Render/GOutput.hpp"
#include "LuxEngine/Core/Render/Window/Swapchain.hpp"
#include "LuxEngine/Core/Devices.hpp"
#include "LuxEngine/Core/Core.hpp"
#include "LuxEngine/Core/LuxAutoInit.hpp"








// Render pass and framebuffers -------------------------------------------------------------------------------------------------------------//








namespace lux::core::render::out{
	alignCache VkRenderPass	renderPass               = nullptr;
	alignCache bool			renderFramebufferResized = false;








	void createRenderPass() {
		//Color
		VkAttachmentDescription colorAttachment{
			.format         = lux::window.swapchain.swapchainImageFormat,		//Swapchain image format //FIXME DONT DEPEND ON A WINDOW
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








	void createFramebuffers() {
		lux::window.swapchain.swapchainFramebuffers.resize(lux::window.swapchain.swapchainImageViews.count()); //FIXME DONT DEPEND ON A WINDOW

		for(uint32 i = 0; i < lux::window.swapchain.swapchainImageViews.count(); ++i) { //FIXME DONT DEPEND ON A WINDOW
			VkFramebufferCreateInfo framebufferInfo{
				.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
				.renderPass      = renderPass,
				.attachmentCount = 1,
				.pAttachments    = &lux::window.swapchain.swapchainImageViews[i], //FIXME DONT DEPEND ON A WINDOW
				.width           = lux::window.swapchain.swapchainExtent.width, //FIXME DONT DEPEND ON A WINDOW
				.height          = lux::window.swapchain.swapchainExtent.height, //FIXME DONT DEPEND ON A WINDOW
				.layers          = 1
			};
			dbg::checkVk(vkCreateFramebuffer(dvc::graphics.LD, &framebufferInfo, nullptr, &lux::window.swapchain.swapchainFramebuffers[i]), "Failed to create framebuffer"); //FIXME DONT DEPEND ON A WINDOW
		}
	}








	// Images -----------------------------------------------------------------------------------------------------------------------------------//








	VkImageView swapchainCreateImageView(const VkImage vImage, const VkFormat vFormat, const VkImageAspectFlags vAspectFlags) {
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
}