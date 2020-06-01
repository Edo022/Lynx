

#include "LuxEngine/Engine/Engine.h"





void Engine::run(bool _useVSync, float _FOV) {
	runRender(_useVSync, _FOV);
	RunCompute();

	Success printf("Starting Mandragora Engine\n");					mainLoop();			MainSeparator;
	Normal  printf("Cleaning memory");		 cleanupRender(); cleanupCompute();			NewLine;

	vkDestroyInstance(instance, nullptr);
	glfwDestroyWindow(window);
	glfwTerminate();
}



void Engine::runRender(bool _useVSync, float _FOV) {
	useVSync = _useVSync;
	FOV = _FOV;
	stdTime start = now;

	if (enableValidationLayers) { Failure printf("D E B U G    M O D E"); }				MainSeparator;
	Normal  printf("Initializing GLFW window                 ");	initWindow();		SuccessNoNl printf("ok\n");
	Normal  printf("Initializing Vulkan");							initVulkan();		MainSeparator;
	Success printf("Initialization completed in %f s", ((stdDuration)(now - start)).count());
}




void Engine::initWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	window = glfwCreateWindow(WIDTH, HEIGHT, "Mandragora Engine", nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}


void Engine::framebufferResizeCallback(GLFWwindow* window, int32 width, int32 height) {
	auto engine = reinterpret_cast<Engine*>(glfwGetWindowUserPointer(window));
	engine->framebufferResized = true;
}



void Engine::initVulkan() {
	//Initialize vulkan
	Normal printf("    Creating Instance...                 ");		createInstance();					SuccessNoNl printf("ok");
	Normal printf("    Creating VK Surface...               ");		createSurface();					SuccessNoNl printf("ok");	NewLine;
	Normal printf("    Searching for physical devices...    ");		getPhysicalDevices();				NewLine;
	Normal printf("    Creating VK command pool...          ");		createGraphicsCommandPool();		SuccessNoNl printf("ok");
	/**/												 			setupDebugMessenger();

	//Create textures
	bufferSize = sizeof(Pixel) * COMPUTE_WIDTH * COMPUTE_HEIGHT;	//Set the size of the buffer
	createBuffer(compute.LD, bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, buffer, bufferMemory);
	createTextureImage();
	createTextureImageView();
	createTextureSampler();

	//Create an object for the render
	createVertexBuffer();
	createIndexBuffer();

	//Create swapchain render components
	createDescriptorSetLayout();
	Normal printf("    Creating VK swapchain...             ");		createSwapChain();					SuccessNoNl printf("ok");
	createSyncObjects();
	createDrawCommandBuffers();
}






VkFormat Engine::findSupportedFormat(LuxStaticArray<VkFormat> candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
	for (VkFormat format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(graphics.PD.device, format, &props);

		if ((tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) ||
			(tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)) {
			return format;
		}
	}
	Quit("Failed to find a supported format");
}










VkCommandBuffer Engine::beginSingleTimeCommands() {
	//Create allocate info
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = graphicsCommandPool;
	allocInfo.commandBufferCount = 1;

	//Allocate command buffer
	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(graphics.LD, &allocInfo, &commandBuffer);

	//Begine command recording
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	//Return the created command buffer
	return commandBuffer;
}




void Engine::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
	//End command recording
	vkEndCommandBuffer(commandBuffer);

	//Submit command buffer to queue
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;
	vkQueueSubmit(graphics.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);

	//Free memory
	vkQueueWaitIdle(graphics.graphicsQueue);
	vkFreeCommandBuffers(graphics.LD, graphicsCommandPool, 1, &commandBuffer);
}









void Engine::FPSCounter() {
	while (running) {
		Normal printf("%lf", FPS);
		sleep(1000);
	}
}




void Engine::mainLoop() {
	std::thread FPSCounterThr(&Engine::FPSCounter, this);
	FPSCounterThr.detach();
	stdTime start;
	bool fullScreen = false;

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		drawFrame();


		//TODO fix full screen
		static int32 lastState;
		static int32 lastw = 0;
		static int32 lasth = 0;
		if (glfwGetKey(window, GLFW_KEY_F11) != lastState && glfwGetKey(window, GLFW_KEY_F11) == GLFW_RELEASE) {
			if (fullScreen) {
				fullScreen = false;
				glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_TRUE);
				glfwSetWindowSize(window, lastw, lasth);
			}
			else {
				fullScreen = true;
				glfwGetWindowSize(window, &lastw, &lasth);
				glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE);
				glfwMaximizeWindow(window);
			}
		}
		lastState = glfwGetKey(window, GLFW_KEY_F11);

		//TODO mouse events
		//printf("%d\n", glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1));

		//FPS counter
		stdDuration elapsed_seconds = now - start;
		FPS = 1 / elapsed_seconds.count();
		start = now;
	}
	running = false;

	vkDeviceWaitIdle(graphics.LD);
}



