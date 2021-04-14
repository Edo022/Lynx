#include "Lynx/Core/Core.hpp" //FIXME if this is places after Devices.cpp, G++ is unable to find glfwCreateWindowSurface
#include "Lynx/Core/Devices.hpp"
#include "Lynx/Core/Render/Window/Swapchain.hpp"
#include "Lynx/Core/AutoInit.hpp"
#include "Lynx/System/SystemInfo.hpp"

#include <set>
#include <vector>

//Compares 2 _VkPhysicalDevice objects
alwaysInline constexpr bool sameDevice(const _VkPhysicalDevice& a, const _VkPhysicalDevice& b){ return a.properties.deviceID == b.properties.deviceID; }






namespace lnx::core::dvc{
	alignCache graphicsDevice         graphics;
	// alignCache computeDevice          compute;
	alignCache RtArray<computeDevice> secondary;


	alignCache vk::Instance   instance;
	alignCache GLFWwindow*  dummyWindow;
	alignCache vk::SurfaceKHR dummySurface;

	alignCache uint32       requiredDeviceExtensionsNum = 1;
	alignCache const char** requiredDeviceExtensions    = new const char*{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	#ifdef LNX_DEBUG
		alignCache VkDebugUtilsMessengerEXT debugMessenger;
		alignCache uint32       validationLayersNum = 1;
		alignCache const char** validationLayers    = new const char*{ "VK_LAYER_KHRONOS_validation" };
	#endif




	LnxAutoInit(LNX_H_DEVICES){
		if(!glfwInit()) exit(-1);																//Initialize GLFW
		//TODO ADD ERROR MESSAGE

		//Extensions
		uint32 glfwExtensionCount;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);	//Get extensions list and count
		//! ^ Freed by GLFW
		const char** extensions = (const char**)malloc(sizeof(const char*) * (glfwExtensionCount _dbg(+ 1)));
		for(uint32 i = 0; i < glfwExtensionCount; ++i) extensions[i] = glfwExtensions[i];		//Save them into an array
		_dbg(extensions[glfwExtensionCount] = (VK_EXT_DEBUG_UTILS_EXTENSION_NAME));				//Add debug extension if in debug mode


		#ifdef LNX_DEBUG
			//Create debugCreateInfo structure
			vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo;
			core::debug::populateDebugMessengerCreateInfo(debugCreateInfo);
			//!^ Ok. vk::DebugUtilsMessengerCreateInfoEXT has implicit conversion to VkDebugUtilsMessengerCreateInfoEXT
		#endif


		auto appInfo = vk::ApplicationInfo()
			.setPApplicationName   ("Lynx")
			.setApplicationVersion (VK_MAKE_VERSION(1, 0, 0))
			.setPEngineName        ("Lynx")
			.setEngineVersion      (VK_MAKE_VERSION(1, 0, 0))
			.setApiVersion         (VK_API_VERSION_1_2)
		;

		//Create instance
		auto createInfo = vk::InstanceCreateInfo()
			.setPNext                   (_dbg(&debugCreateInfo)    _rls(nullptr))
			.setPApplicationInfo        (&appInfo)
			.setEnabledLayerCount       (_dbg(validationLayersNum) _rls(0))
			.setPpEnabledLayerNames     (_dbg(validationLayers)    _rls(nullptr))
			.setEnabledExtensionCount   (glfwExtensionCount _dbg(+ 1))
			.setPpEnabledExtensionNames (extensions)
		;

		//Add validation layers if in debug mode
		#ifdef LNX_DEBUG																	//Search for validation layers
			uint32 layerCount = 0;

			//Get layer count
			switch(vk::enumerateInstanceLayerProperties(&layerCount, nullptr)){
				case vk::Result::eIncomplete:             dbg::printError("Incomplete properties"); break;
				case vk::Result::eErrorOutOfDeviceMemory: dbg::printError("Out of devide memory");  break;
				case vk::Result::eErrorOutOfHostMemory:   dbg::printError("Out of host memory");    break;
				default: break;
			};

			//Get layers
			RtArray<vk::LayerProperties> availableLayers(layerCount);
			switch(vk::enumerateInstanceLayerProperties(&layerCount, availableLayers.begin())){
				case vk::Result::eIncomplete:             dbg::printError("Incomplete properties"); break;
				case vk::Result::eErrorOutOfDeviceMemory: dbg::printError("Out of devide memory");  break;
				case vk::Result::eErrorOutOfHostMemory:   dbg::printError("Out of host memory");    break;
				default: break;
			};

			for(uint32 i = 0; i < validationLayersNum; ++i) {								//For every layer,
				for(uint32 j = 0; j < availableLayers.count(); ++j) {							//For every available layer
					if(0 == strcmp(validationLayers[i], availableLayers[j].layerName)) break;		//Check if the layer is available
					else if(i == availableLayers.count() - 1) {										//If not
						dbg::printError("Validation layers not available. Cannot run in debug mode");	//Print an error
					}
				}
			}
		#endif

		core::dvc::instance = vk::createInstance(createInfo, nullptr);
		free(extensions);



		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //TODO automatically get GLFW version
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);	//TODO automatically get GLFW version
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		dummyWindow = glfwCreateWindow(1, 1, "", nullptr, nullptr);	//Initialize dummy window
		glfwHideWindow(dummyWindow);
		glfwCreateWindowSurface(instance, dummyWindow, nullptr, rcast<vk::SurfaceKHR::CType*>(&dummySurface)); //Initialize dummy surface


