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
				case vk::Result::eIncomplete: dbg::printError("Incomplete properties"); break;
				vkDefaultCases;
			};

			//Get layers
			RtArray<vk::LayerProperties> availableLayers(layerCount);
			switch(vk::enumerateInstanceLayerProperties(&layerCount, availableLayers.begin())){
				case vk::Result::eIncomplete: dbg::printError("Incomplete properties"); break;
				vkDefaultCases;
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

		switch(vk::createInstance(&createInfo, nullptr, &core::dvc::instance)){
			case vk::Result::eErrorInitializationFailed: dbg::printError("Initialization failed"); break;
			case vk::Result::eErrorLayerNotPresent:      dbg::printError("Layer not present");     break;
			case vk::Result::eErrorExtensionNotPresent:  dbg::printError("Extension not present"); break;
			case vk::Result::eErrorIncompatibleDriver:   dbg::printError("Incompatible driver");   break;
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
			case VkResult::VK_ERROR_INITIALIZATION_FAILED: dbg::printError("Initialization failed"); break;
			case VkResult::VK_ERROR_EXTENSION_NOT_PRESENT: dbg::printError("Extension not present"); break;
			default: _dbg(dbg::printError("Unknown result")) _rls(noop);
		}
		chooseDriver();
		getPhysical();
	}








	//-----------------------------------------------------------------------------------------------------------------------------//








	/**
	 * @brief Rates a physical device based on its properties and features
	 * @param pPDevice: The device structure where its infos are stored
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
	 * @param vDevice The physical device to check
	 * @param pErrorText A pointer to a lnx::String where to store the error in case the device is not suitable
	 * @return return True if the device is suitable, false if not
	 */
	bool isSuitable(const vk::PhysicalDevice vDevice, String& pErrorText) {
		//Discard llvmpipe devices
		String name = vDevice.getProperties().deviceName.cbegin();
		for (char* c = name.begin(); *c; ++c) *c = tolower(*c); //TODO ADD TOLOWER FUNCTIONS TO LUX STRING
		if(strstr(name.begin(), "llvm")){
			pErrorText = "Device is not stable";
			return false;
		}

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
				vkDefaultCases;
			};

			switch(vDevice.getSurfacePresentModesKHR(dummySurface, &presentModesCount,   nullptr)){
				case vk::Result::eIncomplete:             dbg::printError("Incomplete surface formats"); break;
				case vk::Result::eErrorSurfaceLostKHR:    dbg::printError("Surface lost");               break;
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
	 * @brief Returns true if the device supports the extensions, false if not
	 */
	bool checkExtensions(const vk::PhysicalDevice vDevice) {
		uint32 extensionCount;

		//Get extension count
		switch(vDevice.enumerateDeviceExtensionProperties(nullptr, &extensionCount, nullptr)){
			case vk::Result::eIncomplete:           dbg::printError("Incomplete extensions"); break;
			case vk::Result::eErrorLayerNotPresent: dbg::printError("Layer not present");     break;
			vkDefaultCases;
		};

		//Get extensions
		RtArray<vk::ExtensionProperties> availableExtensions(extensionCount);
		switch(vDevice.enumerateDeviceExtensionProperties(nullptr, &extensionCount, availableExtensions.begin())){
			case vk::Result::eIncomplete:           dbg::printError("Incomplete extensions"); break;
			case vk::Result::eErrorLayerNotPresent: dbg::printError("Layer not present");     break;
			vkDefaultCases;
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
			switch(vDevice.getSurfaceSupportKHR(i, dummySurface, &hasPresentSupport)){						//Set present family
				case vk::Result::eErrorSurfaceLostKHR: dbg::printError("Surface lost"); break;
				vkDefaultCases;
			};


			if(hasPresentSupport) indices.presentFamily = i;
		}
		return indices;
	}








	bool checkDriver(const char* vDriverName){
		uint32 c;
		putenv((String("VK_ICD_FILENAMES=" "/usr/share/vulkan/icd.d/") + vDriverName).begin());
		switch(instance.enumeratePhysicalDevices(&c, nullptr)){
			case vk::Result::eIncomplete: dbg::printError("Incomplete devices"); break;
			case vk::Result::eErrorInitializationFailed: return false;
			vkDefaultCases;
		};
		return true;
	}




	void chooseDriver(){
		uint32 c;
		if     (checkDriver("radeon_icd.x86_64.json")) return;
		else if(checkDriver("nvidia_icd.x86_64.json")) return;
		else if(checkDriver( "intel_icd.x86_64.json")) return;
		else unsetenv("VK_ICD_FILENAMES");
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

		// chooseDriver();

		//Get physical device count
		switch(instance.enumeratePhysicalDevices(&deviceCount, nullptr)){
			case vk::Result::eIncomplete:                dbg::printError("Incomplete devices");    break;
			case vk::Result::eErrorInitializationFailed: dbg::printError("Initialization failed"); break;
			vkDefaultCases;
		};
		printf("\nDEVICES: %d", deviceCount);
		if(deviceCount == 0) dbg::printError("Failed to find GPUs with Vulkan support");	//Check if there is at least one deice that supports vulkan //FIXME add runtime support

		//Get physical devices
		RtArray<vk::PhysicalDevice> physDevices(deviceCount);
		switch(instance.enumeratePhysicalDevices(&deviceCount, physDevices.begin())){
			case vk::Result::eIncomplete:                dbg::printError("Incomplete devices");    break;
			case vk::Result::eErrorInitializationFailed: dbg::printError("Initialization failed"); break;
			vkDefaultCases;
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
		// printf("\nDEVICE 1: %s", physDevices[0].getProperties().deviceName.cbegin());
		// printf("\nDEVICE 2: %s", physDevices[1].getProperties().deviceName.cbegin());


		//If there are discarded devices, print their names
		if(discardedPhysicalDevices.count() > 0) {
			Failure printf("    Discarded devices:");
			for(uint32 i = 0; i < discardedPhysicalDevices.count(); i += 2) {
				Failure printf("        %s\t|  %s", (char*)discardedPhysicalDevices[i].begin(), (char*)discardedPhysicalDevices[(int64)i + 1].begin());
			}
		}
// exit(0);

		//TODO different score for graphics and compute
		#define physDev (*physicalDevices[i])
		if(physicalDevices.count() > 0) {									//If there are suitable devices
			// graphics.pd = compute.pd = *physicalDevices[0];						//set graphics device at default value
			graphics.pd = *physicalDevices[0];						//set graphics device at default value
			for(uint32 i = 0; i < physicalDevices.count(); ++i) {				//For every physical device
				physDev.indices = getQueueFamilies(physDev.device);					//Get its queue families
				physDev.score = rate(physDev);										//And its score. Then check if it has the necessary queues and set it as the main graphics and or compute physical device
				if(physDev.score > graphics.pd.score || physDev.indices.graphicsFamily != (uint32)-1)        graphics.pd = physDev;
				// if(physDev.score > compute .pd.score || physDev.indices.computeFamilies.count() > 0) compute.pd = physDev;
			}
			for(uint32 i = 0; i < physicalDevices.count(); ++i) {				//For every physical device that isn't the main graphics or compute device
				// if(!sameDevice(physDev, graphics.pd) && !sameDevice(physDev, compute.pd)) {
				if(!sameDevice(physDev, graphics.pd)) {
					secondary.resize(secondary.count() + 1);
					secondary[secondary.count() - 1].pd = physDev;					//Add it to the secondary devices vector (it'll be used as a compute device with less priority. T.T poor gpu)
				}
			}

			//Print the devices names, IDs, scores and tasks
			Success printf("    Found %d suitable device%s:", physicalDevices.count(), (physicalDevices.count() == 1) ? "" : "s");
			for(uint32 i = 0; i < physicalDevices.count(); ++i) {
				// if(sameDevice(physDev, graphics.pd) || sameDevice(physDev, compute.pd)) Main else Normal;
				if(sameDevice(physDev, graphics.pd)) Main else Normal;
				printf("        %s  |  ID: %d  |  %d", physDev.properties.deviceName.cbegin(), physDev.properties.deviceID, physDev.score);
				if(sameDevice(physDev, graphics.pd)) printf("  |  Main graphics");
				// if(sameDevice(physDev, compute.pd))  printf("  |  Main compute");
			}
		}
		//FIXME add runtime support
		else dbg::printError("Failed to find a suitable GPU");
		#undef physDev


		createLogical(graphics.pd, graphics);
		for(uint32 i = 0; i < secondary.count(); ++i) {
			createLogical(secondary[i].pd, secondary[i]);
		}

		//Output created logical devices and queues
		Success printf("    Created %d logical devices:", 2 + secondary.count());
		Main	printf("        Main graphics  |  graphics queues: 1  |  present queues:  1");
		Normal	printf("        %d secondary devices",/*  |  secondary compute queues: %lld", secondary.count, */secondary.count());
	}








	/**
	 * @brief Creates a logical device
	 * @param pPD
	 * @param pDevice
	 */
	void createLogical(const _VkPhysicalDevice& pPDevice, Device& pDevice) {
		std::set<uint32> queues;
		if(sameDevice(pPDevice, graphics.pd)) {									//If it's the main device for graphics
			queues.insert(pPDevice.indices.graphicsFamily);							//Add its graphics queue index
			queues.insert(pPDevice.indices.presentFamily);							//Add its present  queue index
		}
		for(uint32 i = 0; i < pPDevice.indices.computeFamilies.count(); ++i) {	//Add every compute index
			queues.insert(pPDevice.indices.computeFamilies[i]);
		}


		RtArray<vk::DeviceQueueCreateInfo, uint32> queueInfos;				//Create queues
		for(auto queue : queues) {											//For every queue index found
			queueInfos.add(vk::DeviceQueueCreateInfo()							//Create a queue create info struct
				.setQueueFamilyIndex (queue)										//Set index
				.setQueueCount       (1)											//Set count		// ↓ Set priority. 1 for main devices, 0.5 for secondary ones
				.setPQueuePriorities (new float((sameDevice(pPDevice, graphics.pd)) ? 1.0f : 0.5f)) //FIXME
			);
		}


		auto enabledFeatures = vk::PhysicalDeviceFeatures() 				//Set enabled features
			.setFillModeNonSolid  (VK_FALSE)									//No point32 and line render, since we don't use meshes
			.setMultiViewport     (VK_FALSE)									//No multiple viewports
			.setSamplerAnisotropy (VK_FALSE)									//No anistropy filter
		;

		auto deviceCreateInfo = vk::DeviceCreateInfo() 						//Create logical device
			.setQueueCreateInfoCount     (queueInfos.count())					//Set queue infos count
			.setPQueueCreateInfos        (queueInfos.begin())					//Set queue infos
			.setEnabledLayerCount        (_dbg(validationLayersNum) _rls(0))	//  Set validation layer count if in debug mode
			.setPpEnabledLayerNames      (_dbg(validationLayers)    _rls(nullptr))//Set validation layers      if in debug mode
			.setEnabledExtensionCount    (requiredDeviceExtensionsNum)			//Set required extentions count
			.setPpEnabledExtensionNames  (requiredDeviceExtensions)				//Set required extensions
			.setPEnabledFeatures         (&enabledFeatures)						//Set physical device enabled features
		;


		//Create the logical device and save its queues
		vk::Device _logicalDevice;
		switch(pPDevice.device.createDevice(&deviceCreateInfo, nullptr, &_logicalDevice)){
			case vk::Result::eErrorInitializationFailed: dbg::printError("Initialization failed"); break;
			case vk::Result::eErrorExtensionNotPresent:  dbg::printError("Extension not present"); break;
			case vk::Result::eErrorFeatureNotPresent:    dbg::printError("Feature not present");   break;
			case vk::Result::eErrorTooManyObjects:       dbg::printError("Too many objects");      break;
			case vk::Result::eErrorDeviceLost:           dbg::printError("Device lost");           break;
			vkDefaultCases;
		}


		pDevice.ld = _logicalDevice;												//Set logical device
		if(sameDevice(pPDevice, graphics.pd)) {										//If the device is the main graphics device
			_logicalDevice.getQueue(pPDevice.indices.graphicsFamily, 0, &pDevice.gq);	//Set graphics queue index
			_logicalDevice.getQueue(pPDevice.indices.presentFamily , 0, &pDevice.pq);	//Set present  queue index
		}
		for(uint32 i = 0; i < pPDevice.indices.computeFamilies.count(); ++i) {		//Add every compute queue index
			vk::Queue cq;
			_logicalDevice.getQueue(pPDevice.indices.computeFamilies[i], 0, &cq);
			pDevice.cqs.add(cq);
		}
	}
}