void Engine::cleanupSwapChain() {
	vkDestroyImage(graphics.LD, depthImage, nullptr);				//Destroy depth image
	vkDestroyImageView(graphics.LD, depthImageView, nullptr);		//Destroy depth image view
	vkFreeMemory(graphics.LD, depthImageMemory, nullptr);			//Free depth image memory

	vkDestroyPipeline(graphics.LD, graphicsPipeline, nullptr);		//Destroy pipeline
	vkDestroyPipelineLayout(graphics.LD, pipelineLayout, nullptr);	//Destroy pipeline layout
	vkDestroyRenderPass(graphics.LD, renderPass, nullptr);			//Destroy render pass

	for (auto framebuffer : swapChainFramebuffers) vkDestroyFramebuffer(graphics.LD, framebuffer, nullptr);			//Destroy framebuffers
	vkFreeCommandBuffers(graphics.LD, graphicsCommandPool, (uint32)(commandBuffers.size()), commandBuffers.data());	//Free graphics command buffers
	for (auto imageView : swapChainImageViews) vkDestroyImageView(graphics.LD, imageView, nullptr);					//Destroy image views
	vkDestroySwapchainKHR(graphics.LD, swapChain, nullptr);															//destroy swapchain
	vkDestroyDescriptorPool(graphics.LD, descriptorPool, nullptr);													//Destroy graphics descriptor pool
}




void Engine::cleanupRender() {
	cleanupSwapChain();																//Clear swapchain components

	vkDestroySampler(graphics.LD, textureSampler, nullptr);							//Destroy sampler
	vkDestroyDescriptorSetLayout(graphics.LD, descriptorSetLayout, nullptr);		//Destroy descriptor set layout
	vkDestroyCommandPool(graphics.LD, graphicsCommandPool, nullptr);				//Destroy graphics command pool

	vkDestroyImage(graphics.LD, textureImage, nullptr);								//Destroy texture image
	vkDestroyImageView(graphics.LD, textureImageView, nullptr);						//Destroy texture image view
	vkFreeMemory(graphics.LD, textureImageMemory, nullptr);							//Free texture image memory

	vkDestroyBuffer(graphics.LD, vertexBuffer, nullptr);							//Destroy vertex buffer
	vkFreeMemory(graphics.LD, vertexBufferMemory, nullptr);							//Free vertex buffer memory
	vkDestroyBuffer(graphics.LD, indexBuffer, nullptr);								//Destroy index buffer
	vkFreeMemory(graphics.LD, indexBufferMemory, nullptr);							//Free index buffer memory

	for (int64 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {								//For every frame
		vkDestroySemaphore(graphics.LD, renderFinishedSemaphores[i], nullptr);			//Destroy his render semaphore
		vkDestroySemaphore(graphics.LD, imageAvailableSemaphores[i], nullptr);			//Destroy his image  semaphore
		vkDestroyFence(graphics.LD, inFlightFences[i], nullptr);						//Destroy his fence
	}


	if (graphics.PD.properties.deviceID != compute.PD.properties.deviceID) vkDestroyDevice(graphics.LD, nullptr);	//If the compute and the graphics devices are not the same, destroy the graphics device
	vkDestroyDevice(compute.LD, nullptr);																			//Destroy the compute device
	for(auto device:secondary) vkDestroyDevice(device.LD, nullptr);													//Destroy all the secondary devices

	if (enableValidationLayers) DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);					//Destroy the debug messenger if present
	vkDestroySurfaceKHR(instance, surface, nullptr);																//Destroy the vulkan surface
}


//Dark magic
#define populateDebugMessengerCreateInfo(createInfo)\
	createInfo = {};\
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;\
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;\
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;\
	createInfo.pfnUserCallback = debugCallback;






