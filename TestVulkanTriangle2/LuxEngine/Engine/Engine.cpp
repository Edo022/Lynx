

#include "LuxEngine/Engine/Engine.h"
Engine engine;






// Main -------------------------------------------------------------------------------------------------------------------------------------//










void Engine::run(bool vUseVSync, float vFOV) {
	LuxTime start = luxGetTime();


	LuxString compileShaderCommand =
		luxThisDirectory + LuxString("/LuxEngine/Contents/shaders/glslc.exe ") +
		luxThisDirectory + LuxString("/LuxEngine/Contents/shaders/shader.comp -o ") +
		luxThisDirectory + "/LuxEngine/Contents/shaders/comp.spv";
	system(compileShaderCommand.begin());



	initWindow();
	Normal printf("Creating Instance...                     ");			createInstance();							SuccessNoNl printf("ok");
	runGraphics(vUseVSync, vFOV);
	initWindowBuffers();
	runCompute();

	glfwSetMouseButtonCallback(window, &__lp_mouseButtonCallback);
	glfwSetKeyCallback(window, &__lp_key_callback);

	Success printf("Initialization completed in %f s", luxTimeGetDuration(start));
	Success printf("Starting Lux Engine\n");						mainLoop();									MainSeparator;
	Normal  printf("Cleaning memory");								cleanupGraphics(); cleanupCompute();		NewLine;

	vkDestroyInstance(instance, nullptr);
	glfwDestroyWindow(window);
	glfwTerminate();
}




void Engine::mainLoop() {
	std::thread FPSCounterThr(&Engine::FPSCounter, this);
	std::thread renderThr(&Engine::render, this);
	FPSCounterThr.detach();
	renderThr.detach();
	__lp_initialized = true;

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();


		//TODO full screen
	}
	running = false;
	vkDeviceWaitIdle(graphics.LD);
}


void Engine::render() {
	while (running) {
		drawFrame();
		frame++;
		//std::this_thread::sleep_for(std::chrono::nanoseconds(1));
		//sleep(1);
	}
}



void Engine::FPSCounter() {
	while (running) {
		static int delay = 1000;
		sleep(delay);
		FPS = frame * (1000 / delay);
		frame = 0;
		printf("FPS: %lf\n", FPS);
	}
}




//Create the Vulkan instance, using validation layers when in debug mode
void Engine::createInstance() {
	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

	//Application infos
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "LuxEngine";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "LuxEngine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;
	createInfo.pApplicationInfo = &appInfo;


	//Extensions
	LuxMap<const char*> extensions;
	uint32 glfwExtensionCount;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);	//Get extensions list and count
	for (uint32 i = 0; i < glfwExtensionCount; ++i) extensions.add(glfwExtensions[i]);		//Save them into an array
	luxDebug(extensions.add(VK_EXT_DEBUG_UTILS_EXTENSION_NAME));			//Add debug extension if in debug mode
	createInfo.enabledExtensionCount = sc<uint32>(extensions.size());						//Set extension count
	createInfo.ppEnabledExtensionNames = extensions.data(0);								//Set extensions


	//Add validation layers if in debug mode
	#ifndef LUX_DEBUG
	createInfo.enabledLayerCount = 0;
	createInfo.pNext = nullptr;
	#else
	//Search for validation layers
	uint32 layerCount = 0;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);								//Get layer count
	LuxArray<VkLayerProperties> availableLayers((uint64)layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());				//Get layers
	for (const char* layerName : validationLayers) {										//For every layer,
		for (const auto& layerProperties : availableLayers) {									//Check if it's available
			if (strcmp(layerName, layerProperties.layerName) == 0) break;
			else if (strcmp(layerName, availableLayers.end()->layerName) == 0) Exit("Validation layers not available. Cannot run in debug mode");
		}
	}

	//Set debugCreateInfo structure
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
	createInfo.enabledLayerCount = sc<uint32>(validationLayers.size());
	createInfo.ppEnabledLayerNames = validationLayers.data();
	populateDebugMessengerCreateInfo(debugCreateInfo);
	createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	#endif


	//Create instance
	TryVk(vkCreateInstance(&createInfo, nullptr, &instance)) Exit("Failed to create instance");
}



void Engine::initWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	window = glfwCreateWindow(width, height, "Lux Engine", nullptr, nullptr);
	

	unsigned char h[] = {
		255, 0, 0, 255,
		0, 0, 255, 255,
		255, 0, 0, 255,
		0, 0, 255, 255
	};

	GLFWimage icon;
	icon.width = 2;
	icon.height = 2;
	icon.pixels = h;

	glfwSetWindowIcon(window, 1, &icon);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}


void Engine::initWindowBuffers() {
	__windowOutput = createGpuCell(width * height * 4/*A8-R8-G8-B8*/, false);
}








// Shared functions -------------------------------------------------------------------------------------------------------------------------//








//Reads a shader from a file and saves it into a padded int32 array
//*   pLength: a pointer to an int32 where to store the padded code length
//*   pFilePath: a pointer to a char array containing the path to the compiled shader file
//*   returns a pointer to the array where the code is saved
uint32* Engine::readShaderFromFile(uint32* pLength, const char* pFilePath) {
	FILE* fp = fopen(pFilePath, "rb");
	if (fp == NULL) printf("Could not find or open file: %s\n", pFilePath);

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
	for (int32 i = filesize; i < paddedFileSize; ++i) str[i] = 0;

	*pLength = paddedFileSize;
	return (uint32*)str;
}




//Creates a shader module from a compiled shader code and its size in bytes
//*   vDevice: the logical device to use to create the shader module
//*   pCode: a pointer to an int32 array containing the shader code
//*   pLength: a pointer to the code length
VkShaderModule Engine::createShaderModule(const VkDevice vDevice, uint32* pCode, const uint32* pLength) {
	VkShaderModuleCreateInfo createInfo{};								//Create shader module infos
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;		//Set structure type
	createInfo.codeSize = *pLength;										//Set the size of the compiled shader code
	createInfo.pCode = pCode;											//Set the shader code

	VkShaderModule shaderModule;										//Create the shader module
	TryVk(vkCreateShaderModule(vDevice, &createInfo, nullptr, &shaderModule)) Exit("Failed to create shader module");
	return shaderModule;												//Return the created shader module
}




//Creates and allocates a buffer in the memory of a device
//*   vDevice: the logical device where to create the buffer
//*   vSize: the size of the buffer in bytes
//*   vUsage: flags defining the usage of the buffer (VK_BUFFER_USAGE...)
//*   vProperties: flags defining the properties of the memory (VK_MEMORY_PROPERTY_...)
//*   pBuffer: the buffer object to allocate
//*   pMemory: the memory of the buffer
void Engine::createBuffer(const VkDevice vDevice, const VkDeviceSize vSize, const VkBufferUsageFlags vUsage, const VkMemoryPropertyFlags vProperties, VkBuffer* pBuffer, VkDeviceMemory* pMemory) {
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = vSize;
	bufferInfo.usage = vUsage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	TryVk(vkCreateBuffer(vDevice, &bufferInfo, nullptr, pBuffer)) Exit("Failed to create buffer");

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(vDevice, *pBuffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, vProperties);

	//TODO check out of memory
	switch (vkAllocateMemory(vDevice, &allocInfo, nullptr, pMemory)) {
		case VK_SUCCESS: break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:
		case VK_ERROR_TOO_MANY_OBJECTS:
		default: Exit("Failed to allocate buffer memory");
	}
	
	TryVk(vkBindBufferMemory(vDevice, *pBuffer, *pMemory, 0)) Exit("Failed to bind buffer");
}