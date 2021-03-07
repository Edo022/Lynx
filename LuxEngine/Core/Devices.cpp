
//TODO USE MOVE SEMANTICS
#include "LuxEngine/Core/Core.hpp"
#include "LuxEngine/Core/Devices.hpp"
#include "LuxEngine/Core/Render/Window/Swapchain.hpp"
#include "LuxEngine/Types/Containers/RaArray.hpp"
#include "LuxEngine/Core/LuxAutoInit.hpp"
#include "LuxEngine/System/SystemInfo.hpp"

#include <set>
#include <vector>

//Compares 2 _VkPhysicalDevice objects
#define sameDevice(a,b) ((a).properties.deviceID == (b).properties.deviceID)






namespace lux::core::dvc{
	alignCache graphicsDevice         graphics;
	alignCache computeDevice          compute;
	alignCache RtArray<computeDevice> secondary;


	alignCache VkInstance   instance;
	alignCache GLFWwindow*  dummyWindow;
	alignCache VkSurfaceKHR dummySurface;

	alignCache uint32       requiredDeviceExtensionsNum = 1;
	alignCache const char** requiredDeviceExtensions    = new const char*{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	#ifdef LUX_DEBUG
		alignCache VkDebugUtilsMessengerEXT debugMessenger;
		alignCache uint32       validationLayersNum = 1;
		alignCache const char** validationLayers    = new const char*{ "VK_LAYER_KHRONOS_validation" };
	#endif




	luxAutoInit(LUX_H_DEVICES){
		if(!glfwInit()) exit(-1);																//Initialize GLFW
		//TODO ADD ERROR MESSAGE

		//Extensions
		uint32 glfwExtensionCount;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);	//Get extensions list and count
		//! ^ Freed by GLFW
		const char** extensions = (const char**)malloc(sizeof(const char*) * (glfwExtensionCount _dbg(+ 1)));
		for(uint32 i = 0; i < glfwExtensionCount; ++i) extensions[i] = glfwExtensions[i];		//Save them into an array
		_dbg(extensions[glfwExtensionCount] = (VK_EXT_DEBUG_UTILS_EXTENSION_NAME));				//Add debug extension if in debug mode


		//Create debugCreateInfo structure
		_dbg(VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo);
		_dbg(core::debug::populateDebugMessengerCreateInfo(debugCreateInfo));

		VkApplicationInfo appInfo{
			.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pApplicationName   = "LuxEngine",
			.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
			.pEngineName        = "LuxEngine",
			.engineVersion      = VK_MAKE_VERSION(1, 0, 0),
			.apiVersion         = VK_API_VERSION_1_2
		};

		//Create instance
		VkInstanceCreateInfo createInfo{
			.sType                    = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pNext                    = _dbg((VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo)_rls(nullptr),
			.pApplicationInfo         = &appInfo,
			.enabledLayerCount        = _dbg(validationLayersNum) _rls(0),
			_dbg(.ppEnabledLayerNames = validationLayers,)
			.enabledExtensionCount    = glfwExtensionCount _dbg(+ 1),
			.ppEnabledExtensionNames  = extensions
		};
		//Add validation layers if in debug mode
		#ifdef LUX_DEBUG																	//Search for validation layers
			uint32 layerCount = 0;
			vkEnumerateInstanceLayerProperties(&layerCount, nullptr);						//Get layer count
			RtArray<VkLayerProperties> availableLayers(layerCount);
			vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.begin());		//Get layers
			for(uint32 i = 0; i < validationLayersNum; i++) {							//For every layer,
				for(const auto& layerProperties : availableLayers) {							//Check if it's available
					if(validationLayers[i] == layerProperties.layerName) break;				//If not, exit
					else if(validationLayers[i] == availableLayers.end()->layerName) dbg::printError("Validation layers not available. Cannot run in debug mode");
				}
			}
		#endif

		vkCreateInstance(&createInfo, nullptr, &core::dvc::instance);
		free(extensions);



		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //FIXME
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);	//FIXME
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		dummyWindow = glfwCreateWindow(1, 1, "DummyWindow", nullptr, nullptr);	//Initialize dummy window
		glfwCreateWindowSurface(instance, dummyWindow, nullptr, &dummySurface); //Initialize dummy surface


