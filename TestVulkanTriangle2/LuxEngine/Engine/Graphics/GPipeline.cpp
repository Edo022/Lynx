
#include "LuxEngine/Engine/Engine.h"








void Engine::createRenderPass() {
	//Color
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapChainImageFormat;									//Swapchain image format
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;								//Multisampling samples
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;							//Clear the image before writing to it //TODO dont clear for better performances //TODO bug when not clearing
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;							//Save rendered image
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;				//Discard stencil
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;				//Discard stencil
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;						//Default layout
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;					//Presentation layout
	//create Ref
	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;												//Attachment index
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;			//Optimal layout for better performances



	//Subpass
	VkSubpassDescription subpass{};													//Create subpass descriptor
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;					//Set structure type
	subpass.colorAttachmentCount = 1;												//Set number of attachments
	subpass.pColorAttachments = &colorAttachmentRef;								//Previously created color attachment
	subpass.pDepthStencilAttachment = VK_NULL_HANDLE;							//Previously created depth attachment



	VkSubpassDependency dependencies[2];											//Dependencies for implicit convertion
	//From undefined to color
	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependencies[0].dstSubpass = 0;
	dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[0].dependencyFlags = 0;												//Default
	//From color to undefined
	dependencies[1].srcSubpass = 0;
	dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependencies[1].dependencyFlags = 0;												//Default


	//Render pass
	VkRenderPassCreateInfo renderPassInfo{};										//Create render pass infos
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;				//Set structure type
	renderPassInfo.attachmentCount = 1;												//Set number of attachments
	renderPassInfo.pAttachments = &colorAttachment;										//Set attachments
	renderPassInfo.subpassCount = 1;												//Set number of subpasses
	renderPassInfo.pSubpasses = &subpass;											//Set subpass
	renderPassInfo.dependencyCount = 2;												//Set number of dependencies
	renderPassInfo.pDependencies = dependencies;									//Set dependencies

	//Create render pass. Exit if an error occurs
	Try(vkCreateRenderPass(graphics.LD, &renderPassInfo, nullptr, &renderPass)) Exit("Failed to create render pass");
}




void Engine::createFramebuffers() {
	swapChainFramebuffers.resize(swapChainImageViews.size());

	for (uint64 i = 0; i < swapChainImageViews.size(); i++) {
		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = &swapChainImageViews[i];
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;

		Try(vkCreateFramebuffer(graphics.LD, &framebufferInfo, nullptr, &swapChainFramebuffers[i])) Exit("Failed to create framebuffer");
	}
}


