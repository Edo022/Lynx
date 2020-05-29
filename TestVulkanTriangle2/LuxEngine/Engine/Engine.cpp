

#include "LuxEngine/Engine/Engine.h"






void Render::run(bool _useVSync, float _FOV) {
	//running = true; //set in lux init
	useVSync = _useVSync;
	FOV = _FOV;
	stdTime start = now;

	if (enableValidationLayers) { Failure printf("D E B U G    M O D E"); }				MainSeparator;
	Normal  printf("Initializing GLFW window                 ");	initWindow();		SuccessNoNl printf("ok\n");
	Normal  printf("Initializing Vulkan          ");

	initVulkan();




	MainSeparator;

	Success printf("Initialization completed in %f s", ((stdDuration)(now - start)).count());

	Success printf("Starting Mandragora Engine\n");					mainLoop();			MainSeparator;

	//running = false
	Normal  printf("Cleaning memory");								cleanup();			NewLine;
}




void Render::initWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	window = glfwCreateWindow(WIDTH, HEIGHT, "Mandragora Engine", nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}


void Render::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
	auto app = reinterpret_cast<Render*>(glfwGetWindowUserPointer(window));
	app->framebufferResized = true;
}



void Render::initVulkan() {
	//Initialize vulkan
	Normal printf("    Creating Instance...                 ");		createInstance();					SuccessNoNl printf("ok");
	/**/												 			setupDebugMessenger();
	Normal printf("    Creating VK Surface...               ");		createSurface();					SuccessNoNl printf("ok");	NewLine;
	Normal printf("    Searching for physical devices...    ");		getPhysicalDevices();				NewLine;
	Normal printf("    Creating VK command pool...          ");		createGraphicsCommandPool();		SuccessNoNl printf("ok");

	run2();

	//Create textures
	createTextureImage();
	createTextureImageView();
	createTextureSampler();

	//Create a void object for the render
	createVertexBuffer();
	createIndexBuffer();

	if (sc) createDrawCommandBuffers();

	//checkObjects();


	Normal printf("    Creating VK descriptor set layout... ");		createDescriptorSetLayout();		SuccessNoNl printf("ok");

	Normal printf("    Creating VK swapchain...             ");		createSwapChain();					SuccessNoNl printf("ok");

	Normal printf("    Creating VK sync objects...          ");		createSyncObjects();				SuccessNoNl printf("ok");
}







VkFormat Render::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
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




void Render::createTextureSampler() {
	VkSamplerCreateInfo samplerInfo{};								//Sampler infos
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_BEGIN_RANGE;					//VK_FILTER_LINEAR;  begin range removes filtering
	samplerInfo.minFilter = VK_FILTER_BEGIN_RANGE;					//VK_FILTER_LINEAR;  begin range removes filtering

	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;		///Repeat the texture
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;		///Repeat the texture
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;		///Repeat the texture

	samplerInfo.anisotropyEnable = VK_FALSE;						//VK_TRUE;  no need to enable the anistropy filter since we neither use meshes or textures
	samplerInfo.maxAnisotropy = 16;									//Maximum anistropy. Default at 16
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;		//Black background when the texture ends
	samplerInfo.unnormalizedCoordinates = VK_FALSE;					//Use normalized coordinates

	samplerInfo.compareEnable = VK_FALSE;							//Don't compare. We just need to put some pixels on the screen
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;					//(Useless)

	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_BEGIN_RANGE;	//VK_SAMPLER_MIPMAP_MODE_LINEAR;  begin range doesn't use mipmap
	samplerInfo.mipLodBias = 0.0f;									//?? idk sorry
	samplerInfo.minLod = 0.0f;										//Minimum level of detail
	samplerInfo.maxLod = 0.0f;										//Maximum level of detail

	//Create sampler, exit if an error occurs. Sampler is needed for the render
	Try(vkCreateSampler(graphics.LD, &samplerInfo, nullptr, &textureSampler)) Quit("Failed to create texture sampler");
}


void Render::createTextureImageView() {
	textureImageView = createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
}



