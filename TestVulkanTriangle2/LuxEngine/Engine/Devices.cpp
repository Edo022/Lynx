#include "LuxEngine/Engine/Engine.h"

//Compares 2 _VkPhysicalDevice objects
#define sameDevice(a,b) (a.properties.deviceID == b.properties.deviceID)







//Returns the rating of a physical device
static int32 ratePhysicalDevice(_VkPhysicalDevice& device) {
	uint32 score = 0;																			//Device performance evalutation
	if (device.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) score += 1000000;	//Discrete GPUs have performance advantage
	device.properties.limits.maxComputeSharedMemorySize; //TODO non superare il limite della shader
	score += device.properties.limits.maxImageDimension2D;										//Maximum possible size of textures affects graphics quality
	if (device.features.geometryShader) score += 1;												//Geometry shaders needed
	return score;
}




//Returns the queue families of a physical device
QueueFamilyIndices Engine::findQueueFamilies(VkPhysicalDevice device) {
	uint32 queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);						//Enumerate queue families
	LuxArray<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());			//Save queue families

	//Set families
	QueueFamilyIndices indices;
	for (int i = 0; i < queueFamilies.size(); i++) {													//For every queue family
		if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) indices.graphicsFamily = i;				//Set graphics family
		if (queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT) indices.computeFamilies.add(i);				//Add compute families
		VkBool32 hasPresentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &hasPresentSupport);						//Set present family
		if (hasPresentSupport) indices.presentFamily = i;
	}
	return indices;
}




//Returns true if the device supports the extensions, false if not
bool Engine::checkDeviceExtensionSupport(VkPhysicalDevice device) {
	uint32 extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);					//Get extension count
	LuxArray<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());	//Get extensions

	std::set<std::string> requiredExtensions(requiredDeviceExtensions.begin(), requiredDeviceExtensions.end());
	for (const auto& extension : availableExtensions) requiredExtensions.erase(extension.extensionName);//Search for required extensions
	return requiredExtensions.empty();
}




//Returns true if the device is suitable false if not
bool Engine::isDeviceSuitable(VkPhysicalDevice device, std::string errorText) {
	//Check extensions
	if (!checkDeviceExtensionSupport(device)) {
		errorText = "Missing required extensions";
		return false;
	}

	//Check swapchain
	else {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		if (swapChainSupport.formats.size() == 0 || swapChainSupport.presentModes.size() == 0) {
			errorText = "Unsupported swapchain";
			return false;
		}
	}
	return true;
}








//-----------------------------------------------------------------------------------------------------------------------------//








//Find all suitable physical devices, choosing the main and secondary devices according to their capabilities
void Engine::getPhysicalDevices() {
	uint32 deviceCount = 0;
	LuxMap<std::string> discardedPhysicalDevices(0xFFFF, 0xFFFF);
	LuxMap<_VkPhysicalDevice*> physicalDevices(0xFFFF, 0xFFFF);


	//Get physical devices
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	if (deviceCount == 0) Exit("Failed to find GPUs with Vulkan support")
	else {
		//Get physical devices
		LuxArray<VkPhysicalDevice> physDevices(deviceCount);								//Get physical device count
		vkEnumeratePhysicalDevices(instance, &deviceCount, physDevices.data());					//Get physical devices

		forEach(physDevices, i) {																//For every physical device, create and save a _VkPhysicalDevice stucture
			VkPhysicalDeviceProperties properties;	vkGetPhysicalDeviceProperties(physDevices[i], &properties);
			VkPhysicalDeviceFeatures features;		vkGetPhysicalDeviceFeatures(physDevices[i], &features);
			std::string errorText;
			if (isDeviceSuitable(physDevices[i], errorText)) {										//If it's suitable
				physicalDevices.add(new _VkPhysicalDevice(physDevices[i], properties, features, *new QueueFamilyIndices)); //Add it to the physical devices vector
			}
			else {																						//If not
				discardedPhysicalDevices.add(properties.deviceName);									//Add it to the discarded devices vector
				discardedPhysicalDevices.add(errorText);												//And save the reason of its unsuitability
			}
		}
	}


	//If there are discarded devices, print their names
	if (discardedPhysicalDevices.size() > 0) {
		Failure printf("    Discarded devices:");
		for (int32 i = 0; i < discardedPhysicalDevices.size(); i += 2) {
			Failure printf("        %s\t|  %s", discardedPhysicalDevices[i].data(), discardedPhysicalDevices[(int64)i + 1].data());
		}
	}

	//TODO different score for graphics and compute
	#define physDev (*physicalDevices[i])
	if (physicalDevices.size() > 0) {								//If there are suitable devices
		graphics.PD = *physicalDevices[0];								//set graphics device at default value
		compute.PD = *physicalDevices[0];								//set compute  device at default value
		forEach(physicalDevices, i) {									//For every physical device
			physDev.indices = findQueueFamilies(physDev.device);			//Get its queue families
			physDev.score = ratePhysicalDevice(physDev);					//And its score. Then check if it has the necessary queues and set it as the main graphics and or compute physical device
			if (physDev.score > graphics.PD.score || physDev.indices.graphicsFamily != -1) graphics.PD = physDev;
			if (physDev.score > compute.PD.score || physDev.indices.computeFamilies.size() > 0) compute.PD = physDev;
		}
		forEach(physicalDevices, i) {									//For every physical device that isn't the main graphics or compute device
			if (!sameDevice(physDev, graphics.PD) && !sameDevice(physDev, compute.PD)) {
				secondary.resize(secondary.size() + 1);
				secondary[secondary.size() - 1].PD = physDev;				//Add it to the secondary devices vector (it'll be used as a compute device with less priority. T.T poor gpu)
			}
		}

		//Print the devices names, IDs, scores and tasks
		Success printf("    Found %lld suitable device%s:", physicalDevices.size(), (physicalDevices.size() == 1) ? "" : "s");
		forEach(physicalDevices, i) {
			if (sameDevice(physDev, graphics.PD) || sameDevice(physDev, compute.PD)) Main else Normal;
			printf("        %s  |  ID: %d  |  %d", physDev.properties.deviceName, physDev.properties.deviceID, physDev.score);
			if (sameDevice(physDev, graphics.PD)) printf("  |  Main graphics");
			if (sameDevice(physDev, compute.PD)) printf("  |  Main compute");
		}
	}
	else Exit("Failed to find a suitable GPU");




	//Logical devices
	//Create a logical device for graphics, one for computation and one for every secondary device
	createLogicalDevice(&graphics.PD, &graphics.LD, &graphics.graphicsQueue, &graphics.presentQueue, nullptr);
	createLogicalDevice(&compute.PD, &compute.LD, nullptr, nullptr, &compute.computeQueues);
	for (int32 i = 0; i < secondary.size(); i++) {
		createLogicalDevice(&secondary[i].PD, &secondary[i].LD, nullptr, nullptr, &secondary[i].computeQueues);
	}

	//Output created logical devices and queues
	Success printf("    Created %lld logical devices:", 2 + secondary.size());
	Main	printf("        Main graphics  |  graphics queues: 1  |  present queues:  1");
	Main	printf("        Main compute   |  compute queues:  %lld", compute.computeQueues.size());
	Normal	printf("        %lld secondary devices",/*  |  secondary compute queues: %lld", secondary.size, */secondary.size());
}














