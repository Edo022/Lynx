#include "Render.h"

//Compares 2 _VkPhysicalDevice objects
#define sameDevice(a,b) (a.properties.deviceID == b.properties.deviceID)







//Returns the rating of a physical device
static int ratePhysicalDevice(_VkPhysicalDevice& device) {
	uint32_t score = 0;
	if (device.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) score += 1000000;	//Discrete GPUs have performance advantage
	score += device.properties.limits.maxImageDimension2D;										//Maximum possible size of textures affects graphics quality
	if (device.features.geometryShader) score += 1;												//Geometry shaders needed
	return score;
}




//Returns the queue families of a physical device
QueueFamilyIndices Render::findQueueFamilies(VkPhysicalDevice device) {
	//Get queue families
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);						//Enumerate queue families
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());			//Save queue families

	//Set families
	int i = 0;
	QueueFamilyIndices indices;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) indices.graphicsFamily = i;					//Set graphics family
		if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) indices.computeFamilies.push_back(i);		//Add compute families
		VkBool32 hasPresentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &hasPresentSupport);					//Set present family
		if (hasPresentSupport) indices.presentFamily = i;
		i++;
	}

	return indices;
}




//Returns true if the device supports the extensions, false if not
bool Render::checkDeviceExtensionSupport(VkPhysicalDevice device) {
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(requiredDeviceExtensions.begin(), requiredDeviceExtensions.end());
	for (const auto& extension : availableExtensions) requiredExtensions.erase(extension.extensionName);
	return requiredExtensions.empty();
}


//Returns true if the device is suitable false if not
bool Render::isDeviceSuitable(VkPhysicalDevice device, std::string errorText) {
	//Check extensions
	if (!checkDeviceExtensionSupport(device)) {
		errorText = "Missing required extensions";
		return false;
	}

	//Check swapchain
	else {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		if (swapChainSupport.formats.empty() || swapChainSupport.presentModes.empty()) {
			errorText = "Unsupported swapchain";
			return false;
		}
	}

	//Check features
	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);
	if (!supportedFeatures.samplerAnisotropy) { //TODO just don't use it if it's not supported //samplerInfo.anisotropyEnable = VK_FALSE; samplerInfo.maxAnisotropy = 1;
		errorText = "Missing required device features";
		return false;
	}

	return true;
}








//-----------------------------------------------------------------------------------------------------------------------------//








//Find all suitable physical devices, choosing the main and secondary devices according to their capabilities
void Render::getPhysicalDevices() {
	uint32_t deviceCount = 0;
	std::vector<std::string> discardedPhysicalDevices;
	std::vector<_VkPhysicalDevice> physicalDevices;


	//Get physical devices
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	if (deviceCount == 0) Quit("Failed to find GPUs with Vulkan support")
	else {
		//Get physical devices
		std::vector<VkPhysicalDevice> physDevices(deviceCount);											//Get physical device count
		vkEnumeratePhysicalDevices(instance, &deviceCount, physDevices.data());							//Get physical devices

		for (auto& physDev : physDevices) {																//For every physical device, create and save a _VkPhysicalDevice stucture
			VkPhysicalDeviceProperties properties;	vkGetPhysicalDeviceProperties(physDev, &properties);
			VkPhysicalDeviceFeatures features;		vkGetPhysicalDeviceFeatures(physDev, &features);
			std::string errorText;
			if (isDeviceSuitable(physDev, errorText)) {													//If it's suitable
				physicalDevices.push_back(_VkPhysicalDevice(physDev, properties, features, *new QueueFamilyIndices)); //Add it to the physical devices vector
			}
			else {																						//If not
				discardedPhysicalDevices.push_back(properties.deviceName);									//Add it to the discarded devices vector
				discardedPhysicalDevices.push_back(errorText);												//And save the reason of its unsuitability
			}
		}
	}


	//If there are discarded devices, print their names
	if (discardedPhysicalDevices.size() > 0) {
		Failure printf("    Discarded devices:");
		for (int i = 0; i < discardedPhysicalDevices.size(); i += 2) {
			Failure printf("        %s\t|  %s", discardedPhysicalDevices[i].data(), discardedPhysicalDevices[(int64_t)i + 1].data());
		}
	}


	//TODO different score for graphics and compute
	if (physicalDevices.size() > 0) {									//If there are suitable devices
		graphics.PD = physicalDevices[0];									//set graphics device at default value
		compute.PD = physicalDevices[0];									//set compute  device at default value
		for (auto& physDevice : physicalDevices) {							//For every physical device
			physDevice.indices = findQueueFamilies(physDevice.device);			//Get its queue families
			physDevice.score = ratePhysicalDevice(physDevice);					//And its score. Then check if it has the necessary queues and set it as the main graphics and or compute physical device
			if (physDevice.score > graphics.PD.score || physDevice.indices.graphicsFamily != -1) graphics.PD = physDevice;
			if (physDevice.score > compute.PD.score || physDevice.indices.computeFamilies.size() > 0) compute.PD = physDevice;
		}
		for (auto& physDevice : physicalDevices) {							//For every physical device that isn't the main graphics or compute device
			if (!sameDevice(physDevice, graphics.PD) && !sameDevice(physDevice, compute.PD)) {
				secondary.resize(secondary.size() + 1);
				secondary[secondary.size() - 1].PD = physDevice;				//Add it to the secondary devices vector (it'll be used as a compute device with less priority. T.T poor gpu)
			}
		}

		//Print the devices names, IDs, scores and tasks
		Success printf("    Found %lld suitable device%s:", physicalDevices.size(), (physicalDevices.size() == 1) ? "" : "s");
		for (auto& physDevice : physicalDevices) {
			if (sameDevice(physDevice, graphics.PD) || sameDevice(physDevice, compute.PD)) Main else Normal;
			printf("        %s  |  ID: %d  |  %d", physDevice.properties.deviceName, physDevice.properties.deviceID, physDevice.score);
			if (sameDevice(physDevice, graphics.PD)) printf("  |  Main graphics");
			if (sameDevice(physDevice, compute.PD)) printf("  |  Main compute");
		}
	}
	else Quit("Failed to find a suitable GPU");




	//Logical devices
	//Create a logical device for graphics, one for computation and one for every secondary device
	createLogicalDevice(&graphics.PD, &graphics.LD, &graphics.graphicsQueue, &graphics.presentQueue, nullptr);
	createLogicalDevice(&compute.PD, &compute.LD, nullptr, nullptr, &compute.computeQueues);
	for (int i = 0; i < secondary.size(); i++) {
		createLogicalDevice(&secondary[i].PD, &secondary[i].LD, nullptr, nullptr, &secondary[i].computeQueues);
	}

	//Output created logical devices and queues
	Success printf("    Created %lld logical devices:", 2 + secondary.size());
	Main	printf("        Main graphics  |  graphics queues: 1  |  present queues:  1");
	Main	printf("        Main compute   |  compute queues:  %lld", compute.computeQueues.size());
	Normal	printf("        %lld secondary devices",/*  |  secondary compute queues: %lld", secondary.size(), */secondary.size());
}














