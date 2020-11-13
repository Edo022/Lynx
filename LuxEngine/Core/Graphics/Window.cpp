
#include "LuxEngine/Core/Core.hpp"
#include "LuxEngine/Core/Graphics/Window.hpp"
#include "LuxEngine/Core/Input/Input.hpp"
#include "LuxEngine/Types/Containers/LuxMap.hpp"
#include "LuxEngine/Memory/Gpu/VMemory.hpp"
#include "LuxEngine/Core/ConsoleOutput.hpp"
//#include "LuxEngine/Core/Core.h"
#include "LuxEngine/Core/LuxAutoInit.hpp"




// #pragma optimize("", off)
// PostInitializer(LUX_H_WINDOW);
// #pragma optimize("", on)
namespace lux::core::g::wnd{
	// GLFWwindow* window = window;
	// int32			NoInitVar(width), NoInitVar(height);
	// FenceDE			NoInitLux(windowResizeFence);
	// rem::Cell		NoInitVar(gpuCellWindowSize);
	// rem::Cell		NoInitVar(gpuCellWindowOutput);
	// rem::Cell		NoInitVar(gpuCellWindowOutput_i);
	// rem::Cell		NoInitVar(gpuCellWindowZBuffer);
	GLFWwindow* window = window;
	int32			width, height;
	FenceDE			windowResizeFence;
	rem::Cell		gpuCellWindowSize;
	rem::Cell		gpuCellWindowOutput;
	rem::Cell		gpuCellWindowOutput_i;
	rem::Cell		gpuCellWindowZBuffer;





	// void preInit( ){
	AutoInit(LUX_H_WINDOW){
		window = nullptr;
		width = 1920 * 2, height = 1080;
		// windowResizeFence.FenceDE::FenceDE( );
		windowResizeFence = FenceDE( );
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
		//DynArray<const char*> extensions;
		const char** extensions;
		//TODO manage nullptr in add functions
		uint32 glfwExtensionCount;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);	//Get extensions list and count

		extensions = (const char**)malloc(sizeof(const char**) * (glfwExtensionCount + 1));
		for(uint32 i = 0; i < glfwExtensionCount; ++i) extensions[i] = glfwExtensions[i];		//Save them into an array
		luxDebug(extensions[glfwExtensionCount] = (VK_EXT_DEBUG_UTILS_EXTENSION_NAME));							//Add debug extension if in debug mode




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
			//.enabledLayerCount{ validationLayers.count( ) },
			//.ppEnabledLayerNames{ validationLayers.begin( )->begin( ) },
			.enabledLayerCount{ validationLayersNum },
			.ppEnabledLayerNames{ validationLayers },
			#endif
			.enabledExtensionCount{ glfwExtensionCount + 1 },
			.ppEnabledExtensionNames{ extensions },
			//.enabledExtensionCount{ extensions.count( ) },
			//.ppEnabledExtensionNames{ extensions.begin( ) },
		};
		//Add validation layers if in debug mode
		#ifndef LUX_DEBUG
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
		#else
		//Search for validation layers
		uint32 layerCount = 0;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);					//Get layer count
		RTArray<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.begin( ));	//Get layers
		//for(const char* layerName : validationLayers) {									//For every layer,
		//	for(const auto& layerProperties : availableLayers) {							//Check if it's available
		//		if(strcmp(layerName, layerProperties.layerName) == 0) break;					//If not, exit
		//		else if(strcmp(layerName, availableLayers.end( )->layerName) == 0) printError("Validation layers not available. Cannot run in debug mode", true, -1);
		//	}
		//}
		//for(const String& layerName : validationLayers) {									//For every layer,
		//	for(const auto& layerProperties : availableLayers) {							//Check if it's available
		//		if(layerName == layerProperties.layerName) break;					//If not, exit
		//		else if(layerName == availableLayers.end( )->layerName) printError("Validation layers not available. Cannot run in debug mode", true, -1);
		//	}
		//}
		for(uint32 i = 0; i < validationLayersNum; i++) {									//For every layer,
			for(const auto& layerProperties : availableLayers) {							//Check if it's available
				if(validationLayers[i] == layerProperties.layerName) break;					//If not, exit
				else if(validationLayers[i] == availableLayers.end( )->layerName) luxPrintError("Validation layers not available. Cannot run in debug mode");
			}
		}
		#endif

		// TryVk(vkCreateInstance(&createInfo, nullptr, &core::instance)) printError("Failed to create instance", true, -1);
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
			glfwSetFramebufferSizeCallback(window, g::framebufferResizeCallback);

			glfwSetCursorPosCallback(window, input::mouseCursorPosCallback);
			glfwSetMouseButtonCallback(window, input::mouseButtonCallback);
			glfwSetScrollCallback(window, input::mouseAxisCallback);

			glfwSetKeyCallback(window, input::keyCallback);
		}
	}
}