void Engine::createLogicalDevice(_VkPhysicalDevice* PD, VkDevice* LD, VkQueue* graphicsQueue, VkQueue* presentQueue, LuxMap<VkQueue>* computeQueues) {
	//List unique device's queues
	std::set<int32> uniqueQueueFamilyIndices;
	if (sameDevice((*PD), graphics.PD)) {												//If it's the main device for graphics,
		uniqueQueueFamilyIndices.insert(PD->indices.graphicsFamily);					//Add his graphics family
		uniqueQueueFamilyIndices.insert(PD->indices.presentFamily);						//And his present family
	}
	forEach(PD->indices.computeFamilies, i) {											//And then add every compute family, graphics ones included
		uniqueQueueFamilyIndices.insert(PD->indices.computeFamilies[i]);
	}




	//Queue infos
	LuxMap<VkDeviceQueueCreateInfo> queueCreateInfos;								//Create a queue create info array
	for (auto queueFamilyIndex : uniqueQueueFamilyIndices) {							//For every device queue family index found
		VkDeviceQueueCreateInfo queueCreateInfo{};											//Create a queue create info struct
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;					//Set structure type
		queueCreateInfo.queueFamilyIndex = queueFamilyIndex;								//Set index
		queueCreateInfo.queueCount = 1;														//Set count		// ↓ Set priority. 1 for main devices, 0.5 for secondary ones
		queueCreateInfo.pQueuePriorities = new float((sameDevice((*PD), graphics.PD) || sameDevice((*PD), compute.PD)) ? 1.0f : 0.5f);
		queueCreateInfos.add(queueCreateInfo);											//Add it to the queue create info array
	}

	//Required extensions
	VkPhysicalDeviceFeatures enabledDeviceFeatures{};								//Set enabled features
	enabledDeviceFeatures.samplerAnisotropy = VK_FALSE;									//No anistropy filter
	enabledDeviceFeatures.multiViewport = VK_FALSE;										//No multiple viewports
	enabledDeviceFeatures.fillModeNonSolid = VK_FALSE;									//No point32 and line render, since we don't use meshes

	//Fill deviceCreateInfo
	VkDeviceCreateInfo deviceCreateInfo{};											//Create deviceCreateInfo structure for logical device creation
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;						//Set structure type
	deviceCreateInfo.queueCreateInfoCount = (int32)queueCreateInfos.size();				//Set queue infos count
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();						//Set queue infos
	deviceCreateInfo.enabledExtensionCount = (int32)requiredDeviceExtensions.size();	//Set required extentions count
	deviceCreateInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();			//Set required extensions
	deviceCreateInfo.pEnabledFeatures = &enabledDeviceFeatures;							//Set physical device enabled features

	luxDebug(deviceCreateInfo.enabledLayerCount = (int32)validationLayers.size();)		//Set validation layers count if in debug mode
	luxDebug(deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();)			//Set validation layers if in debug mode
	luxRelease(deviceCreateInfo.enabledLayerCount = 0;)									//Disable validation layers if in release mode



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
				forEach(PD->indices.computeFamilies, i) {													//Add every compute queue to the main compute queue list
					VkQueue computeQueue;
					vkGetDeviceQueue(_logicalDevice, PD->indices.computeFamilies[i], 0, &computeQueue);
					compute.computeQueues.add(computeQueue);
				}
			}
		}
		else {																							//If it's none of them
			*LD = _logicalDevice;																			//Add it to the list of secondary logical devices
			forEach(PD->indices.computeFamilies, i) {														//Add every compute queue to the secondary compute queues
				VkQueue computeQueue;
				vkGetDeviceQueue(_logicalDevice, PD->indices.computeFamilies[i], 0, &computeQueue);
				computeQueues->add(computeQueue);
			}
		}
	}
	else Exit("Failed to create logical device");
}
