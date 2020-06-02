
#include "LuxEngine/Engine/Engine.h"








void Engine::createGraphicsPipeline() {
	//PipelineCreateInfo struct to create the graphics pipeline
	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;


	//Create the structures that will fill the pipelineCreateInfo
	//Create shader modules
	#define StageInfo(_module, _stage) VkPipelineShaderStageCreateInfo { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0, _stage, _module, "main", nullptr }
	uint32 size = 0;																								//Size of the shader code
	VkShaderModule vertShaderModule = createShaderModule(graphics.LD, readShaderFromFile(&size, VERT_PATH), &size);	//Create vertex   shader module from shader's file (It needs to be in a variable to be destroyed)
	VkShaderModule fragShaderModule = createShaderModule(graphics.LD, readShaderFromFile(&size, FRAG_PATH), &size);	//Create fragment shader module from shader's file (It needs to be in a variable to be destroyed)

	//Create shader stages
	VkPipelineShaderStageCreateInfo shaderStages[] = { StageInfo(vertShaderModule, VK_SHADER_STAGE_VERTEX_BIT), StageInfo(fragShaderModule, VK_SHADER_STAGE_FRAGMENT_BIT) };
	pipelineInfo.stageCount = 2;													//Set the number of stages
	pipelineInfo.pStages = shaderStages;											//Set stages


	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};							//Create the vertex input infos
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;	//Set structure type
	auto bindingDescription = Vertex::getBindingDescription();							//Get binding   descriptions
	auto attributeDescriptions = Vertex::getAttributeDescriptions();					//Get attrubute descriptions
	vertexInputInfo.vertexBindingDescriptionCount = 1;									//Set number of binding   descriptions
	vertexInputInfo.vertexAttributeDescriptionCount = (uint32)(attributeDescriptions.size());//Set number of attribute descriptions
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;					//Set binding   descriptions
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data(0);		//Set attribute descriptions
	pipelineInfo.pVertexInputState = &vertexInputInfo;								//Save vertex input


	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};							//Create input assembly infos	
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;	//Set structure type
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;						//Set topology type
	inputAssembly.primitiveRestartEnable = VK_FALSE;									//Don't use the primitive restart system
	pipelineInfo.pInputAssemblyState = &inputAssembly;								//Save input assembly


	VkViewport viewport{};															//Create the viewport. It's the area where to render
	viewport.x = 0.0f;																	//Viewport start position
	viewport.y = 0.0f;																	//Viewport start position
	viewport.width = (float)swapChainExtent.width;										//Viewport width (maximum)
	viewport.height = (float)swapChainExtent.height;									//Viewport height (maximum)
	viewport.minDepth = 0.0f;															//Viewport depth (Vulkan default)
	viewport.maxDepth = 1.0f;															//Viewport depth (Vulkan default)

	VkRect2D scissor{};																//Create the scrissor. It cuts the viewport
	scissor.offset = { 0, 0 };															//Scrissor start position
	scissor.extent = swapChainExtent;													//Scrissor size. (don't cut the viewport)

	VkPipelineViewportStateCreateInfo viewportState{};								//Create viewport state infos. This structure contains the informations about viewports and scissors
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;		//Set structure type
	viewportState.viewportCount = 1;													//Set number of viewports
	viewportState.pViewports = &viewport;												//Set viewports
	viewportState.scissorCount = 1;														//Set number of scissors
	viewportState.pScissors = &scissor;													//Set scissors
	pipelineInfo.pViewportState = &viewportState;									//Save viewport state


	VkPipelineRasterizationStateCreateInfo rasterizer{};							//Rasterizer to convert primitives into pixels
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;		//Set structure type
	rasterizer.depthClampEnable = VK_FALSE;												//Clamp based on fragment depth (Required depth clamp feature)
	rasterizer.rasterizerDiscardEnable = VK_FALSE;										//Some dark magic I don't understand
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;										//Define how to draw triangles
	rasterizer.lineWidth = 1.0f;														//line thickness
	rasterizer.cullMode = VK_CULL_MODE_NONE;											//Faces to cull. I don't cull any face since i only need 2 triangles to render the texture
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;								//Define which side is visible
	rasterizer.depthBiasEnable = VK_FALSE;												//Depth biases to fix shadow glitches
	pipelineInfo.pRasterizationState = &rasterizer;									//Save rasterizer


	VkPipelineMultisampleStateCreateInfo multisampling{};							//Create multisapling for antialiasing
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;		//Set structure type
	multisampling.sampleShadingEnable = VK_FALSE;										//Whether to use the sample shading (Requires minSampleShading definition)
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;							//Number of samples to use
	pipelineInfo.pMultisampleState = &multisampling;								//Save multisampling infos


	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlending{};							//Create color blending infos
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;		//Set structure type
	colorBlending.logicOpEnable = VK_FALSE;												//Whether to apply logical operations
	colorBlending.logicOp = VK_LOGIC_OP_COPY;											//Logical operations
	colorBlending.attachmentCount = 1;													//Number of color blend attachments
	colorBlending.pAttachments = &colorBlendAttachment;									//Color blend attachments      ↓ RGBA blend color
	colorBlending.blendConstants[0] = colorBlending.blendConstants[1] = colorBlending.blendConstants[2] = colorBlending.blendConstants[3] = 0.0f;
	pipelineInfo.pColorBlendState = &colorBlending;									//Save color blending infos


	VkPipelineDepthStencilStateCreateInfo depthStencil{};							//Depth stencil infos 
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;	//Set structure type
	depthStencil.depthTestEnable = VK_FALSE;											//Don't use depth test
	depthStencil.depthWriteEnable = VK_FALSE;											//Don't use depth writes
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;									//Logical operation used to compare depths
	depthStencil.depthBoundsTestEnable = VK_FALSE;										//Don't limit the depth
	depthStencil.minDepthBounds = 0.0f;													//Maximum depth for depth bounds
	depthStencil.maxDepthBounds = 1.0f;													//Minimum depth for depth bounds
	depthStencil.stencilTestEnable = VK_FALSE;											//Don't use stencil test
	depthStencil.front;																	//Some dark magic
	depthStencil.back;																	//Some dark magic
	pipelineInfo.pDepthStencilState = &depthStencil;								//Save depth stencil


	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};								//Pipeline's layout infos
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;			//Set structure type
	pipelineLayoutInfo.setLayoutCount = 1;												//Number of descriptor set layouts
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;								//Descriptor set layouts (Samplers and uniform buffers)
	pipelineLayoutInfo.pushConstantRangeCount = 0;										//Number of push constants
	pipelineLayoutInfo.pPushConstantRanges = nullptr;									//Push constants		↓ Create pipeline layout
	Try(vkCreatePipelineLayout(graphics.LD, &pipelineLayoutInfo, nullptr, &pipelineLayout)) Quit("Failed to create pipeline layout");
	pipelineInfo.layout = pipelineLayout;											//Save pipeline layout



	//Create the graphics pipeline
	Try(vkCreateGraphicsPipelines(graphics.LD, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline)) Quit("Failed to create graphics pipeline");

	vkDestroyShaderModule(graphics.LD, fragShaderModule, nullptr);				//Destroy vertex   shader's shader module
	vkDestroyShaderModule(graphics.LD, vertShaderModule, nullptr);				//Destroy fragment shader's shader module
}








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

	//Depth
	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = findDepthFormat();										//Attachment format
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;								//Multisampling samples
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;							//Clear the image before writing to it //TODO don't clear. better performances //TODO bug when not clearing
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;						//Discard the image after being rendered
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;				//Discard stencil
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;				//Discard stencil
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;						//Default layout
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;	//Optimal layout for better performances
	//create Ref
	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	//Subpass
	VkSubpassDescription subpass{};													//Create subpass descriptor
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;					//Set structure type
	subpass.colorAttachmentCount = 1;												//Set number of attachments
	subpass.pColorAttachments = &colorAttachmentRef;								//Previously created color attachment
	subpass.pDepthStencilAttachment = &depthAttachmentRef;							//Previously created depth attachment



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
	VkAttachmentDescription attachments[] = { colorAttachment, depthAttachment };
	VkRenderPassCreateInfo renderPassInfo{};										//Create render pass infos
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;				//Set structure type
	renderPassInfo.attachmentCount = 2;												//Set number of attachments
	renderPassInfo.pAttachments = attachments;										//Set attachments
	renderPassInfo.subpassCount = 1;												//Set number of subpasses
	renderPassInfo.pSubpasses = &subpass;											//Set subpass
	renderPassInfo.dependencyCount = 2;												//Set number of dependencies
	renderPassInfo.pDependencies = dependencies;									//Set dependencies

	//Create render pass. Exit if an error occurs
	Try(vkCreateRenderPass(graphics.LD, &renderPassInfo, nullptr, &renderPass)) Quit("Failed to create render pass");
}