void Render::createLogicalDevice(_VkPhysicalDevice* PD, VkDevice* LD, VkQueue* graphicsQueue, VkQueue* presentQueue, std::vector<VkQueue>* computeQueues) {
	//List unique device's queues
	std::set<int32_t> uniqueQueueFamilyIndices;
	if (sameDevice((*PD), graphics.PD)) {													//If it's the main device for graphics,
		uniqueQueueFamilyIndices.insert(PD->indices.graphicsFamily);						//Add his graphics family
		uniqueQueueFamilyIndices.insert(PD->indices.presentFamily);							//And his present family
	}
	for (auto deviceQueueFamilyIndex : PD->indices.computeFamilies) {						//And then add every compute family, graphic ones included
		uniqueQueueFamilyIndices.insert(deviceQueueFamilyIndex);
	}


	//Fill deviceCreateInfo structure for logical device creation
	VkDeviceCreateInfo deviceCreateInfo{};

	//Queue infos
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	for (auto queueFamilyIndex : uniqueQueueFamilyIndices) {							//For every device queue family index found
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamilyIndex;								//Set index
		queueCreateInfo.queueCount = 1;														//Set count		// ↓ Set priority. 1 for main devices, 0.5 for secondary ones
		queueCreateInfo.pQueuePriorities = new float((sameDevice((*PD), graphics.PD) || sameDevice((*PD), compute.PD)) ? 1.0f : 0.5f);
		queueCreateInfos.push_back(queueCreateInfo);										//Add to queue create info vector
	}

	//Required extensions
	VkPhysicalDeviceFeatures enabledDeviceFeatures{};
	enabledDeviceFeatures.samplerAnisotropy = VK_TRUE;
	enabledDeviceFeatures.multiViewport = VK_TRUE;		//Multiple viewports
	enabledDeviceFeatures.fillModeNonSolid = VK_TRUE;	//Point and line render

	//Fill deviceCreateInfo
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = (int32_t)queueCreateInfos.size();			//Set queue infos count
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();						//Set queue infos
	deviceCreateInfo.enabledExtensionCount = (int32_t)requiredDeviceExtensions.size();	//Set required extentions count
	deviceCreateInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();			//Set required extensions
	deviceCreateInfo.pEnabledFeatures = &enabledDeviceFeatures;							//Set physical device enabled features
	#ifndef NDEBUG																		//If in debug mode
	deviceCreateInfo.enabledLayerCount = (int32_t)validationLayers.size();					//Set validation layers count
	deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();							//Set validation layers
	#else																				//Else
	deviceCreateInfo.enabledLayerCount = 0;													//Disable validation layers
	#endif


	//Create the logical device and save its queues, exit if an error occurs
	VkDevice _logicalDevice;
	if (vkCreateDevice(PD->device, &deviceCreateInfo, nullptr, &_logicalDevice) == VK_SUCCESS) {
		if (sameDevice((*PD), graphics.PD) || sameDevice((*PD), compute.PD)) {
			if (sameDevice((*PD), graphics.PD)) {														//If it's the main graphics device
				graphics.LD = _logicalDevice;																//Set it as the main graphics logical device
				vkGetDeviceQueue(_logicalDevice, PD->indices.graphicsFamily, 0, &graphics.graphicsQueue);	//Set graphics queue
				vkGetDeviceQueue(_logicalDevice, PD->indices.presentFamily, 0, &graphics.presentQueue);		//Set present queue
			}
			if (computeQueues != nullptr) {																//If it's the main compute device and the function was called to create his logical device
				compute.LD = _logicalDevice;																//Set it as the main compute logical device
				for (auto deviceComputeFamily : PD->indices.computeFamilies) {								//Add every compute queue to the main compute queue list
					VkQueue computeQueue;
					vkGetDeviceQueue(_logicalDevice, deviceComputeFamily, 0, &computeQueue);
					compute.computeQueues.push_back(computeQueue);
				}
			}
		}
		else {																							//If it's none of them
			*LD = _logicalDevice;																			//Add it to the list of secondary logical devices
			for (auto deviceComputeFamily : PD->indices.computeFamilies) {									//Add every compute queue to the secondary compute queues
				VkQueue computeQueue;
				vkGetDeviceQueue(_logicalDevice, deviceComputeFamily, 0, &computeQueue);
				computeQueues->push_back(computeQueue);
			}
		}
	}
	else Quit("Failed to create logical device");
}
