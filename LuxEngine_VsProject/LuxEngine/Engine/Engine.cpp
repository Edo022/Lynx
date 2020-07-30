

#include "LuxEngine/Engine/Engine.h"
#include "Input/Input.h"
#include "LuxEngine/System/System.h"








// Main -------------------------------------------------------------------------------------------------------------------------------------//








namespace lux::_engine {
	//Deprecated function
	//TODO remove
	//Compiles a shader from a file. Shader files must have the .comp extension
	static bool compileShader(const char* pShaderPath) {
		lux::String compileShaderCommand = lux::sys::dir::thisDir + "/../LuxEngine_VsProject/LuxEngine/Contents/shaders/glslc.exe " + pShaderPath + " -o " + pShaderPath + ".spv";  //lib
		//lux::String compileShaderCommand = lux::sys::dir::thisDir + "/LuxEngine/Contents/shaders/glslc.exe " + pShaderPath + " -o " + pShaderPath + ".spv"; //No .lib
		return system(compileShaderCommand.begin()) == 0;
	}
}

namespace lux{
	//This function returns a reference of the engine object
	Engine& getEngine( ){
		static Engine engine_;
		return engine_;
	}
}




//Initializes the engine object and all the Lux namespace
//Don't call this function. Use LuxInit( ) instead
void Engine::init(bool useVSync) {
	static bool once = true;
	if(once){
		once = false;
		lux::sys::__lp_init_system( );
		lux::thr::__lp_init_thread( );
		__lp_goniometric_functions_init( );

		std::thread renderThr(&Engine::run, this, useVSync, 45);
		renderThr.detach( );
		running = true;

		while(!initialized) sleep(10);
	}
}






void Engine::run(bool vUseVSync, float vFOV) {
	//Start init time counter and compile shaders
	//TODO create specific function to get some extensions or all the files
	lux::sys::dir::thisDir;
	LuxTime start = luxStartChrono( );
	shaderPath = lux::sys::dir::thisDir + "/../LuxEngine_VsProject/LuxEngine/Contents/shaders/";     //.lib
	//shaderPath = lux::sys::dir::thisDir + "/LuxEngine/Contents/shaders/";    //No .lib
	try {
		for(const auto& name : std::filesystem::recursive_directory_iterator(shaderPath.begin( ))) {
			lux::String luxStrPath = lux::String(name.path( ).u8string( ).c_str( )); lux::sys::dir::fixWindowsPath(luxStrPath);
			if(lux::sys::dir::getExtensionFromPath(luxStrPath) == "comp") {
				if(!lux::_engine::compileShader(luxStrPath.begin( ))) Exit("compilation error")
				else Normal printf("%s", luxStrPath.begin( ));
			}
		}
	}
	catch(const std::system_error& e) {
		std::cout << "system_error. code: " << e.code( ) << "\nmessage: " << e.what( ) << '\n';
	}

	//Init
	initWindow();
	Normal printf("Creating Instance...                     ");			createInstance();						SuccessNoNl printf("ok");
	graphicsInit(vUseVSync, vFOV);
	computeInit();

	//Loop
	Success printf("Initialization completed in %f s", luxStopChrono(start));
	Success printf("Starting Lux Engine\n");						mainLoop();									MainSeparator;

	//Exit
	Normal  printf("Cleaning memory");								graphicsCleanup(); computeCleanup();		NewLine;
	vkDestroyInstance(instance, nullptr);
	glfwDestroyWindow(window);
	glfwTerminate();
}




void Engine::mainLoop( ) {
	luxDebug(SetThreadDescription(GetCurrentThread( ), L"\tLuxEngine  |  User input"));
	std::thread FPSCounterThr(&Engine::runFPSCounterThr, this);		FPSCounterThr.detach( );
	std::thread renderThr(&Engine::runRenderThr, this);				renderThr.detach( );
	initialized = true;


	while(!glfwWindowShouldClose(window)) {
		glfwWaitEvents( );
	}
	running = false;
	vkDeviceWaitIdle(graphics.LD);
}




void Engine::runRenderThr() {
	luxDebug(SetThreadDescription(GetCurrentThread(), L"\tLuxEngine  |  Render"));
	while (running) {
		graphicsDrawFrame();
		//TODO it does nothing but it's probably important, somehow. dunno
		//vkDeviceWaitIdle(compute.LD);
		frames++;
	}
}