void Engine::createFramebuffers() {
	swapChainFramebuffers.resize(swapChainImageViews.size());

	for (uint64 i = 0; i < swapChainImageViews.size(); i++) {
		LuxDynArray<VkImageView> attachments(2, 2);
		attachments.add(swapChainImageViews[i]);
		attachments.add(depthImageView);

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = (uint32)(attachments.size());
		framebufferInfo.pAttachments = attachments.data(0);
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;

		Try(vkCreateFramebuffer(graphics.LD, &framebufferInfo, nullptr, &swapChainFramebuffers[i])) Quit("Failed to create framebuffer");
	}
}








// Descriptors ------------------------------------------------------------------------------------------------------------------------------//








void Engine::createDescriptorPool() {
	LuxArray<VkDescriptorPoolSize> poolSizes(2);
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32>(swapChainImages.size());
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32>(swapChainImages.size());

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32>(swapChainImages.size());

	Try(vkCreateDescriptorPool(graphics.LD, &poolInfo, nullptr, &descriptorPool)) Quit("Failed to create descriptor pool");
}




void Engine::createDescriptorSetLayout() {
	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	LuxArray<VkDescriptorSetLayoutBinding> bindings = { samplerLayoutBinding };
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	Try(vkCreateDescriptorSetLayout(graphics.LD, &layoutInfo, nullptr, &descriptorSetLayout)) {
		Quit("Failed to create descriptor set layout");
	}
}




void Engine::createDescriptorSets() {
	LuxArray<VkDescriptorSetLayout> layouts;
	layouts.resize(swapChainImages.size(), descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32>(swapChainImages.size());
	allocInfo.pSetLayouts = layouts.data();

	descriptorSets.resize(swapChainImages.size());
	Try(vkAllocateDescriptorSets(graphics.LD, &allocInfo, descriptorSets.data())) Quit("Failed to allocate descriptor sets");

	for (uint64 i = 0; i < descriptorSets.size(); i++) {
		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = textureImageView;
		imageInfo.sampler = textureSampler;

		LuxArray<VkWriteDescriptorSet> descriptorWrites(1);
		descriptorWrites[0] = {};
		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = descriptorSets[i];
		descriptorWrites[0].dstBinding = 1;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(graphics.LD, static_cast<uint32>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}
