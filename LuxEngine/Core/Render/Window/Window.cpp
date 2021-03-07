#include "LuxEngine/Core/Core.hpp"
#include "LuxEngine/Core/Render/Window/Window.hpp"
#include "LuxEngine/Core/Input/Input.hpp"
#include "LuxEngine/Types/Containers/RaArray.hpp"
#include "LuxEngine/Types/VPointer.hpp"
#include "LuxEngine/Debug/Debug.hpp"
#include "LuxEngine/Core/LuxAutoInit.hpp"








namespace lux{




	// luxAutoInit(LUX_H_WINDOW){
	// 	glfwInit();

	// 	//Extensions
	// 	uint32 glfwExtensionCount;
	// 	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);	//Get extensions list and count
	// 	//! ^ Freed by GLFW

	// 	const char** extensions = (const char**)malloc(sizeof(const char*) * (glfwExtensionCount _dbg(+ 1)));
	// 	for(uint32 i = 0; i < glfwExtensionCount; ++i) extensions[i] = glfwExtensions[i];		//Save them into an array
	// 	_dbg(extensions[glfwExtensionCount] = (VK_EXT_DEBUG_UTILS_EXTENSION_NAME));				//Add debug extension if in debug mode


	// 	//Create debugCreateInfo structure
	// 	_dbg(VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo);
	// 	_dbg(core::debug::populateDebugMessengerCreateInfo(debugCreateInfo));

	// 	VkApplicationInfo appInfo{
	// 		.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
	// 		.pApplicationName   = "LuxEngine",
	// 		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
	// 		.pEngineName        = "LuxEngine",
	// 		.engineVersion      = VK_MAKE_VERSION(1, 0, 0),
	// 		.apiVersion         = VK_API_VERSION_1_2
	// 	};

	// 	//Create instance
	// 	VkInstanceCreateInfo createInfo{
	// 		.sType                    = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
	// 		.pNext                    = _dbg((VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo)_rls(nullptr),
	// 		.pApplicationInfo         = &appInfo,
	// 		.enabledLayerCount        = _dbg(core::wnd::validationLayersNum) _rls(0),
	// 		_dbg(.ppEnabledLayerNames = core::wnd::validationLayers,)
	// 		.enabledExtensionCount    = glfwExtensionCount _dbg(+ 1),
	// 		.ppEnabledExtensionNames  = extensions
	// 	};
	// 	//Add validation layers if in debug mode
	// 	#ifdef LUX_DEBUG																	//Search for validation layers
	// 		uint32 layerCount = 0;
	// 		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);						//Get layer count
	// 		RtArray<VkLayerProperties> availableLayers(layerCount);
	// 		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.begin());		//Get layers
	// 		for(uint32 i = 0; i < core::wnd::validationLayersNum; i++) {							//For every layer,
	// 			for(const auto& layerProperties : availableLayers) {							//Check if it's available
	// 				if(core::wnd::validationLayers[i] == layerProperties.layerName) break;				//If not, exit
	// 				else if(core::wnd::validationLayers[i] == availableLayers.end()->layerName) dbg::printError("Validation layers not available. Cannot run in debug mode");
	// 			}
	// 		}
	// 	#endif

	// 	vkCreateInstance(&createInfo, nullptr, &core::dvc::instance);
	// 	free(extensions);
	// }

	Window window; //TODO REMOVE
	// alignCache GLFWwindow* window                = nullptr;
	// alignCache int32       width                 = 1920 * 2;
	// alignCache int32       height                = 1080;
	// std::mutex	windowResizeFence;
	// alignCache vram::ptr<int32, Ram,  Storage>   wSize_g     = nullptr;
	// alignCache vram::ptr<int32, VRam, Storage>   fOut_G   = nullptr;
	// alignCache vram::ptr<int32, VRam, Storage>   iOut_g = nullptr;
	// alignCache vram::ptr<int32, VRam, Storage>   zBuff_g  = nullptr;




	void Window::initWindow() {
		// glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		window = glfwCreateWindow(width, height, "Lux Engine", nullptr, nullptr);
		glfwCreateWindowSurface(core::dvc::instance, window, nullptr, &surface);

		{ //Set icon
			unsigned char h[] = {
				255, 0,   0,   255,
				0,   0,   255, 255,
				255, 0,   0,   255,
				0,   0,   255, 255
			};
			GLFWimage icon{
				.width = 2,
				.height = 2,
				.pixels = h
			};
			glfwSetWindowIcon(window, 1, &icon);
		}


		{ //Set callbacks
			glfwSetWindowUserPointer      (window, this);
			glfwSetFramebufferSizeCallback(window, core::render::framebufferResizeCallback);

			glfwSetCursorPosCallback      (window, input::mouseCursorPosCallback);
			glfwSetMouseButtonCallback    (window, input::mouseButtonCallback);
			glfwSetScrollCallback         (window, input::mouseAxisCallback);

			glfwSetKeyCallback            (window, input::keyCallback);
		}
	}
}