//TODO add FPS limit
void Engine::runFPSCounterThr() {
	luxDebug(SetThreadDescription(GetCurrentThread(), L"\tLuxEngine  |  FPS counter"));
	while (running) {
		static int delay = 1000;
		sleep(delay);
		FPS = frames * (1000 / delay);
		frames = 0;
		printf("FPS: %lf\n", FPS);
	}
}
















//Create the Vulkan instance, using validation layers when in debug mode
void Engine::createInstance( ) {
	//Application infos
	VkApplicationInfo appInfo{
		.sType{ VK_STRUCTURE_TYPE_APPLICATION_INFO },
		.pApplicationName{ "LuxEngine" },
		.applicationVersion{ VK_MAKE_VERSION(1, 0, 0) },
		.pEngineName{ "LuxEngine" },
		.engineVersion{ VK_MAKE_VERSION(1, 0, 0) },
		.apiVersion{ VK_API_VERSION_1_0 },
	};


	//Extensions
	lux::Map<const char*, uint32> extensions;
	uint32 glfwExtensionCount;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);	//Get extensions list and count
	for (uint32 i = 0; i < glfwExtensionCount; ++i) extensions.add(glfwExtensions[i]);		//Save them into an array
	luxDebug(extensions.add(VK_EXT_DEBUG_UTILS_EXTENSION_NAME));							//Add debug extension if in debug mode


	//Add validation layers if in debug mode
	#ifndef LUX_DEBUG
	createInfo.enabledLayerCount = 0;
	createInfo.pNext = nullptr;
	#else
	//Search for validation layers
	uint32 layerCount = 0;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);								//Get layer count
	lux::Array<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.begin());				//Get layers
	for (const char* layerName : validationLayers) {										//For every layer,
		for (const auto& layerProperties : availableLayers) {								//Check if it's available
			if (strcmp(layerName, layerProperties.layerName) == 0) break;
			else if (strcmp(layerName, availableLayers.end()->layerName) == 0) Exit("Validation layers not available. Cannot run in debug mode");
		}
	}

	//Set debugCreateInfo structure
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
	lux::_engine::populateDebugMessengerCreateInfo(debugCreateInfo);
	#endif




	//Create instance
	VkInstanceCreateInfo createInfo{
		.sType{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO },
		luxDebug(.pNext{ (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo }),
		.pApplicationInfo{ &appInfo },
		luxDebug(.enabledLayerCount{ validationLayers.size( ) }),
		luxDebug(.ppEnabledLayerNames{ validationLayers.begin( ) }),
		.enabledExtensionCount{ extensions.size( ) },
		.ppEnabledExtensionNames{ extensions.data(0) },
	};
	TryVk(vkCreateInstance(&createInfo, nullptr, &instance)) Exit("Failed to create instance");
}




void Engine::initWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(width, height, "Lux Engine", nullptr, nullptr);

	{ //Set icon
		unsigned char h[] = {
			255, 0, 0, 255,
			0, 0, 255, 255,
			255, 0, 0, 255,
			0, 0, 255, 255
		};
		GLFWimage icon{
			.width{ 2 },
			.height{ 2 },
			.pixels{ h },
		};
		glfwSetWindowIcon(window, 1, &icon);
	}


	{ //Set callbacks
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

		glfwSetCursorPosCallback(window, lux::input::mouseCursorPosCallback);
		glfwSetMouseButtonCallback(window, lux::input::mouseButtonCallback);
		glfwSetScrollCallback(window, lux::input::mouseAxisCallback);

		glfwSetKeyCallback(window, lux::input::keyCallback);
	}
}








// Shared and buffers -----------------------------------------------------------------------------------------------------------------------//








