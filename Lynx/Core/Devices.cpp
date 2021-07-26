#include "Lynx/Core/Core.hpp" //FIXME if this is places after Devices.cpp, G++ is unable to find glfwCreateWindowSurface
#include "Lynx/Core/Devices.hpp"
#include "Lynx/Core/Render/Window/Swapchain.hpp"
#include "Lynx/Core/AutoInit.hpp"
#include "Lynx/System/SystemInfo.hpp"
#include <set>
#include <vector>








/**
 * @brief Compares 2 _VkPhysicalDevice objects
 *     This function should only be used by the engine
 * Complexity: O(1)
 * @param pA The first device
 * @param pB The second device
 * @return alwaysInline constexpr
 */
alwaysInline constexpr bool sameDevice(const _VkPhysicalDevice& pA, const _VkPhysicalDevice& pB){
	return pA.properties.deviceID == pB.properties.deviceID;
}




namespace lnx::core::dvc{
	alignCache Device graphics;
	alignCache RtArray<Device> secondary;


	alignCache vk::Instance   instance;
	alignCache GLFWwindow*    dummyWindow;
	alignCache vk::SurfaceKHR dummySurface;

	alignCache uint32       requiredDeviceExtensionsNum = 1;
	alignCache const char** requiredDeviceExtensions    = new const char*{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	#ifdef LNX_DEBUG
		alignCache VkDebugUtilsMessengerEXT debugMessenger;
		alignCache uint32       validationLayersNum = 1;
		alignCache const char** validationLayers    = new const char*{ "VK_LAYER_KHRONOS_validation" };
	#endif








	LnxAutoInit(LNX_H_DEVICES){
		dbg::checkCond(!glfwInit(), "GLFW not initialized");

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
		#ifdef LNX_DEBUG
			uint32 layerCount = 0;

			//Get layer count
			switch(vk::enumerateInstanceLayerProperties(&layerCount, nullptr)){
				case vk::Result::eIncomplete: dbg::logError("Incomplete properties"); break;
				vkDefaultCases;
			};

			//Get layers
			RtArray<vk::LayerProperties> availableLayers(layerCount);
			switch(vk::enumerateInstanceLayerProperties(&layerCount, availableLayers.begin())){
				case vk::Result::eIncomplete: dbg::logError("Incomplete properties"); break;
				vkDefaultCases;
			};

			for(uint32 i = 0; i < validationLayersNum; ++i) {								//For every layer,
				for(uint32 j = 0; j < availableLayers.count(); ++j) {							//For every available layer
					if(0 == strcmp(validationLayers[i], availableLayers[j].layerName)) break;		//Check if the layer is available
					else if(i == availableLayers.count() - 1) {										//If not
						dbg::logError("Validation layers not available. Cannot run in debug mode");	//Print an error
					}
				}
			}
		#endif

		switch(vk::createInstance(&createInfo, nullptr, &core::dvc::instance)){
			case vk::Result::eErrorInitializationFailed: dbg::logError("Initialization failed"); break;
			case vk::Result::eErrorLayerNotPresent:      dbg::logError("Layer not present");     break;
			case vk::Result::eErrorExtensionNotPresent:  dbg::logError("Extension not present"); break;
			case vk::Result::eErrorIncompatibleDriver:   dbg::logError("Incompatible driver");   break;
			vkDefaultCases;
		}
		free(extensions);



		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //TODO automatically get GLFW version
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //TODO automatically get GLFW version
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		dummyWindow = glfwCreateWindow(1, 1, "", nullptr, nullptr);	//Initialize dummy window
		glfwHideWindow(dummyWindow);

		switch(glfwCreateWindowSurface(instance, dummyWindow, nullptr, rcast<vk::SurfaceKHR::CType*>(&dummySurface))){
			case VkResult::VK_SUCCESS: break;
			case VkResult::VK_ERROR_INITIALIZATION_FAILED: dbg::logError("Initialization failed"); break;
			case VkResult::VK_ERROR_EXTENSION_NOT_PRESENT: dbg::logError("Extension not present"); break;
			default: _dbg(dbg::logError("Unknown result")) _rls(noop);
		}
		getPhysicalDevices();
	}








	//-----------------------------------------------------------------------------------------------------------------------------//








	/**
	 * @brief Rates a physical device based on its properties and features
	 *     This function should only be used by the engine
	 * Complexity: O(1)
	 * @param pPDevice The device structure where its infos are stored
	 * @return The rating of the physical device
	 */
	uint32 rate(const _VkPhysicalDevice& pPDevice) {
		uint32 score = 0;																				//Device performance evalutation
		if(pPDevice.properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) score += 1000000;	//Discrete GPUs have performance advantage
		//pPDevice.properties.limits.maxComputeSharedMemorySize;										//TODO dont create shaders larger than the device limit
		score += pPDevice.properties.limits.maxImageDimension2D;										//Maximum possible count of textures affects graphics quality
		if(pPDevice.features.geometryShader) score += 1;												//Geometry shaders needed
		return score;
	}








	/**
	 * @brief Checks if a device has the required extensions and properties to run vulkan
	 *     This function should only be used by the engine
	 * Complexity: O(1)
	 * @param vDevice The physical device to check
	 * @param pErrorText A pointer to a lnx::String where to store the error in case the device is not suitable
	 * @return return True if the device is suitable, false if not
	 */
	bool isSuitable(const vk::PhysicalDevice vDevice, const bool vAllowLLVM, String& pErrorText) {
		//Discard llvmpipe devices if required
		if(!vAllowLLVM){
			String name = vDevice.getProperties().deviceName.cbegin();
			for (char* c = name.begin(); *c; ++c) *c = tolower(*c); //TODO ADD TOLOWER FUNCTIONS TO LUX STRING
			if(strstr(name.begin(), "llvm")){
				pErrorText = "Device is not stable";
				return false;
			}
		}
		else { dbg::logWarn("No device available. Using llvmpipe"); }

		//Check extensions
		if(!checkExtensions(vDevice)) {
			pErrorText = "Missing required extensions";
			return false;
		}

		//Check swapchain support
		else {
			uint32 surfaceFormatsCount = 0, presentModesCount = 0;
			switch(vDevice.getSurfaceFormatsKHR(dummySurface, &surfaceFormatsCount, nullptr)){
				case vk::Result::eIncomplete:             dbg::logError("Incomplete surface formats"); break;
				case vk::Result::eErrorSurfaceLostKHR:    dbg::logError("Surface lost");               break;
				vkDefaultCases;
			};

			switch(vDevice.getSurfacePresentModesKHR(dummySurface, &presentModesCount,   nullptr)){
				case vk::Result::eIncomplete:             dbg::logError("Incomplete surface formats"); break;
				case vk::Result::eErrorSurfaceLostKHR:    dbg::logError("Surface lost");               break;
				vkDefaultCases;
			};

			if(!surfaceFormatsCount || !presentModesCount) {
				pErrorText = "Unsupported swapchain";
				return false;
			}
		}
		return true;
	}








	/**
	 * @brief Checks if a device supports the extensions required to run the engine
	 *     This function should only be used by the engine
	 * Complexity: O(n)
	 *     where n = number of available extensions
	 * @return True if the device supports the extensions, false if not
	 */
	bool checkExtensions(const vk::PhysicalDevice vDevice) {
		uint32 extensionCount;

		//Get extension count
		switch(vDevice.enumerateDeviceExtensionProperties(nullptr, &extensionCount, nullptr)){
			case vk::Result::eIncomplete:           dbg::logError("Incomplete extensions"); break;
			case vk::Result::eErrorLayerNotPresent: dbg::logError("Layer not present");     break;
			vkDefaultCases;
		};

		//Get extensions
		RtArray<vk::ExtensionProperties> availableExtensions(extensionCount);
		switch(vDevice.enumerateDeviceExtensionProperties(nullptr, &extensionCount, availableExtensions.begin())){
			case vk::Result::eIncomplete:           dbg::logError("Incomplete extensions"); break;
			case vk::Result::eErrorLayerNotPresent: dbg::logError("Layer not present");     break;
			vkDefaultCases;
		};


		//Check that each required extension is supported
		for(uint32 i = 0; i < requiredDeviceExtensionsNum; ++i){
			bool r = false;
			for(uint32 j = 0; j < availableExtensions.count(); ++j){
				r |= !strcmp(requiredDeviceExtensions[i], availableExtensions[j].extensionName);
				if(r) break;
			}
			if(!r) return false;
		}
		return true;
	}








	/**
	 * @brief Finds and returns the queue families of a physical device
	 *     This function should only be used by the engine
	 * Complexity: O(n)
	 *     where n = number of queue families of the device
	 * @param vDev The physical device
	 * @return The queue families of the device as a QueueFamilyIndices object
	 */
	QueueFamilyIndices getQueueFamilies(const vk::PhysicalDevice vDev) {
		//Enumerate queue families
		uint32 queueFamilyCount = 0;
		vDev.getQueueFamilyProperties(&queueFamilyCount, nullptr);

		//Save queue families
		RtArray<vk::QueueFamilyProperties> queueFamilies;
		queueFamilies.resize(queueFamilyCount);
		vDev.getQueueFamilyProperties(&queueFamilyCount, queueFamilies.begin());

		//Set families
		QueueFamilyIndices indices;
		for(uint32 i = 0; i < queueFamilies.count(); ++i) {												//For every queue family
			if(queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics) indices.graphicsFamily = i;		//Set graphics family
			if(queueFamilies[i].queueFlags & vk::QueueFlagBits::eCompute ) indices.computeFamilies.add(i);	//Add compute families

			vk::Bool32 hasPresentSupport = false;
			switch(vDev.getSurfaceSupportKHR(i, dummySurface, &hasPresentSupport)){						//Set present family
				case vk::Result::eErrorSurfaceLostKHR: dbg::logError("Surface lost"); break;
				vkDefaultCases;
			};

			if(hasPresentSupport) indices.presentFamily = i;
		}
		return indices;
	}








	//-----------------------------------------------------------------------------------------------------------------------------//








	//TODO add multiple gpu support
	//TODO use different score for graphics and compute devices
	//FIXME print errors in release mode too idk
	/**
	 * @brief Finds all the suitable physical devices, choosing the main and secondary devices according to their capabilities
	 *     This function should only be used by the engine
	 * Complexity: O(n)
	 *     where n = number of physical devices in the system
	 */
	void getPhysicalDevices() {
		//Get physical device count
		uint32 deviceCount = 0;					//Number of physical devices. Used to call vkEnumeratePhysicalDevices
		switch(instance.enumeratePhysicalDevices(&deviceCount, nullptr)){
			case vk::Result::eIncomplete:                dbg::logError("Incomplete devices");    break;
			case vk::Result::eErrorInitializationFailed: dbg::logError("Initialization failed"); break;
			vkDefaultCases;
		};
		if(deviceCount == 0) dbg::logError("Failed to find GPUs with Vulkan support");


		//Get physical devices
		RtArray<vk::PhysicalDevice> physDevices(deviceCount);
		switch(instance.enumeratePhysicalDevices(&deviceCount, physDevices.begin())){
			case vk::Result::eIncomplete:                dbg::logError("Incomplete devices");    break;
			case vk::Result::eErrorInitializationFailed: dbg::logError("Initialization failed"); break;
			vkDefaultCases;
		};





		RtArray<String>             discarded;	//Array of strings containing the discarded physical devices names
		RtArray<_VkPhysicalDevice*> suitable;	//Array of physical devices

		for(auto& vkpdevice : physDevices) {							//For every physical device
			auto properties = vkpdevice.getProperties();					//Get properties
			auto features   = vkpdevice.getFeatures();						//Get features
			String errorText;
			if(isSuitable(vkpdevice, physDevices.count() == 1, errorText)) {//If it's suitable, add it to the physical devices array
				suitable.add(new _VkPhysicalDevice(vkpdevice, properties, features, *new QueueFamilyIndices));
			}
			else {															//If not
				discarded.add(properties.deviceName.cbegin());					//Add its name to the discarded devices array
				discarded.add(errorText);										//Save the reason of its unsuitability
			}
		}


		if(suitable.count() > 0) {										//If there are suitable devices
			graphics.pd = *suitable[0];										//Set graphics device at default value
			for(auto& dev : suitable) {										//For every physical device
				dev->indices = getQueueFamilies(dev->device);					//Get its queue families
				dev->score = rate(*dev);										//Get its score. If it has the highest score and an available graphics queue
				if((dev->score > graphics.pd.score && dev->indices.graphicsFamily != (uint32)-1) || graphics.pd.indices.graphicsFamily == (uint32)-1) {
					graphics.pd = *dev;												//Set it as the main graphics device
					createLogicalDevices(graphics.pd, graphics);					//Create the graphics logical device
				}
			}
			for(auto& dev : suitable) {										//For every physical device
				if(!sameDevice(*dev, graphics.pd)) {							//If it's not the main graphics device
					secondary.resize(secondary.count() + 1);						//Add it to the secondary compute devices array
					secondary[secondary.count() - 1].pd = *dev;						//Create its logical device
					createLogicalDevices(secondary[secondary.count() - 1].pd, secondary[secondary.count() - 1]);
				}
			}




			//If there are discarded devices, print their names
			if(discarded.count() > 0) {
				String discardedList = "    Discarded devices:";
				for(uint32 i = 0; i < discarded.count(); i += 2) {
					//FIXME use proper lnx::String format function
					discardedList += dbg::string_format("        %s  |  %s", (char*)discarded[i].begin(), (char*)discarded[(uint64)i + 1].begin()).c_str();
				}
				//FIXME write overload for logs with lnx::String
				dbg::logWarn(discardedList.begin());
			}

			//Print the devices names, IDs, scores and tasks
			dbg::logInfo("    Found %d suitable device%s:", suitable.count(), (suitable.count() == 1) ? "" : "s");
			for(auto& dev : suitable) {
				//FIXME unify output string
				dbg::logInfo("        %s  |  ID: %d  |  %d", dev->properties.deviceName.cbegin(), dev->properties.deviceID, dev->score);
				if(sameDevice(*dev, graphics.pd)) dbg::logInfo("  |  Main graphics");
			}

			//Print created logical devices and queues
			dbg::logInfo("    Created %d logical device%s:", 1 + secondary.count(), (secondary.count() ? "s" : ""));
			dbg::logInfo("        Main graphics  |  graphics queues: 1  |  present queues: 1  |  compute queues: %d", graphics.pd.indices.computeFamilies.count());
			if(secondary.count()) {
				for(auto& dev : secondary){
					dbg::logInfo("        Compute        |  compute queues: %d", dev.pd.indices.computeFamilies.count());
				}
			}
		}
		else { dbg::logError("Failed to find a suitable GPU"); }
	}








	/**
	 * @brief Creates a logical device from a physical device wrapper
	 *     This function should only be used by the engine
	 * Complexity: O(n)
	 *     where n = pPdev.indices.computeFamilies.count()
	 * @param pPDev The physical device to create the logical one from
	 * @param pDev The logical object device where to store the newly created one
	 */
	void createLogicalDevices(const _VkPhysicalDevice& pPDev, Device& pDev) {
		RtArray<uint32> qIndices;
		if(sameDevice(pPDev, graphics.pd)) {							//If it's the main device for graphics
			qIndices.addUnique(pPDev.indices.graphicsFamily);				//Add its graphics queue index
			qIndices.addUnique(pPDev.indices.presentFamily);				//Add its present  queue index
		}
		for(const auto cqIndex : pPDev.indices.computeFamilies) {		//Add every compute index
			qIndices.addUnique(cqIndex);
		}


		RtArray<vk::DeviceQueueCreateInfo, uint32> queueInfos;				//Create queues
		for(const auto qIndex : qIndices) {									//For every queue index found
			queueInfos.add(vk::DeviceQueueCreateInfo()							//Create a queue create info struct
				.setQueueFamilyIndex (qIndex)										//Set queue index
				.setQueueCount       (1)											//Use one queue
				.setPQueuePriorities (new float(1.0f)) 								//Set priority
				// .setPQueuePriorities (new float((sameDevice(pPDev, graphics.pd)) ? 1.0f : 0.5f)) //FIXME
			);
		}


		auto enabledFeatures = vk::PhysicalDeviceFeatures() 					//Set enabled features
			.setFillModeNonSolid  (VK_FALSE)										//No point32 and line render, since we don't use meshes
			.setMultiViewport     (VK_FALSE)										//No multiple viewports
			.setSamplerAnisotropy (VK_FALSE)										//No anistropy filter
		;

		auto deviceCreateInfo = vk::DeviceCreateInfo() 							//Create logical device
			.setQueueCreateInfoCount     (queueInfos.count())						//Set queue infos count
			.setPQueueCreateInfos        (queueInfos.begin())						//Set queue infos
			.setEnabledLayerCount        (_dbg(validationLayersNum) _rls(0))		//Set validation layers count if in debug mode
			.setPpEnabledLayerNames      (_dbg(validationLayers)    _rls(nullptr))	//Set validation layers       if in debug mode
			.setEnabledExtensionCount    (requiredDeviceExtensionsNum)				//Set required extentions count
			.setPpEnabledExtensionNames  (requiredDeviceExtensions)					//Set required extensions
			.setPEnabledFeatures         (&enabledFeatures)							//Set physical device enabled features
		;


		//Create the logical device and save its queues
		vk::Device _logicalDevice;
		switch(pPDev.device.createDevice(&deviceCreateInfo, nullptr, &_logicalDevice)){
			case vk::Result::eErrorInitializationFailed: dbg::logError("Initialization failed"); break;
			case vk::Result::eErrorExtensionNotPresent:  dbg::logError("Extension not present"); break;
			case vk::Result::eErrorFeatureNotPresent:    dbg::logError("Feature not present");   break;
			case vk::Result::eErrorTooManyObjects:       dbg::logError("Too many objects");      break;
			case vk::Result::eErrorDeviceLost:           dbg::logError("Device lost");           break;
			vkDefaultCases;
		}


		pDev.ld = _logicalDevice;												//Set logical device
		if(sameDevice(pPDev, graphics.pd)) {										//If the device is the main graphics device
			_logicalDevice.getQueue(pPDev.indices.graphicsFamily, 0, &pDev.gq);		//Set graphics queue index
			_logicalDevice.getQueue(pPDev.indices.presentFamily , 0, &pDev.pq);		//Set present  queue index
		}
		for(const auto cqIndex : pPDev.indices.computeFamilies) {				//Add every compute queue index
			vk::Queue cq;
			_logicalDevice.getQueue(cqIndex, 0, &cq);
			pDev.cqs.add(cq);
		}
	}
}
//TODO USE TRANSFER QUEUES WHEN AVAILABLE
//TODO compute queues otherwise