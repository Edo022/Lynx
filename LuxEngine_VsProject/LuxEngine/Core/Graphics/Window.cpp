
#include "LuxEngine/Core/Core.h"
#include "LuxEngine/Core/Graphics/Window.h"
#include "LuxEngine/Core/Input/Input.h"
#include "LuxEngine/Types/Containers/LuxMap.h"
#include "LuxEngine/Memory/Gpu/VMemory.h"
#include "LuxEngine/Core/Core.h"




namespace lux::core::g::wnd{
	PostInitializer(LUX_H_WINDOW);
	GLFWwindow* window = window;
	int32			width = width, height = height;
	FenceDE			windowResizeFence(DontInitialize( ));
	rem::Cell		gpuCellWindowSize = gpuCellWindowSize;
	rem::Cell		gpuCellWindowOutput = gpuCellWindowOutput;
	rem::Cell		gpuCellWindowOutput_i = gpuCellWindowOutput_i;
	rem::Cell		gpuCellWindowZBuffer = gpuCellWindowZBuffer;





	void preInit( ){
		window = nullptr;
		width = 1920 * 2, height = 1080;
		windowResizeFence.FenceDE::FenceDE( );
		gpuCellWindowSize = nullptr;
		gpuCellWindowOutput = nullptr;
		gpuCellWindowOutput_i = nullptr;
		gpuCellWindowZBuffer = nullptr;
	}




	//Create the Vulkan instance, using validation layers when in debug mode
	void createInstance( ) {
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
		DynArray<const char*> extensions;
		uint32 glfwExtensionCount;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);	//Get extensions list and count
		for(uint32 i = 0; i < glfwExtensionCount; ++i) extensions.add(glfwExtensions[i]);		//Save them into an array
		luxDebug(extensions.add(VK_EXT_DEBUG_UTILS_EXTENSION_NAME));							//Add debug extension if in debug mode




		//Create debugCreateInfo structure
		luxDebug(VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo);
		luxDebug(debug::populateDebugMessengerCreateInfo(debugCreateInfo));

		//Create instance
		VkInstanceCreateInfo createInfo{
			.sType{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO },
			#ifdef LUX_DEBUG
			.pNext{ (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo },
			#endif
			.pApplicationInfo{ &appInfo },
			#ifdef LUX_DEBUG
			.enabledLayerCount{ validationLayers.size( ) },
			.ppEnabledLayerNames{ validationLayers.begin( ) },
			#endif
			.enabledExtensionCount{ extensions.size( ) },
			.ppEnabledExtensionNames{ extensions.begin( ) },
		};
		//Add validation layers if in debug mode
		#ifndef LUX_DEBUG
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
		#else
		//Search for validation layers
		uint32 layerCount = 0;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);					//Get layer count
		Array<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.begin( ));	//Get layers
		for(const char* layerName : validationLayers) {									//For every layer,
			for(const auto& layerProperties : availableLayers) {							//Check if it's available
				if(strcmp(layerName, layerProperties.layerName) == 0) break;					//If not, exit
				else if(strcmp(layerName, availableLayers.end( )->layerName) == 0) printError("Validation layers not available. Cannot run in debug mode", true, -1);
			}
		}
		#endif

		TryVk(vkCreateInstance(&createInfo, nullptr, &instance)) printError("Failed to create instance", true, -1);
	}




	void initWindow( ) {
		glfwInit( );
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
			glfwSetWindowUserPointer(window, nullptr);
			glfwSetFramebufferSizeCallback(window, g::framebufferResizeCallback);

			glfwSetCursorPosCallback(window, input::mouseCursorPosCallback);
			glfwSetMouseButtonCallback(window, input::mouseButtonCallback);
			glfwSetScrollCallback(window, input::mouseAxisCallback);

			glfwSetKeyCallback(window, input::keyCallback);
		}
	}
}