//Reads a shader from a file and saves it into a padded int32 array
//*   pLength: a pointer to an int32 where to store the padded code length
//*   pFilePath: a pointer to a char array containing the path to the compiled shader file
//*   returns a pointer to the array where the code is saved
uint32* Engine::cshaderReadFromFile(uint32* pLength, const char* pFilePath) {
	FILE* fp;
	fopen_s(&fp, pFilePath, "rb");									//Open the file
	if (fp == NULL) printf("Could not find or open file: %s\n", pFilePath);

	_fseeki64(fp, 0, SEEK_END);										//Go to the end of the file
	int32 filesize = scast<int32>(_ftelli64(fp));					//And get the file size
	_fseeki64(fp, 0, SEEK_SET);										//Go to the beginning of the file
	int32 paddedFileSize = int32(ceil(filesize / 4.0)) * 4;			//Calculate the padded size

	char* str = (char*)malloc(sizeof(char) * paddedFileSize);		//Allocate a buffer to save the file (Freed in createShaderModule function #LLID CSF0000)
	fread(str, filesize, sizeof(char), fp);							//Read the file
	fclose(fp);														//Close the file
	for (int32 i = filesize; i < paddedFileSize; ++i) str[i] = 0;	//Add padding

	*pLength = paddedFileSize;										//Set length
	return (uint32*)str;											//Return the buffer
}



//Creates a shader module from a compiled shader code and its size in bytes
//*   vDevice: the logical device to use to create the shader module
//*   pCode: a pointer to an int32 array containing the shader code
//*   pLength: a pointer to the code length
VkShaderModule Engine::cshaderCreateModule(const VkDevice vDevice, uint32* pCode, const uint32* pLength) {
	VkShaderModuleCreateInfo createInfo{};								//Create shader module infos
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;		//Set structure type
	createInfo.codeSize = *pLength;										//Set the size of the compiled shader code
	createInfo.pCode = pCode;											//Set the shader code

	VkShaderModule shaderModule;										//Create the shader module
	TryVk(vkCreateShaderModule(vDevice, &createInfo, nullptr, &shaderModule)) Exit("Failed to create shader module");
	free(pCode);														//#LLID CSF0000 Free memory
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
	TryVk (vkCreateBuffer(vDevice, &bufferInfo, nullptr, pBuffer)) Exit("Failed to create buffer");

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(vDevice, *pBuffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = graphicsFindMemoryType(memRequirements.memoryTypeBits, vProperties);

	//TODO check out of memory
	//TODO don't quit in VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS but return an error code
	switch (vkAllocateMemory(vDevice, &allocInfo, nullptr, pMemory)) {
		case VK_SUCCESS: break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY: {	//IF out of device memory, use the host memory
			VkMemoryAllocateInfo allocInfo2{};
			allocInfo2.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo2.allocationSize = memRequirements.size;
			allocInfo2.memoryTypeIndex = graphicsFindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
			switch (vkAllocateMemory(vDevice, &allocInfo2, nullptr, pMemory)) {
				case VK_SUCCESS: break;
				case VK_ERROR_OUT_OF_HOST_MEMORY: //TODO add case. same as next out of host memory
				default: Exit("Failed to allocate buffer memory");
			}
			break;
		}
		case VK_ERROR_OUT_OF_HOST_MEMORY:		//TODO If out of host memory
		case VK_ERROR_TOO_MANY_OBJECTS:	//TODO
		default: Exit("Failed to allocate buffer memory");
	}

	TryVk(vkBindBufferMemory(vDevice, *pBuffer, *pMemory, 0)) Exit("Failed to bind buffer");
}




//Creates and submits a command buffer to copy from vSrcBuffer to dstBuffer
//*   vSrcBuffer: the source buffer where to read the data
//*   vDstBuffer: the destination buffer where to copy the data
//*   vSize: the size in bytes of the data to copy
void Engine::copyBuffer(const VkBuffer vSrcBuffer, const VkBuffer vDstBuffer, const VkDeviceSize vSize) {
	VkBufferCopy copyRegion{};												//Create buffer copy object
	copyRegion.size = vSize;												//Set size of the copied region
	//TODO add offset and automatize cells
	//copyRegion.dstOffset
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();				//Start command buffer
	vkCmdCopyBuffer(commandBuffer, vSrcBuffer, vDstBuffer, 1, &copyRegion);	//Record the copy command
	endSingleTimeCommands(commandBuffer);									//End command buffer
}


















	//Dunno
	//inline void lux::_engine::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	//	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	//	if(func != nullptr) func(instance, debugMessenger, pAllocator);
	//}
