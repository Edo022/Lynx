
#include "LuxEngine/Engine/Engine.h"









// Render pass and framebuffers -------------------------------------------------------------------------------------------------------------//








//TODO render pass seems to be useless
void Engine::createRenderPass() {
	//Color
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapchainImageFormat;									//Swapchain image format
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;								//Multisampling samples
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;						//Don't clear for better performance
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;						//Don't save rendered image
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;				//Discard stencil
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;				//Discard stencil
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;						//Default layout
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;					//Present layout
	//create Ref
	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;												//Attachment index
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;			//Optimal layout for better performances


	//Subpass
	VkSubpassDescription subpass{};													//Create subpass descriptor
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;					//Set structure type
	subpass.colorAttachmentCount = 1;												//Set number of attachments
	subpass.pColorAttachments = &colorAttachmentRef;								//Previously created color attachment
	subpass.pDepthStencilAttachment = VK_NULL_HANDLE;								//Previously created depth attachment


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
	renderPassInfo.pAttachments = &colorAttachment;									//Set attachments
	renderPassInfo.subpassCount = 1;												//Set number of subpasses
	renderPassInfo.pSubpasses = &subpass;											//Set subpass
	renderPassInfo.dependencyCount = 2;												//Set number of dependencies
	renderPassInfo.pDependencies = dependencies;									//Set dependencies

	//Create render pass. Exit if an error occurs
	TryVk(vkCreateRenderPass(graphics.LD, &renderPassInfo, nullptr, &renderPass)) Exit("Failed to create render pass");
}




void Engine::createFramebuffers() {
	swapchainFramebuffers.resize(swapchainImageViews.size());

	for (uint64 i = 0; i < swapchainImageViews.size(); ++i) {
		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = &swapchainImageViews[i];
		framebufferInfo.width = swapchainExtent.width;
		framebufferInfo.height = swapchainExtent.height;
		framebufferInfo.layers = 1;

		TryVk(vkCreateFramebuffer(graphics.LD, &framebufferInfo, nullptr, &swapchainFramebuffers[i])) Exit("Failed to create framebuffer");
	}
}








// Images -----------------------------------------------------------------------------------------------------------------------------------//









VkImageView Engine::swapchainCreateImageView(const VkImage vImage, const VkFormat vFormat, const VkImageAspectFlags vAspectFlags) {
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = vImage;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = vFormat;
	viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.subresourceRange.aspectMask = vAspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView imageView = VK_NULL_HANDLE;
	TryVk(vkCreateImageView(graphics.LD, &viewInfo, nullptr, &imageView)) Exit("Failed to create texture image view");
	return imageView;
}




void Engine::swapchainCopyBufferToImage(const VkBuffer vBuffer, const  VkImage vImage, const uint32 vWidth, const uint32 vHeight) {
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = { vWidth, vHeight, 1 };

	vkCmdCopyBufferToImage(commandBuffer, vBuffer, vImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
	endSingleTimeCommands(commandBuffer);
}