void Render::createTextureImage() {
	runCommandBuffer();

	createImage(WIDTH, HEIGHT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory);
	transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	copyBufferToImage(buffer, textureImage, WIDTH, HEIGHT);
	transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}



void Render::createImage(uint32 width, uint32 height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	Try(vkCreateImage(graphics.LD, &imageInfo, nullptr, &image)) Quit("Failed to create image");

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(graphics.LD, image, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	Try(vkAllocateMemory(graphics.LD, &allocInfo, nullptr, &imageMemory)) Quit("Failed to allocate image memory");
	vkBindImageMemory(graphics.LD, image, imageMemory, 0);
}




VkCommandBuffer Render::beginSingleTimeCommands() {
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


void Render::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
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



void Render::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else Quit("Unsupported layout transition");

	vkCmdPipelineBarrier(
		commandBuffer,
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

	endSingleTimeCommands(commandBuffer);
}







void Render::copyBufferToImage(VkBuffer buffer, VkImage image, uint32 width, uint32 height) {
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
	region.imageExtent = {
		width,
		height,
		1
	};

	vkCmdCopyBufferToImage(
		commandBuffer,
		buffer,
		image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&region
	);

	endSingleTimeCommands(commandBuffer);
}






void Render::FPSCounter() {
	while (running) {
		Normal printf("%lf", FPS);
		sleep(1000);
	}
}


//TODO output stops when interacting with console
void Render::mainLoop() {
	running = true;
	std::thread FPSCounterThr(&Render::FPSCounter, this);
	FPSCounterThr.detach();
	stdTime start;
	bool fullScreen = false;

	int64 frameNum = 0;


	while (!glfwWindowShouldClose(window)) {
		//checkObjects();

		glfwPollEvents();
		drawFrame();
		frameNum++;

		static int lastState;
		static int lastw = 0;
		static int lasth = 0;


		//TODO fix full screen
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


	vkDeviceWaitIdle(graphics.LD);
	//running = false;
}



void Render::cleanupSwapChain() {
	vkDestroyImageView(graphics.LD, depthImageView, nullptr);
	vkDestroyImage(graphics.LD, depthImage, nullptr);
	vkFreeMemory(graphics.LD, depthImageMemory, nullptr);

	for (auto framebuffer : swapChainFramebuffers) vkDestroyFramebuffer(graphics.LD, framebuffer, nullptr);

	vkFreeCommandBuffers(graphics.LD, graphicsCommandPool, static_cast<uint32>(commandBuffers.size()), commandBuffers.data());

	vkDestroyPipeline(graphics.LD, graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(graphics.LD, pipelineLayout, nullptr);
	vkDestroyRenderPass(graphics.LD, renderPass, nullptr);

	for (auto imageView : swapChainImageViews) vkDestroyImageView(graphics.LD, imageView, nullptr);
	vkDestroySwapchainKHR(graphics.LD, swapChain, nullptr);


	vkDestroyDescriptorPool(graphics.LD, descriptorPool, nullptr);
}

void Render::cleanup() {
	cleanupSwapChain();

	vkDestroySampler(graphics.LD, textureSampler, nullptr);
	vkDestroyImageView(graphics.LD, textureImageView, nullptr);

	vkDestroyImage(graphics.LD, textureImage, nullptr);
	vkFreeMemory(graphics.LD, textureImageMemory, nullptr);

	vkDestroyDescriptorSetLayout(graphics.LD, descriptorSetLayout, nullptr);


	vkDestroyBuffer(graphics.LD, indexBuffer, nullptr);
	vkFreeMemory(graphics.LD, indexBufferMemory, nullptr);

	vkDestroyBuffer(graphics.LD, vertexBuffer, nullptr);
	vkFreeMemory(graphics.LD, vertexBufferMemory, nullptr);


	for (int64 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(graphics.LD, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(graphics.LD, imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(graphics.LD, inFlightFences[i], nullptr);
	}

	vkDestroyCommandPool(graphics.LD, graphicsCommandPool, nullptr);

	cleanupCompute();

	if (graphics.PD.properties.deviceID != compute.PD.properties.deviceID) vkDestroyDevice(graphics.LD, nullptr);
	vkDestroyDevice(compute.LD, nullptr);

	//TODO fix
	//for(auto device:secondary) vkDestroyDevice(device.LD, nullptr);

	if (enableValidationLayers) DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);

	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyInstance(instance, nullptr);
	glfwDestroyWindow(window);

	glfwTerminate();
}



#define populateDebugMessengerCreateInfo(createInfo)\
	createInfo = {};\
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;\
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;\
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;\
	createInfo.pfnUserCallback = debugCallback;






//Create the Vulkan instance, using validation layers when in debug mode
void Render::createInstance() {
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
	std::vector<const char*> extensions;
	uint32 glfwExtensionCount;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);		//Get extensions list and count
	for (uint32 i = 0; i < glfwExtensionCount; i++) { extensions.push_back(glfwExtensions[i]); }	//Save them in a vector
	if (enableValidationLayers) { extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME); }	//Add debug extension if in debug mode
	createInfo.enabledExtensionCount = static_cast<uint32>(extensions.size());					//Set extension count
	createInfo.ppEnabledExtensionNames = extensions.data();										//Set extensions


//Add validation layers if in debug mode
	#ifdef NDEBUG
	createInfo.enabledLayerCount = 0;
	createInfo.pNext = nullptr;
	#else
	//Search for validation layers
	uint32 layerCount = 0;
	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);					//Get layer count
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());	//Get layers
	for (const char* layerName : validationLayers) {							//For every layer,
		for (const auto& layerProperties : availableLayers) {						//Check if it's available
			if (strcmp(layerName, layerProperties.layerName) != 0) Quit("Validation layers not available. Cannot run in debug mode.");
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




void Render::setupDebugMessenger() {
	if (!enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	populateDebugMessengerCreateInfo(createInfo);

	Try(CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger)) {
		Quit("Failed to set up debug messenger");
	}
}

void Render::createSurface() {
	Try(glfwCreateWindowSurface(instance, window, nullptr, &surface)) {
		Quit("Failed to create window surface");
	}
}








VkImageView Render::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) {
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView imageView = VK_NULL_HANDLE;
	Try(vkCreateImageView(graphics.LD, &viewInfo, nullptr, &imageView)) Quit("Failed to create texture image view");

	return imageView;
}


void Render::createRenderPass() {
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

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;




	//Subpass
	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;


	//Dependencies for implicit convertion
	VkSubpassDependency dependencies[2];
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
	std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 2;
	renderPassInfo.pDependencies = dependencies;

	//Create render pass
	Try(vkCreateRenderPass(graphics.LD, &renderPassInfo, nullptr, &renderPass)) Quit("Failed to create render pass");
}



void Render::createDescriptorSetLayout() {
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


VkShaderModule Render::createShaderModule(uint32* code, uint32* size) {
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = *size;
	createInfo.pCode = code;

	VkShaderModule shaderModule;
	Try(vkCreateShaderModule(graphics.LD, &createInfo, nullptr, &shaderModule)) {
		Quit("Failed to create shader module");
	}
	return shaderModule;
}



#define StageInfo(_module, _stage) VkPipelineShaderStageCreateInfo {	\
	VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,				\
	nullptr,															\
	0,																	\
	_stage,																\
	_module,															\
	"main",																\
	nullptr																}


void Render::createGraphicsPipeline() {
	//PipelineCreateInfo struct to create the graphics pipeline
	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;




	//Create the structures that will fill the pipelineCreateInfo

	uint32 size = 0;
	VkShaderModule vertShaderModule = createShaderModule(readFile(&size, VERT_PATH), &size);			//Create vertex   shader module from shader's file (It needs to be in a variable to be destroyed)
	VkShaderModule fragShaderModule = createShaderModule(readFile(&size, FRAG_PATH), &size);			//Create fragment shader module from shader's file (It needs to be in a variable to be destroyed)

	VkPipelineShaderStageCreateInfo shaderStages[] = { StageInfo(vertShaderModule, VK_SHADER_STAGE_VERTEX_BIT), StageInfo(fragShaderModule, VK_SHADER_STAGE_FRAGMENT_BIT) };
	pipelineInfo.stageCount = 2;																	//Set the number of stages
	pipelineInfo.pStages = shaderStages;															//Set stages


	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescriptions = Vertex::getAttributeDescriptions();
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
	pipelineInfo.pVertexInputState = &vertexInputInfo;												//Save vertex input


	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;
	pipelineInfo.pInputAssemblyState = &inputAssembly;												//Save input assembly


	VkViewport viewport{};										//Area where to render
	viewport.x = 0.0f;											//Viewport start position
	viewport.y = 0.0f;											//Viewport start position
	viewport.width = (float)swapChainExtent.width;				//Viewport width (maximum)
	viewport.height = (float)swapChainExtent.height;			//Viewport height (maximum)
	viewport.minDepth = 0.0f;									//Viewport depth (Vulkan default)
	viewport.maxDepth = 1.0f;									//Viewport depth (Vulkan default)

	VkRect2D scissor{};											//Cut from viewport
	scissor.offset = { 0, 0 };									//Scrissor start position
	scissor.extent = swapChainExtent;							//Scrissor size. (don't cut the viewport)

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;
	pipelineInfo.pViewportState = &viewportState;													//Save viewport state


	VkPipelineRasterizationStateCreateInfo rasterizer{};							//Rasterizer to convert primitives into pixels
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;												//Clamp based on fragment depth (Required depth clamp feature)
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;										//Define how to draw triangles
	rasterizer.lineWidth = 1.0f;														//line T H I C C H N E S S
	rasterizer.cullMode = VK_CULL_MODE_NONE;											//Faces to cull
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;								//Define which side is visible
	rasterizer.depthBiasEnable = VK_FALSE;												//Depth biases to fix shadow glitchhes. //TODO set true if needed
	pipelineInfo.pRasterizationState = &rasterizer;									//Save rasterizer


	VkPipelineMultisampleStateCreateInfo multisampling{};							//Multisapling for antialiasing
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;										//Whether to use the sample shading (Requires minSampleShading definition)
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;							//Number of samples to use
	pipelineInfo.pMultisampleState = &multisampling;								//Save multisampling infos


	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlending{};							//Color blending infos
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;												//Whether to apply logical operations
	colorBlending.logicOp = VK_LOGIC_OP_COPY;											//Logical operations
	colorBlending.attachmentCount = 1;													//Number of color blend attachments
	colorBlending.pAttachments = &colorBlendAttachment;									//Color blend attachments      ↓ RGBA blend color
	colorBlending.blendConstants[0] = colorBlending.blendConstants[1] = colorBlending.blendConstants[2] = colorBlending.blendConstants[3] = 0.0f;
	pipelineInfo.pColorBlendState = &colorBlending;									//Save color blending infos


	VkPipelineDepthStencilStateCreateInfo depthStencil{};							//Depth stencil infos 
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_FALSE;											//Whether to use the depth test
	depthStencil.depthWriteEnable = VK_FALSE;											//Whether to use depth writes
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;									//Logical operation used to compare depths
	depthStencil.depthBoundsTestEnable = VK_FALSE;										//Whether to limit the depth to a certain range
	depthStencil.minDepthBounds = 0.0f;													//Maximum depth for depth bounds
	depthStencil.maxDepthBounds = 1.0f;													//Minimum depth for depth bounds
	depthStencil.stencilTestEnable = VK_FALSE;											//Whether to use the stencil test when testing depth
	depthStencil.front;																	//Parameter of the stencil test
	depthStencil.back;																	//Parameter of the stencil test
	pipelineInfo.pDepthStencilState = &depthStencil;								//Save depth stencil


	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};								//Pipeline's layout infos
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
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







void Render::createFramebuffers() {
	swapChainFramebuffers.resize(swapChainImageViews.size());

	for (int64 i = 0; i < swapChainImageViews.size(); i++) {
		std::array<VkImageView, 2> attachments = { swapChainImageViews[i],depthImageView };

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = static_cast<uint32>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;

		Try(vkCreateFramebuffer(graphics.LD, &framebufferInfo, nullptr, &swapChainFramebuffers[i])) Quit("Failed to create framebuffer");
	}
}



void Render::createGraphicsCommandPool() {
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = graphics.PD.indices.graphicsFamily;

	Try(vkCreateCommandPool(graphics.LD, &poolInfo, nullptr, &graphicsCommandPool)) Quit("Failed to create graphics command pool");
}

void Render::createVertexBuffer() {
	//Create staging buffer
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);


	//Map memory
	void* data;
	vkMapMemory(graphics.LD, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (int64)bufferSize);
	vkUnmapMemory(graphics.LD, stagingBufferMemory);

	//Create vertex buffer
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);
	copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

	//Destroy staging buffer
	vkDestroyBuffer(graphics.LD, stagingBuffer, nullptr);
	vkFreeMemory(graphics.LD, stagingBufferMemory, nullptr);
}



void Render::createIndexBuffer() {
	//Create staging buffer
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	//Map memory
	void* data;
	vkMapMemory(graphics.LD, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (int64)bufferSize);
	vkUnmapMemory(graphics.LD, stagingBufferMemory);

	//Create index buffer
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);
	copyBuffer(stagingBuffer, indexBuffer, bufferSize);

	//Destroy staging buffer
	vkDestroyBuffer(graphics.LD, stagingBuffer, nullptr);
	vkFreeMemory(graphics.LD, stagingBufferMemory, nullptr);
}



