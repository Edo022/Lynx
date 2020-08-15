

#include "LuxEngine/Core/Graphics/GOutput.h"
#include "LuxEngine/Core/Core.h"
#include "LuxEngine/Core/Graphics/GSwapchain.h"
#include "LuxEngine/Core/Devices.h"








// Render pass and framebuffers -------------------------------------------------------------------------------------------------------------//







namespace lux::core::g::out{
	VkRenderPass	renderPass;
	const int32		renderMaxFramesInFlight = 8;
	bool			renderFramebufferResized = false;








	void createRenderPass( ) {
		//Color
		VkAttachmentDescription colorAttachment{
			.format{ swapchain::swapchainImageFormat },				//Swapchain image format
			.samples{ VK_SAMPLE_COUNT_1_BIT },						//Multisampling samples
			.loadOp{ VK_ATTACHMENT_LOAD_OP_DONT_CARE },				//Don't clear for better performance
			.storeOp{ VK_ATTACHMENT_STORE_OP_DONT_CARE },			//Don't save rendered image
			.stencilLoadOp{ VK_ATTACHMENT_LOAD_OP_DONT_CARE },		//Discard stencil
			.stencilStoreOp{ VK_ATTACHMENT_STORE_OP_DONT_CARE },	//Discard stencil
			.initialLayout{ VK_IMAGE_LAYOUT_UNDEFINED },			//Default layout
			.finalLayout{ VK_IMAGE_LAYOUT_PRESENT_SRC_KHR },		//Present layout
		};


		//create attachment reference
		VkAttachmentReference colorAttachmentRef{ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
		//Create subpass description
		VkSubpassDescription subpass{
			.pipelineBindPoint{ VK_PIPELINE_BIND_POINT_GRAPHICS },	//Set structure type
			.colorAttachmentCount{ 1 },								//Set number of attachments
			.pColorAttachments{ &colorAttachmentRef },				//Previously created color attachment
			.pDepthStencilAttachment{ VK_NULL_HANDLE },				//Previously created depth attachment
		};


		//Dependencies for implicit convertion
		VkSubpassDependency dependencies[2]{
			{	//From undefined to color
				.srcSubpass{ VK_SUBPASS_EXTERNAL },
				.dstSubpass{ 0 },
				.srcStageMask{ VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT },
				.dstStageMask{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT },
				.srcAccessMask{ VK_ACCESS_MEMORY_READ_BIT },
				.dstAccessMask{ VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT },
				.dependencyFlags{ 0 },
			}, { //From color to undefined
				.srcSubpass{ 0 },
				.dstSubpass{ VK_SUBPASS_EXTERNAL },
				.srcStageMask{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT },
				.dstStageMask{ VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT },
				.srcAccessMask{ VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT },
				.dstAccessMask{ VK_ACCESS_MEMORY_READ_BIT },
				.dependencyFlags{ 0 },
			}
		};


		//Render pass
		VkRenderPassCreateInfo renderPassInfo{ 						//Create render pass infos
			.sType{ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO },	//Set structure type
			.attachmentCount{ 1 },									//Set number of attachments
			.pAttachments{ &colorAttachment },						//Set attachments
			.subpassCount{ 1 },										//Set number of subpasses
			.pSubpasses{ &subpass },								//Set subpass
			.dependencyCount{ 2 },									//Set number of dependencies
			.pDependencies{ dependencies },							//Set dependencies
		};

		//Create render pass. Exit if an error occurs
		TryVk(vkCreateRenderPass(dvc::graphics.LD, &renderPassInfo, nullptr, &renderPass)) printError("Failed to create render pass", true, -1);
	}








	void createFramebuffers( ) {
		swapchain::swapchainFramebuffers.resize(swapchain::swapchainImageViews.size( ));

		for(uint32 i = 0; i < swapchain::swapchainImageViews.size( ); ++i) {
			VkFramebufferCreateInfo framebufferInfo{
				.sType{ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO },
				.renderPass{ renderPass },
				.attachmentCount{ 1 },
				.pAttachments{ &swapchain::swapchainImageViews[i] },
				.width{ swapchain::swapchainExtent.width },
				.height{ swapchain::swapchainExtent.height },
				.layers{ 1 },
			};
			TryVk(vkCreateFramebuffer(dvc::graphics.LD, &framebufferInfo, nullptr, &swapchain::swapchainFramebuffers[i])) printError("Failed to create framebuffer", true, -1);
		}
	}








	// Images -----------------------------------------------------------------------------------------------------------------------------------//









	VkImageView swapchainCreateImageView(const VkImage vImage, const VkFormat vFormat, const VkImageAspectFlags vAspectFlags) {
		VkImageViewCreateInfo viewInfo{
			.sType{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO },
			.image{ vImage },
			.viewType{ VK_IMAGE_VIEW_TYPE_2D },
			.format{ vFormat },
			.components{
				.r{ VK_COMPONENT_SWIZZLE_IDENTITY },
				.g{ VK_COMPONENT_SWIZZLE_IDENTITY },
				.b{ VK_COMPONENT_SWIZZLE_IDENTITY },
				.a{ VK_COMPONENT_SWIZZLE_IDENTITY },
		},
			.subresourceRange{
					.aspectMask{ vAspectFlags },
					.baseMipLevel{ 0 },
					.levelCount{ 1 },
					.baseArrayLayer{ 0 },
					.layerCount{ 1 },
			},
		};
		VkImageView imageView = VK_NULL_HANDLE;
		TryVk(vkCreateImageView(dvc::graphics.LD, &viewInfo, nullptr, &imageView)) printError("Failed to create texture image view", true, -1);
		return imageView;
	}
}