//Create the Vulkan instance, using validation layers when in debug mode
void Engine::createInstance() {
	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;


	//Application infos
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Mandragora";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "MandragoraEngine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;
	createInfo.pApplicationInfo = &appInfo;


	//Extensions
	LuxArray<const char*> extensions;
	uint32 glfwExtensionCount;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);	//Get extensions list and count
	for (uint32 i = 0; i < glfwExtensionCount; i++) extensions.add(glfwExtensions[i]);		//Save them into an array
	if (enableValidationLayers) extensions.add(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);			//Add debug extension if in debug mode
	createInfo.enabledExtensionCount = static_cast<uint32>(extensions.size());				//Set extension count
	createInfo.ppEnabledExtensionNames = extensions.data(0);								//Set extensions


	//Add validation layers if in debug mode
	#ifdef NDEBUG
	createInfo.enabledLayerCount = 0;
	createInfo.pNext = nullptr;
	#else
	//Search for validation layers
	uint32 layerCount = 0;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);				//Get layer count
	LuxStaticArray<VkLayerProperties> availableLayers((uint64)layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());	//Get layers
	for (const char* layerName : validationLayers) {											//For every layer,
		for (const auto& layerProperties : availableLayers) {										//Check if it's available
			if (strcmp(layerName, layerProperties.layerName) == 0) break;
			else if(strcmp(layerName, availableLayers.end()->layerName) == 0) Quit("Validation layers not available. Cannot run in debug mode.");
		}
	}

	//Set debugCreateInfo structure
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
	createInfo.enabledLayerCount = static_cast<uint32>(validationLayers.size());
	createInfo.ppEnabledLayerNames = validationLayers.data();
	populateDebugMessengerCreateInfo(debugCreateInfo);
	createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	#endif


	//Create instance
	Try(vkCreateInstance(&createInfo, nullptr, &instance)) Quit("Failed to create instance");
}




void Engine::setupDebugMessenger() {
	if (!enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	populateDebugMessengerCreateInfo(createInfo);
	Try(CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger)) Quit("Failed to set up debug messenger");
}

void Engine::createSurface() {
	Try(glfwCreateWindowSurface(instance, window, nullptr, &surface)) Quit("Failed to create window surface");
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



void Engine::createDescriptorSetLayout() {
	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::array<VkDescriptorSetLayoutBinding, 2> bindings = { samplerLayoutBinding };
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	Try(vkCreateDescriptorSetLayout(graphics.LD, &layoutInfo, nullptr, &descriptorSetLayout)) {
		Quit("Failed to create descriptor set layout");
	}
}


//Creates a shadere module from a compiled shader code and its size in bytes
VkShaderModule Engine::createShaderModule(VkDevice device, uint32* code, uint32* size) {
	VkShaderModuleCreateInfo createInfo{};								//Create shader module infos
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;		//Set structure type
	createInfo.codeSize = *size;										//Set the size of the compiled shader code
	createInfo.pCode = code;											//Set the shader code

	VkShaderModule shaderModule;										//Create the shader module
	Try(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule)) Quit("Failed to create shader module");
	return shaderModule;												//Return the created shader module
}




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







void Engine::createFramebuffers() {
	swapChainFramebuffers.resize(swapChainImageViews.size());

	for (uint64 i = 0; i < swapChainImageViews.size(); i++) {
		LuxArray<VkImageView> attachments(2, 2);
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



void Engine::createGraphicsCommandPool() {
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = graphics.PD.indices.graphicsFamily;

	Try(vkCreateCommandPool(graphics.LD, &poolInfo, nullptr, &graphicsCommandPool)) Quit("Failed to create graphics command pool");
}




void Engine::createDescriptorPool() {
	std::array<VkDescriptorPoolSize, 2> poolSizes{};
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



void Engine::createDescriptorSets() {
	LuxStaticArray<VkDescriptorSetLayout> layouts;
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

		std::array<VkWriteDescriptorSet, 1> descriptorWrites{};

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



void Engine::createBuffer(VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	Try(vkCreateBuffer(device, &bufferInfo, nullptr, &buffer)) Quit("Failed to create buffer");

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	Try(vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory)) Quit("Failed to allocate buffer memory");
	Try(vkBindBufferMemory(device, buffer, bufferMemory, 0)) Quit("Failed to bind buffer");
}



//Creates and submits a command buffer to copy from srcBuffer to dstBuffer
void Engine::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
	VkBufferCopy copyRegion{};												//Create buffer copy object
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();				//Start command buffer
	copyRegion.size = size;													//Set size of the copied region
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);	//Record the copy command
	endSingleTimeCommands(commandBuffer);									//End command buffer
}




//Returns the index of the memory with the specified type and properties. Exits if not found
uint32 Engine::findMemoryType(uint32 typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;							//Get memory properties
	vkGetPhysicalDeviceMemoryProperties(graphics.PD.device, &memProperties);

	for (uint32 i = 0; i < memProperties.memoryTypeCount; i++) {			//Search for the memory that has the specified properties and type and return its index
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) return i; 
	}
	Quit("Failed to find suitable memory type");
}