void Render::createDescriptorPool() {
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



void Render::createDescriptorSets() {
	std::vector<VkDescriptorSetLayout> layouts(swapChainImages.size(), descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32>(swapChainImages.size());
	allocInfo.pSetLayouts = layouts.data();

	descriptorSets.resize(swapChainImages.size());
	Try(vkAllocateDescriptorSets(graphics.LD, &allocInfo, descriptorSets.data())) Quit("Failed to allocate descriptor sets");

	for (int64 i = 0; i < descriptorSets.size(); i++) {
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



void Render::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	Try(vkCreateBuffer(graphics.LD, &bufferInfo, nullptr, &buffer)) Quit("Failed to create buffer");

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(graphics.LD, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	Try(vkAllocateMemory(graphics.LD, &allocInfo, nullptr, &bufferMemory)) Quit("Failed to allocate buffer memory");
	vkBindBufferMemory(graphics.LD, buffer, bufferMemory, 0);
}



//Creates and submits a command buffer to copy from srcBuffer to dstBuffer
void Render::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
	VkBufferCopy copyRegion{};
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();				//Start command buffer
	copyRegion.size = size;													//Set size of the copied region
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);	//Record the command to copy the buffer
	endSingleTimeCommands(commandBuffer);									//End command buffer
}


uint32 Render::findMemoryType(uint32 typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(graphics.PD.device, &memProperties);

	for (uint32 i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) return i;
	}
	Quit("Failed to find suitable memory type");
}


void Render::createDrawCommandBuffers() {
	commandBuffers.resize(swapChainFramebuffers.size());

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = graphicsCommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32)commandBuffers.size();

	Try(vkAllocateCommandBuffers(graphics.LD, &allocInfo, commandBuffers.data())) {
		Quit("Failed to allocate command buffers");
	}

	for (int64 i = 0; i < commandBuffers.size(); i++) {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = swapChainFramebuffers[i];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapChainExtent;


		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();


		Try(vkBeginCommandBuffer(commandBuffers[i], &beginInfo)) Quit("Failed to begin recording command buffer");

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);


		VkBuffer vertexBuffers[] = { vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT32); //LLID0
		vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);

		vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32>(indices.size()), 1, 0, 0, 0);


		vkCmdEndRenderPass(commandBuffers[i]);

		Try(vkEndCommandBuffer(commandBuffers[i])) Quit("Failed to record command buffer");
	}
}


void Render::createSyncObjects() {
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




void Render::drawFrame() {
	vkWaitForFences(graphics.LD, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
	uint32 imageIndex;

	VkResult result = vkAcquireNextImageKHR(graphics.LD, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) Quit("Failed to acquire swapchain image");

	if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
		vkWaitForFences(graphics.LD, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
	}
	imagesInFlight[imageIndex] = inFlightFences[currentFrame];


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


	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}
















VKAPI_ATTR VkBool32 VKAPI_CALL Render::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
}