		// GLFWwindow* dummyWindow = glfwCreateWindow(0, 0, "DummyWindow", nullptr, nullptr);
		// VkSurfaceKHR dummySurface; glfwCreateWindowSurface(instance, dummyWindow, nullptr, &dummySurface);
		dbg::checkVk(glfwCreateWindowSurface(instance, dummyWindow, nullptr, &dummySurface), "Failed to create window surface");
		getPhysical();
	}





	//Rates a physical device based on its properties and features
	//*   pDevice: a pointer to the device structure where its infos are stored
	//*   Returns the rating of the physical device
	int32 rate(const _VkPhysicalDevice* pDevice) {
		uint32 score = 0;																				//Device performance evalutation
		if(pDevice->properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) score += 1000000;	//Discrete GPUs have performance advantage
		//TODO dont create shaders larger than the device limit
		//pDevice->properties.limits.maxComputeSharedMemorySize;
		score += pDevice->properties.limits.maxImageDimension2D;										//Maximum possible count of textures affects graphics quality
		if(pDevice->features.geometryShader) score += 1;												//Geometry shaders needed
		return score;
	}




	/**
	 * @brief Checks if a device has the required extensions and properties to run vulkan
	 * @param vDevice The physical device to check
	 * @param pErrorText A pointer to a lux::String where to store the error in case the device is not suitable
	 * @return return True if the device is suitable, false if not
	 */
	bool isSuitable(const VkPhysicalDevice vDevice, String& pErrorText) {
		//Check extensions
		if(!checkExtensions(vDevice)) {
			pErrorText = "Missing required extensions";
			return false;
		}

		//Check swapchain support
		else {
			// wnd::SwapChainSupportDetails swapChainSupport = wnd::getSwapchainSupportDetails(vDevice, surface); //FIXME DONT USE GLOBAL SURFACE OR CREATE DUMMY WINDOW
			wnd::SwapChainSupportDetails swapChainSupport = wnd::getSwapchainSupportDetails(vDevice, dummySurface);
			if(!swapChainSupport.formats.count() || !swapChainSupport.presentModes.count()) {
				pErrorText = "Unsupported swapchain";
				return false;
			}
		}
		return true;
	}




	/**
	 * @brief Returns true if the device supports the extensions, false if not
	 */
	bool checkExtensions(const VkPhysicalDevice vDevice) {
		uint32 extensionCount;
		vkEnumerateDeviceExtensionProperties(vDevice, nullptr, &extensionCount, nullptr);						//Get extension count
		RtArray<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(vDevice, nullptr, &extensionCount, availableExtensions.begin());	//Get extensions

		//TODO dont use std
		std::set<const char*> requiredExtensions(requiredDeviceExtensions, requiredDeviceExtensions);
		for(const auto& extension : availableExtensions) requiredExtensions.erase(extension.extensionName);		//Search for required extensions
		return requiredExtensions.empty();
	}




	/**
	 * @brief Finds and returns the queue families of a physical device
	 */
	QueueFamilyIndices getQueueFamilies(const VkPhysicalDevice vDevice) {
		uint32 queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(vDevice, &queueFamilyCount, nullptr);						//Enumerate queue families
		RtArray<VkQueueFamilyProperties> queueFamilies;
		queueFamilies.resize(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(vDevice, &queueFamilyCount, queueFamilies.begin());		//Save queue families

		//Set families
		QueueFamilyIndices indices;
		for(uint32 i = 0; i < queueFamilies.count(); ++i) {												//For every queue family
			if(queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) indices.graphicsFamily = i;					//Set graphics family
			if(queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT)  indices.computeFamilies.add(i);				//Add compute families
			VkBool32 hasPresentSupport = false;
			// vkGetPhysicalDeviceSurfaceSupportKHR(vDevice, i, surface, &hasPresentSupport);						//Set present family
			vkGetPhysicalDeviceSurfaceSupportKHR(vDevice, i, dummySurface, &hasPresentSupport);						//Set present family
			if(hasPresentSupport) indices.presentFamily = i;
		}
		return indices;
	}








	//-----------------------------------------------------------------------------------------------------------------------------//








	//TODO add multiple gpu support
	/**
	 * @brief Finds all the suitable physical devices, choosing the main and secondary devices according to their capabilities
	 */
	void getPhysical() {
		uint32 deviceCount = 0;
		RtArray<String> discardedPhysicalDevices;
		RtArray<_VkPhysicalDevice*> physicalDevices;


		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);							//Get physical device count
		//FIXME add runtime support
		if(deviceCount == 0) dbg::printError("Failed to find GPUs with Vulkan support");		//Check if there is at least one deice that supports vulkan

		//Get physical devices
		RtArray<VkPhysicalDevice> physDevices(deviceCount);									//Create physical device array
		//FIXME add runtime support
		vkEnumeratePhysicalDevices(instance, &deviceCount, physDevices.begin());				//Get physical devices

		for(uint32 i = 0; i < physDevices.count(); ++i) {																//For every physical device, create and save a _VkPhysicalDevice stucture
			VkPhysicalDeviceProperties properties;	vkGetPhysicalDeviceProperties(physDevices[i], &properties);
			VkPhysicalDeviceFeatures features;		vkGetPhysicalDeviceFeatures(physDevices[i], &features);
			String errorText;
			if(isSuitable(physDevices[i], errorText)) {																//If it's suitable
				physicalDevices.add(new _VkPhysicalDevice(physDevices[i], properties, features, *new QueueFamilyIndices));	//Add it to the physical devices vector
			}
			else {																												//If not
				discardedPhysicalDevices.add(properties.deviceName);															//Add it to the discarded devices vector
				discardedPhysicalDevices.add(errorText);																		//And save the reason of its unsuitability
			}
		}


		//If there are discarded devices, print their names
		if(discardedPhysicalDevices.count() > 0) {
			Failure printf("    Discarded devices:");
			for(uint32 i = 0; i < discardedPhysicalDevices.count(); i += 2) {
				Failure printf("        %s\t|  %s", (char*)discardedPhysicalDevices[i].begin(), (char*)discardedPhysicalDevices[(int64)i + 1].begin());
			}
		}

		//TODO different score for graphics and compute
		#define physDev (*physicalDevices[i])
		if(physicalDevices.count() > 0) {								//If there are suitable devices
			graphics.PD = compute.PD = *physicalDevices[0];				//set graphics device at default value
			for(uint32 i = 0; i < physicalDevices.count(); ++i) {				//For every physical device
				physDev.indices = getQueueFamilies(physDev.device);				//Get its queue families
				physDev.score = rate(&physDev);									//And its score. Then check if it has the necessary queues and set it as the main graphics and or compute physical device
				if(physDev.score > graphics.PD.score || physDev.indices.graphicsFamily != (uint32)-1)        graphics.PD = physDev;
				if(physDev.score > compute .PD.score || physDev.indices.computeFamilies.count() > 0) compute.PD = physDev;
			}
			for(uint32 i = 0; i < physicalDevices.count(); ++i) {				//For every physical device that isn't the main graphics or compute device
				if(!sameDevice(physDev, graphics.PD) && !sameDevice(physDev, compute.PD)) {
					secondary.resize(secondary.count() + 1);
					secondary[secondary.count() - 1].PD = physDev;				//Add it to the secondary devices vector (it'll be used as a compute device with less priority. T.T poor gpu)
				}
			}

			//Print the devices names, IDs, scores and tasks
			Success printf("    Found %d suitable device%s:", physicalDevices.count(), (physicalDevices.count() == 1) ? "" : "s");
			for(uint32 i = 0; i < physicalDevices.count(); ++i) {
				if(sameDevice(physDev, graphics.PD) || sameDevice(physDev, compute.PD)) Main else Normal;
				printf("        %s  |  ID: %d  |  %d", physDev.properties.deviceName, physDev.properties.deviceID, physDev.score);
				if(sameDevice(physDev, graphics.PD)) printf("  |  Main graphics");
				if(sameDevice(physDev, compute.PD))  printf("  |  Main compute");
			}
		}
		//FIXME add runtime support
		else dbg::printError("Failed to find a suitable GPU");
		#undef physDev




		//Create a logical device for graphics, one for computation and one for every secondary device
		createLogical(&graphics.PD, &graphics.LD, nullptr);
		createLogical(&compute.PD,  &compute.LD, &compute.computeQueues);
		for(uint32 i = 0; i < secondary.count(); ++i) {
			createLogical(&secondary[i].PD, &secondary[i].LD, &secondary[i].computeQueues);
		}

		//Output created logical devices and queues
		Success printf("    Created %d logical devices:", 2 + secondary.count());
		Main	printf("        Main graphics  |  graphics queues: 1  |  present queues:  1");
		Main	printf("        Main compute   |  compute queues:  %d", compute.computeQueues.count());
		Normal	printf("        %d secondary devices",/*  |  secondary compute queues: %lld", secondary.count, */secondary.count());
	}








	/**
	 * @brief Creates a logical device from a physical one
	 * @param pPD A pointer to the physical device structure containing its infos
	 * @param pLD A pointer to the logical device where to store the created device
	 * @param pComputeQueues A pointer to an array of compute queues
	 *		This is only used to know if the physical device is for graphics, computation or is secondary
	 */
	void createLogical(const _VkPhysicalDevice* pPD, VkDevice* pLD, RtArray<VkQueue>* pComputeQueues) {
		//List the queues of the device as unique int32s
		std::set<int32> uniqueQueueFamilyIndices;
		if(sameDevice(*pPD, graphics.PD)) {									//If it's the main device for graphics,
			uniqueQueueFamilyIndices.insert(pPD->indices.graphicsFamily);		//Add his graphics family
			uniqueQueueFamilyIndices.insert(pPD->indices.presentFamily);		//And his present family
		}
		for(uint32 i = 0; i < pPD->indices.computeFamilies.count(); ++i) {		//And then add every compute family, graphics ones included
			uniqueQueueFamilyIndices.insert(pPD->indices.computeFamilies[i]);
		}


		//Queue infos
		RtArray<VkDeviceQueueCreateInfo, uint32> queueCreateInfos;			//Create a queue create info array
		for(auto queueFamilyIndex : uniqueQueueFamilyIndices) {				//For every device queue family index found
			queueCreateInfos.add(VkDeviceQueueCreateInfo{						//Create a queue create info struct
				.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,				//Set structure type
				.queueFamilyIndex = (uint32)queueFamilyIndex,						//Set index
				.queueCount = 1,													//Set count		// ↓ Set priority. 1 for main devices, 0.5 for secondary ones
				.pQueuePriorities = new float((sameDevice(*pPD, graphics.PD) || sameDevice(*pPD, compute.PD)) ? 1.0f : 0.5f)
			});
		}


		//Required extensions
		VkPhysicalDeviceFeatures enabledDeviceFeatures{ 					//Set enabled features
			.fillModeNonSolid  = VK_FALSE,										//No point32 and line render, since we don't use meshes
			.multiViewport     = VK_FALSE,										//No multiple viewports
			.samplerAnisotropy = VK_FALSE										//No anistropy filter
		};

		//Fill deviceCreateInfo
		VkDeviceCreateInfo deviceCreateInfo{ 								//Create deviceCreateInfo structure for logical device creation
			.sType                    = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,	//Set structure type
			.queueCreateInfoCount     = queueCreateInfos.count(),				//Set queue infos count
			.pQueueCreateInfos        =    queueCreateInfos.begin(),			//Set queue infos
			.enabledLayerCount        = _dbg(validationLayersNum) _rls(0),	//Set validation layer count if in debug mode
			_dbg(.ppEnabledLayerNames = validationLayers,)					//Set validation layers      if in debug mode
			.enabledExtensionCount    =   requiredDeviceExtensionsNum,			//Set required extentions count
			.ppEnabledExtensionNames  = requiredDeviceExtensions,				//Set required extensions
			.pEnabledFeatures         = &enabledDeviceFeatures					//Set physical device enabled features
		};


		//Create the logical device and save its queues, exit if an error occurs
		VkDevice _logicalDevice;
		if(vkCreateDevice(pPD->device, &deviceCreateInfo, nullptr, &_logicalDevice) == VK_SUCCESS) {
			if(sameDevice(*pPD, graphics.PD) || sameDevice(*pPD, compute.PD)) {
				if(sameDevice(*pPD, graphics.PD)) {															//If it's the main graphics device
					graphics.LD = _logicalDevice;																//Set it as the main graphics logical device
					vkGetDeviceQueue(_logicalDevice, pPD->indices.graphicsFamily, 0, &graphics.graphicsQueue);	//Set graphics queue
					vkGetDeviceQueue(_logicalDevice, pPD->indices.presentFamily , 0, &graphics.presentQueue );	//Set present queue
				}
				if(pComputeQueues != nullptr) {																//If it's the main compute device and the function was called to create his logical device
					compute.LD = _logicalDevice;																//Set it as the main compute logical device
					for(uint32 i = 0; i < pPD->indices.computeFamilies.count(); ++i) {							//Add every compute queue to the main compute queue list
						VkQueue computeQueue;
						vkGetDeviceQueue(_logicalDevice, pPD->indices.computeFamilies[i], 0, &computeQueue);
						compute.computeQueues.add(computeQueue);
					}
				}
			}
			else {																							//If it's none of them
				*pLD = _logicalDevice;																			//Add it to the list of secondary logical devices
				for(uint32 i = 0; i < pPD->indices.computeFamilies.count(); ++i) {								//Add every compute queue to the secondary compute queues
					VkQueue computeQueue;
					vkGetDeviceQueue(_logicalDevice, pPD->indices.computeFamilies[i], 0, &computeQueue);
					pComputeQueues->add(computeQueue);
				}
			}
		}
		else dbg::printError("Failed to create logical device");
	}
}