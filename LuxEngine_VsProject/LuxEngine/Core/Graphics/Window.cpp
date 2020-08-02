
#include "LuxEngine/Core/Engine.h"
#include "LuxEngine/Core/Graphics/Window.h"




namespace lux::core::g{
	GLFWwindow* window;
	int32			width = 1920 * 2, height = 1080;
	FenceDE			windowResizeFence;
	LuxCell			gpuCellWindowSize;
	LuxCell			gpuCellWindowOutput;
	LuxCell			gpuCellWindowOutput_i;




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
		lux::Map<const char*, uint32> extensions;
		uint32 glfwExtensionCount;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);	//Get extensions list and count
		for(uint32 i = 0; i < glfwExtensionCount; ++i) extensions.add(glfwExtensions[i]);		//Save them into an array
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
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.begin( ));				//Get layers
		for(const char* layerName : lux::getEngine().validationLayers) {										//For every layer,
			for(const auto& layerProperties : availableLayers) {								//Check if it's available
				if(strcmp(layerName, layerProperties.layerName) == 0) break;
				else if(strcmp(layerName, availableLayers.end( )->layerName) == 0) Exit("Validation layers not available. Cannot run in debug mode");
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
			luxDebug(.enabledLayerCount{ lux::getEngine().validationLayers.size( ) }),
			luxDebug(.ppEnabledLayerNames{ lux::getEngine().validationLayers.begin( ) }),
			.enabledExtensionCount{ extensions.size( ) },
			.ppEnabledExtensionNames{ extensions.data(0) },
		};
		TryVk(vkCreateInstance(&createInfo, nullptr, &lux::getEngine().instance)) Exit("Failed to create instance");
	}




	void initWindow( ) {
		glfwInit( );
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		lux::core::g::window = glfwCreateWindow(lux::core::g::width, lux::core::g::height, "Lux Engine", nullptr, nullptr);

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
			glfwSetWindowIcon(lux::core::g::window, 1, &icon);
		}


		{ //Set callbacks
			glfwSetWindowUserPointer(lux::core::g::window, nullptr);
			glfwSetFramebufferSizeCallback(lux::core::g::window, lux::core::g::framebufferResizeCallback);

			glfwSetCursorPosCallback(lux::core::g::window, lux::input::mouseCursorPosCallback);
			glfwSetMouseButtonCallback(lux::core::g::window, lux::input::mouseButtonCallback);
			glfwSetScrollCallback(lux::core::g::window, lux::input::mouseAxisCallback);

			glfwSetKeyCallback(lux::core::g::window, lux::input::keyCallback);
		}
	}
}