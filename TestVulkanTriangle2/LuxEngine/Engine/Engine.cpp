

#include "LuxEngine/Engine/Engine.h"








// Main -------------------------------------------------------------------------------------------------------------------------------------//










void Engine::run(bool _useVSync, float _FOV) {
	initWindow();
	Normal printf("    Creating Instance...                 ");		createInstance();					SuccessNoNl printf("ok");
	runGraphics(_useVSync, _FOV);
	RunCompute();

	glfwSetMouseButtonCallback(window, &__lp_mouseButtonCallback);
	glfwSetKeyCallback(window, &__lp_key_callback);

	Success printf("Starting Mandragora Engine\n");					mainLoop();			MainSeparator;
	Normal  printf("Cleaning memory");		 cleanupGraphics(); cleanupCompute();			NewLine;

	vkDestroyInstance(instance, nullptr);
	glfwDestroyWindow(window);
	glfwTerminate();
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

		//FPS counter
		stdDuration elapsed_seconds = now - start;
		FPS = 1 / elapsed_seconds.count();
		start = now;
	}
	running = false;

	vkDeviceWaitIdle(graphics.LD);
}


void Engine::FPSCounter() {
	while (running) {
		Normal printf("%lf", FPS);
		sleep(1000);
	}
}




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
	LuxDynArray<const char*> extensions;
	uint32 glfwExtensionCount;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);	//Get extensions list and count
	for (uint32 i = 0; i < glfwExtensionCount; i++) extensions.add(glfwExtensions[i]);		//Save them into an array
	if (enableValidationLayers) extensions.add(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);			//Add debug extension if in debug mode
	createInfo.enabledExtensionCount = sc<uint32>(extensions.size());						//Set extension count
	createInfo.ppEnabledExtensionNames = extensions.data(0);								//Set extensions


	//Add validation layers if in debug mode
	#ifdef NDEBUG
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
			else if (strcmp(layerName, availableLayers.end()->layerName) == 0) Quit("Validation layers not available. Cannot run in debug mode.");
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
	Try(vkCreateInstance(&createInfo, nullptr, &instance)) Quit("Failed to create instance");
}




void Engine::initWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	window = glfwCreateWindow(WIDTH, HEIGHT, "Mandragora Engine", nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}








// Shared functions -------------------------------------------------------------------------------------------------------------------------//








//Reads a shader from a file and saves it into a padded int32 array
//*   length: a pointer to an int32 where to store the padded code length
//*   filename: a pointer to a char array containing the path to the compiled shader file
//*   returns a pointer to the array where the code is saved
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




//Creates a shadere module from a compiled shader code and its size in bytes
//*   device: the logical device to use to create the shader module
//*   code: a pointer to an int32 array containing the shader code
//*   length: a pointer to the code length
VkShaderModule Engine::createShaderModule(VkDevice device, uint32* code, uint32* length) {
	VkShaderModuleCreateInfo createInfo{};								//Create shader module infos
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;		//Set structure type
	createInfo.codeSize = *length;										//Set the size of the compiled shader code
	createInfo.pCode = code;											//Set the shader code

	VkShaderModule shaderModule;										//Create the shader module
	Try(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule)) Quit("Failed to create shader module");
	return shaderModule;												//Return the created shader module
}




//Creates and allocates a buffer in the memory of a device
//*   device: the logical device where to create the buffer
//*   size: the size of the buffer in bytes
//*   usage: flags defining the usage of the buffer (VK_BUFFER_USAGE...)
//*   properties: flags defining the properties of the memory (VK_MEMORY_PROPERTY_...)
//*   buffer: the buffer object to allocate
//*   memory: the memory of the buffer
void Engine::createBuffer(VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& memory) {
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

	Try(vkAllocateMemory(device, &allocInfo, nullptr, &memory)) Quit("Failed to allocate buffer memory");
	Try(vkBindBufferMemory(device, buffer, memory, 0)) Quit("Failed to bind buffer");
}