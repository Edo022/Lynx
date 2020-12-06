#include "LuxEngine/Core/Core.hpp"
#include "LuxEngine/Core/Render/Window.hpp"
#include "LuxEngine/Core/Input/Input.hpp"
#include "LuxEngine/Types/Containers/RaArray.hpp"
#include "LuxEngine/Memory/Gpu/VMemory.hpp"
#include "LuxEngine/Core/ConsoleOutput.hpp"
#include "LuxEngine/Core/LuxAutoInit.hpp"








namespace lux::core::render::wnd{
	GLFWwindow* window 					= nullptr;
	int32		width = 1920 * 2, height = 1080; //TODO
	FenceDE		windowResizeFence;
	rem::Cell	gpuCellWindowSize 		= nullptr;
	rem::Cell	gpuCellWindowOutput 	= nullptr;
	rem::Cell	gpuCellWindowOutput_i 	= nullptr;
	rem::Cell	gpuCellWindowZBuffer	= nullptr;




	//Create the Vulkan instance, using validation layers when in debug mode
	void createInstance( ) {
		VkApplicationInfo appInfo{
			.sType				{ VK_STRUCTURE_TYPE_APPLICATION_INFO	},
			.pApplicationName	{ "LuxEngine" 							},
			.applicationVersion	{ VK_MAKE_VERSION(1, 0, 0) 				},
			.pEngineName		{ "LuxEngine" 							},
			.engineVersion		{ VK_MAKE_VERSION(1, 0, 0) 				},
			.apiVersion			{ VK_API_VERSION_1_0 					},
		};


		//Extensions
		const char** extensions;
		//TODO manage nullptr in add functions
		uint32 glfwExtensionCount;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);	//Get extensions list and count

		extensions = (const char**)malloc(sizeof(const char**) * (glfwExtensionCount luxDebug(+ 1)));
		for(uint32 i = 0; i < glfwExtensionCount; ++i) extensions[i] = glfwExtensions[i];		//Save them into an array
		luxDebug(extensions[glfwExtensionCount] = (VK_EXT_DEBUG_UTILS_EXTENSION_NAME));			//Add debug extension if in debug mode



		//Create debugCreateInfo structure
		luxDebug(VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo);
		luxDebug(debug::populateDebugMessengerCreateInfo(debugCreateInfo));

		//Create instance
		VkInstanceCreateInfo createInfo{
			.sType							{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO 				},
			.pNext							{ luxDebug((VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo) luxRelease(nullptr) },
			.pApplicationInfo				{ &appInfo 												},
			.enabledLayerCount				{ luxDebug(validationLayersNum) luxRelease(0)			},
			luxDebug(.ppEnabledLayerNames	{ validationLayers 										},)
			.enabledExtensionCount			{ glfwExtensionCount luxDebug(+ 1) 						},
			.ppEnabledExtensionNames		{ extensions 											}
		};
		//Add validation layers if in debug mode
		luxDebug(																		//Search for validation layers
			uint32 layerCount = 0;
			vkEnumerateInstanceLayerProperties(&layerCount, nullptr);						//Get layer count
			RtArray<VkLayerProperties> availableLayers(layerCount);
			vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.begin( ));		//Get layers
			for(uint32 i = 0; i < validationLayersNum; i++) {									//For every layer,
				for(const auto& layerProperties : availableLayers) {							//Check if it's available
					if(validationLayers[i] == layerProperties.layerName) break;					//If not, exit
					else if(validationLayers[i] == availableLayers.end( )->layerName) luxPrintError("Validation layers not available. Cannot run in debug mode");
				}
			}
		);

		int hh = vkCreateInstance(&createInfo, nullptr, &core::instance);
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
			glfwSetFramebufferSizeCallback(window, render::framebufferResizeCallback);

			glfwSetCursorPosCallback(window, input::mouseCursorPosCallback);
			glfwSetMouseButtonCallback(window, input::mouseButtonCallback);
			glfwSetScrollCallback(window, input::mouseAxisCallback);

			glfwSetKeyCallback(window, input::keyCallback);
		}
	}
}