void Engine::createDrawCommandBuffers() {
	commandBuffers.resize(swapChainFramebuffers.size());				//One command buffer for every swapchain's framebuffer 

	VkCommandBufferAllocateInfo allocInfo{};							//Create allocate infos
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;	//Set structure type
	allocInfo.commandPool = graphicsCommandPool;						//Set command pool	
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;					//Set command buffer level
	allocInfo.commandBufferCount = (uint32)commandBuffers.size();		//Set number of command buffers
	//Allocate command buffers
	Try(vkAllocateCommandBuffers(graphics.LD, &allocInfo, commandBuffers.data())) Quit("Failed to allocate command buffers");

	for (uint64 i = 0; i < commandBuffers.size(); i++) {				//For every command buffer
		VkCommandBufferBeginInfo beginInfo{};								//Create begin info struct
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;		//Set structure type

		VkClearValue clearValues[2];										//Create clear values
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };					//Color clear value
		clearValues[1].depthStencil = { 1.0f, 0 };							//Stencil clear value

		VkRenderPassBeginInfo renderPassInfo{};								//Create render pass infos
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;	//Set structure type
		renderPassInfo.renderPass = renderPass;								//Set render pass
		renderPassInfo.framebuffer = swapChainFramebuffers[i];				//Set frame buffer
		renderPassInfo.renderArea.offset = { 0, 0 };						//No offset
		renderPassInfo.renderArea.extent = swapChainExtent;					//Maximum extent
		renderPassInfo.clearValueCount = 2;									//Clear values number
		renderPassInfo.pClearValues = clearValues;							//Set clear values


		//Begin command buffer and render pass
		Try(vkBeginCommandBuffer(commandBuffers[i], &beginInfo)) Quit("Failed to begin command buffer");
		vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		//Bind pipeline, vertices, indices and descriptors to che command buffer
		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
		vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, &vertexBuffer, new VkDeviceSize(0));
		vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT32); //LLID0
		vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);
		//Draw texture
		vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32>(indices.size()), 1, 0, 0, 0);

		//Eng command buffer and render pass
		vkCmdEndRenderPass(commandBuffers[i]);
		Try(vkEndCommandBuffer(commandBuffers[i])) Quit("Failed to record command buffer");
	}
}


void Engine::createSyncObjects() {
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (int64 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(graphics.LD, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(graphics.LD, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(graphics.LD, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
			Quit("Failed to create synchronization objects for a frame");
		}
	}
}




void Engine::drawFrame() {
	//Wait fences
	vkWaitForFences(graphics.LD, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	//Update render result
	runCommandBuffer();
	transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	copyBufferToImage(buffer, textureImage, WIDTH, HEIGHT);
	transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	//Acquire swapchain image
	uint32 imageIndex;
	VkResult result = vkAcquireNextImageKHR(graphics.LD, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR) { recreateSwapChain(); return; }
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) Quit("Failed to acquire swapchain image");

	if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) vkWaitForFences(graphics.LD, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
	imagesInFlight[imageIndex] = inFlightFences[currentFrame];


	//Submit to queue
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(graphics.LD, 1, &inFlightFences[currentFrame]);
	Try(vkQueueSubmit(graphics.graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame])) Quit("Failed to submit draw command buffer");


	//Present
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	VkSwapchainKHR swapChains[] = { swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	result = vkQueuePresentKHR(graphics.presentQueue, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
		framebufferResized = false;
		recreateSwapChain();
		vkDeviceWaitIdle(graphics.LD);
	}
	else Try(result) Quit("Failed to present swapchain image");

	//Update frame number
	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}







//Read a shader from a file and save it in a padded char array
uint32* Engine::readShaderFromFile(uint32* length, const char* filename) {
	FILE* fp = fopen(filename, "rb");
	if (fp == NULL) printf("Could not find or open file: %s\n", filename);

	//Get file size.
	fseek(fp, 0, SEEK_END);
	int32 filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	int32 paddedFileSize = int32(ceil(filesize / 4.0)) * 4;

	//Read file contents.
	char* str = (char*)malloc(sizeof(char) * paddedFileSize);
	fread(str, filesize, sizeof(char), fp);
	fclose(fp);

	//Data padding. 
	for (int32 i = filesize; i < paddedFileSize; i++) str[i] = 0;

	*length = paddedFileSize;
	return (uint32*)str;
}









VKAPI_ATTR VkBool32 VKAPI_CALL Engine::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
}