		dbg::checkVk(glfwCreateWindowSurface(instance, dummyWindow, nullptr, rcast<vk::SurfaceKHR::CType*>(&dummySurface)), "Failed to create window surface");
		getPhysical();
	}





	//Rates a physical device based on its properties and features
	//*   pDevice: a pointer to the device structure where its infos are stored
	//*   Returns the rating of the physical device
	uint32 rate(const _VkPhysicalDevice* pDevice) {
		uint32 score = 0;																				//Device performance evalutation
		if(pDevice->properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) score += 1000000;	//Discrete GPUs have performance advantage
		//pDevice->properties.limits.maxComputeSharedMemorySize; //TODO dont create shaders larger than the device limit
		score += pDevice->properties.limits.maxImageDimension2D;										//Maximum possible count of textures affects graphics quality
		if(pDevice->features.geometryShader) score += 1;												//Geometry shaders needed
		return score;
	}




	/**
	 * @brief Checks if a device has the required extensions and properties to run vulkan
	 * @param vDevice The physical device to check
	 * @param pErrorText A pointer to a lnx::String where to store the error in case the device is not suitable
	 * @return return True if the device is suitable, false if not
	 */
	bool isSuitable(const vk::PhysicalDevice vDevice, String& pErrorText) {
		//Check extensions
		if(!checkExtensions(vDevice)) {
			pErrorText = "Missing required extensions";
			return false;
		}

		//Check swapchain support
		else {
			uint32 surfaceFormatsCount = 0, presentModesCount = 0;
			switch(vDevice.getSurfaceFormatsKHR(dummySurface, &surfaceFormatsCount, nullptr)){
				case vk::Result::eIncomplete:             dbg::printError("Incomplete surface formats"); break;
				case vk::Result::eErrorSurfaceLostKHR:    dbg::printError("Surface lost");               break;
				case vk::Result::eErrorOutOfDeviceMemory: dbg::printError("Out of devide memory");       break;
				case vk::Result::eErrorOutOfHostMemory:   dbg::printError("Out of host memory");         break;
				case vk::Result::eSuccess: break;
				default: dbg::printError("Unknown result");
			};

			switch(vDevice.getSurfacePresentModesKHR(dummySurface, &presentModesCount,   nullptr)){
				case vk::Result::eIncomplete:             dbg::printError("Incomplete surface formats"); break;
				case vk::Result::eErrorSurfaceLostKHR:    dbg::printError("Surface lost");               break;
				case vk::Result::eErrorOutOfDeviceMemory: dbg::printError("Out of devide memory");       break;
				case vk::Result::eErrorOutOfHostMemory:   dbg::printError("Out of host memory");         break;
				case vk::Result::eSuccess: break;
				default: dbg::printError("Unknown result");
			};

			if(!surfaceFormatsCount || !presentModesCount) {
				pErrorText = "Unsupported swapchain";
				return false;
			}
		}
		return true;
	}




	/**
	 * @brief Returns true if the device supports the extensions, false if not
	 */
	bool checkExtensions(const vk::PhysicalDevice vDevice) {
		uint32 extensionCount;

		//Get extension count
		switch(vDevice.enumerateDeviceExtensionProperties(nullptr, &extensionCount, nullptr)){
			case vk::Result::eIncomplete:             dbg::printError("Incomplete extensions"); break;
			case vk::Result::eErrorLayerNotPresent:   dbg::printError("Layer not present");     break;
			case vk::Result::eErrorOutOfDeviceMemory: dbg::printError("Out of devide memory");  break;
			case vk::Result::eErrorOutOfHostMemory:   dbg::printError("Out of host memory");    break;
			case vk::Result::eSuccess: break;
			default: dbg::printError("Unknown result");
		};

		//Get extensions
		RtArray<vk::ExtensionProperties> availableExtensions(extensionCount);
		switch(vDevice.enumerateDeviceExtensionProperties(nullptr, &extensionCount, availableExtensions.begin())){
			case vk::Result::eIncomplete:             dbg::printError("Incomplete extensions"); break;
			case vk::Result::eErrorLayerNotPresent:   dbg::printError("Layer not present");     break;
			case vk::Result::eErrorOutOfDeviceMemory: dbg::printError("Out of devide memory");  break;
			case vk::Result::eErrorOutOfHostMemory:   dbg::printError("Out of host memory");    break;
			default: break;
		};

		//TODO dont use std
		std::set<const char*> requiredExtensions(requiredDeviceExtensions, requiredDeviceExtensions);
		for(const auto& extension : availableExtensions) requiredExtensions.erase(extension.extensionName);		//Search for required extensions
		return requiredExtensions.empty();
	}




	/**
	 * @brief Finds and returns the queue families of a physical device
	 */
	QueueFamilyIndices getQueueFamilies(const vk::PhysicalDevice vDevice) {
		uint32 queueFamilyCount = 0;
		vDevice.getQueueFamilyProperties(&queueFamilyCount, nullptr);						//Enumerate queue families
		RtArray<vk::QueueFamilyProperties> queueFamilies;
		queueFamilies.resize(queueFamilyCount);
		vDevice.getQueueFamilyProperties(&queueFamilyCount, queueFamilies.begin());			//Save queue families

		//Set families
		QueueFamilyIndices indices;
		for(uint32 i = 0; i < queueFamilies.count(); ++i) {												//For every queue family
			if(queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics) indices.graphicsFamily = i;		//Set graphics family
			if(queueFamilies[i].queueFlags & vk::QueueFlagBits::eCompute ) indices.computeFamilies.add(i);	//Add compute families

			vk::Bool32 hasPresentSupport = false;
			switch(vDevice.getSurfaceSupportKHR(i, dummySurface, &hasPresentSupport)){								//Set present family
				case vk::Result::eErrorSurfaceLostKHR:    dbg::printError("Surface lost");         break;
				case vk::Result::eErrorOutOfDeviceMemory: dbg::printError("Out of devide memory"); break;
				case vk::Result::eErrorOutOfHostMemory:   dbg::printError("Out of host memory");   break;
				case vk::Result::eSuccess: break;
				default: dbg::printError("Unknown result");
			};


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

		//Get physical device count
		switch(instance.enumeratePhysicalDevices(&deviceCount, nullptr)){
			case vk::Result::eIncomplete:                dbg::printError("Incomplete devices");    break;
			case vk::Result::eErrorInitializationFailed: dbg::printError("Initialization failed"); break;
			case vk::Result::eErrorOutOfDeviceMemory:    dbg::printError("Out of devide memory");  break;
			case vk::Result::eErrorOutOfHostMemory:      dbg::printError("Out of host memory");    break;
			case vk::Result::eSuccess: break;
			default: dbg::printError("Unknown result");
		};

		if(deviceCount == 0) dbg::printError("Failed to find GPUs with Vulkan support");	//Check if there is at least one deice that supports vulkan //FIXME add runtime support

		//Get physical devices
		RtArray<vk::PhysicalDevice> physDevices(deviceCount);
		switch(instance.enumeratePhysicalDevices(&deviceCount, physDevices.begin())){
			case vk::Result::eIncomplete:                dbg::printError("Incomplete devices");    break;
			case vk::Result::eErrorInitializationFailed: dbg::printError("Initialization failed"); break;
			case vk::Result::eErrorOutOfDeviceMemory:    dbg::printError("Out of devide memory");  break;
			case vk::Result::eErrorOutOfHostMemory:      dbg::printError("Out of host memory");    break;
			case vk::Result::eSuccess: break;
			default: dbg::printError("Unknown result");
		};


		for(uint32 i = 0; i < physDevices.count(); ++i) {									//For every physical device, create and save a _VkPhysicalDevice stucture
			auto properties = physDevices[i].getProperties();
			auto features   = physDevices[i].getFeatures();
			String errorText;
			if(isSuitable(physDevices[i], errorText)) {											//If it's suitable
				physicalDevices.add(new _VkPhysicalDevice(physDevices[i], properties, features, *new QueueFamilyIndices));	//Add it to the physical devices vector
			}
			else {																				//If not
				discardedPhysicalDevices.add(properties.deviceName.cbegin());						//Add it to the discarded devices vector
				discardedPhysicalDevices.add(errorText);											//And save the reason of its unsuitability
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
		if(physicalDevices.count() > 0) {									//If there are suitable devices
			// graphics.PD = compute.PD = *physicalDevices[0];						//set graphics device at default value
			graphics.PD = *physicalDevices[0];						//set graphics device at default value
			for(uint32 i = 0; i < physicalDevices.count(); ++i) {				//For every physical device
				physDev.indices = getQueueFamilies(physDev.device);					//Get its queue families
				physDev.score = rate(&physDev);										//And its score. Then check if it has the necessary queues and set it as the main graphics and or compute physical device
				if(physDev.score > graphics.PD.score || physDev.indices.graphicsFamily != (uint32)-1)        graphics.PD = physDev;
				// if(physDev.score > compute .PD.score || physDev.indices.computeFamilies.count() > 0) compute.PD = physDev;
			}
			for(uint32 i = 0; i < physicalDevices.count(); ++i) {				//For every physical device that isn't the main graphics or compute device
				// if(!sameDevice(physDev, graphics.PD) && !sameDevice(physDev, compute.PD)) {
				if(!sameDevice(physDev, graphics.PD)) {
					secondary.resize(secondary.count() + 1);
					secondary[secondary.count() - 1].PD = physDev;					//Add it to the secondary devices vector (it'll be used as a compute device with less priority. T.T poor gpu)
				}
			}

			//Print the devices names, IDs, scores and tasks
			Success printf("    Found %d suitable device%s:", physicalDevices.count(), (physicalDevices.count() == 1) ? "" : "s");
			for(uint32 i = 0; i < physicalDevices.count(); ++i) {
				// if(sameDevice(physDev, graphics.PD) || sameDevice(physDev, compute.PD)) Main else Normal;
				if(sameDevice(physDev, graphics.PD)) Main else Normal;
				printf("        %s  |  ID: %d  |  %d", physDev.properties.deviceName.cbegin(), physDev.properties.deviceID, physDev.score);
				if(sameDevice(physDev, graphics.PD)) printf("  |  Main graphics");
				// if(sameDevice(physDev, compute.PD))  printf("  |  Main compute");
			}
		}
		//FIXME add runtime support
		else dbg::printError("Failed to find a suitable GPU");
		#undef physDev




		//FIXME
		//Create a logical device for graphics, one for computation and one for every secondary device
		// createLogical(&graphics.PD, &graphics.LD, nullptr);
		// createLogical(&compute.PD,  &compute.LD, &compute.computeQueues);
		createLogical(&graphics.PD, &graphics.LD, &graphics.computeQueues);
		//FIXME

		for(uint32 i = 0; i < secondary.count(); ++i) {
			createLogical(&secondary[i].PD, &secondary[i].LD, &secondary[i].computeQueues);
		}

		//Output created logical devices and queues
		Success printf("    Created %d logical devices:", 2 + secondary.count());
		Main	printf("        Main graphics  |  graphics queues: 1  |  present queues:  1");
		// Main	printf("        Main compute   |  compute queues:  %d", compute.computeQueues.count());
		Normal	printf("        %d secondary devices",/*  |  secondary compute queues: %lld", secondary.count, */secondary.count());
	}








	/**
	 * @brief Creates a logical device from a physical one
	 * @param pPD A pointer to the physical device structure containing its infos
	 * @param pLD A pointer to the logical device where to store the created device
	 * @param pComputeQueues A pointer to an array of compute queues
	 *		This is only used to know if the physical device is for graphics, computation or is secondary
	 */
	void createLogical(const _VkPhysicalDevice* pPD, vk::Device* pLD, RtArray<vk::Queue>* pComputeQueues) {
		//List the queues of the device as unique int32s
		std::set<int32> uniqueQueueFamilyIndices;
		if(sameDevice(*pPD, graphics.PD)) {										//If it's the main device for graphics,
			uniqueQueueFamilyIndices.insert((i32)pPD->indices.graphicsFamily);		//Add his graphics family
			uniqueQueueFamilyIndices.insert((i32)pPD->indices.presentFamily);		//And his present family
		}
		for(uint32 i = 0; i < pPD->indices.computeFamilies.count(); ++i) {		//And then add every compute family, graphics ones included
			uniqueQueueFamilyIndices.insert((i32)pPD->indices.computeFamilies[i]);
		}


		//Queue infos
		RtArray<vk::DeviceQueueCreateInfo, uint32> queueCreateInfos;			//Create a queue create info array
		for(auto queueFamilyIndex : uniqueQueueFamilyIndices) {					//For every device queue family index found
			queueCreateInfos.add(vk::DeviceQueueCreateInfo()						//Create a queue create info struct
				.setQueueFamilyIndex ((uint32)queueFamilyIndex)						//Set index
				.setQueueCount       (1)											//Set count		// ↓ Set priority. 1 for main devices, 0.5 for secondary ones
				// .setPQueuePriorities (new float((sameDevice(*pPD, graphics.PD) || sameDevice(*pPD, compute.PD)) ? 1.0f : 0.5f))
				.setPQueuePriorities (new float((sameDevice(*pPD, graphics.PD)) ? 1.0f : 0.5f))
			);
		}


		//Required extensions
		auto enabledDeviceFeatures = vk::PhysicalDeviceFeatures() 				//Set enabled features
			.setFillModeNonSolid  (VK_FALSE)									//No point32 and line render, since we don't use meshes
			.setMultiViewport     (VK_FALSE)									//No multiple viewports
			.setSamplerAnisotropy (VK_FALSE)									//No anistropy filter
		;

		//Fill deviceCreateInfo
		auto deviceCreateInfo = vk::DeviceCreateInfo() 							//Create deviceCreateInfo structure for logical device creation
			.setQueueCreateInfoCount     (queueCreateInfos.count())					//Set queue infos count
			.setPQueueCreateInfos        (queueCreateInfos.begin())					//Set queue infos
			.setEnabledLayerCount        (_dbg(validationLayersNum) _rls(0))		//Set validation layer count if in debug mode
			.setPpEnabledLayerNames      (_dbg(validationLayers)    _rls(nullptr))	//Set validation layers      if in debug mode
			.setEnabledExtensionCount    (requiredDeviceExtensionsNum)				//Set required extentions count
			.setPpEnabledExtensionNames  (requiredDeviceExtensions)					//Set required extensions
			.setPEnabledFeatures         (&enabledDeviceFeatures)					//Set physical device enabled features
		;


		//Create the logical device and save its queues, exit if an error occurs
		vk::Device _logicalDevice;
		if(pPD->device.createDevice(&deviceCreateInfo, nullptr, &_logicalDevice) == vk::Result::eSuccess) {
			// if(sameDevice(*pPD, graphics.PD) || sameDevice(*pPD, compute.PD)) {
			if(sameDevice(*pPD, graphics.PD)) {
				if(sameDevice(*pPD, graphics.PD)) {													//If it's the main graphics device
					graphics.LD = _logicalDevice;														//Set it as the main graphics logical device
					_logicalDevice.getQueue(pPD->indices.graphicsFamily, 0, &graphics.graphicsQueue);	//Set graphics queue
					_logicalDevice.getQueue(pPD->indices.presentFamily , 0, &graphics.presentQueue );	//Set present queue

					//FIXME //FIXME
					for(uint32 i = 0; i < pPD->indices.computeFamilies.count(); ++i) {					//Add every compute queue to the main compute queue list
						vk::Queue computeQueue;
						_logicalDevice.getQueue(pPD->indices.computeFamilies[i], 0, &computeQueue);
						graphics.computeQueues.add(computeQueue);
					}
					//FIXME //FIXME
				}
				// if(pComputeQueues != nullptr) {														//If it's the main compute device and the function was called to create his logical device
				// 	compute.LD = _logicalDevice;														//Set it as the main compute logical device
				// 	for(uint32 i = 0; i < pPD->indices.computeFamilies.count(); ++i) {					//Add every compute queue to the main compute queue list
				// 		vk::Queue computeQueue;
				// 		_logicalDevice.getQueue(pPD->indices.computeFamilies[i], 0, &computeQueue);
				// 		compute.computeQueues.add(computeQueue);
				// 	}
				// }
			}
			else {																				//If it's none of them
				*pLD = _logicalDevice;																//Add it to the list of secondary logical devices
				for(uint32 i = 0; i < pPD->indices.computeFamilies.count(); ++i) {					//Add every compute queue to the secondary compute queues
					vk::Queue computeQueue;
					_logicalDevice.getQueue(pPD->indices.computeFamilies[i], 0, &computeQueue);
					pComputeQueues->add(computeQueue);
				}
			}
		}
		else dbg::printError("Failed to create logical device");
	}
}