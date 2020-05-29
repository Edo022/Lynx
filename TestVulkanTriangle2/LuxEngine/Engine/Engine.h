#pragma once

//Disabled useless warnings
#pragma warning( disable : 26812 )			//Prefer enum class to enum
#pragma warning( disable : 26495 )			//Uninitialized variables
#pragma warning( disable : 6237  )			//Always true enableValidationLayers condition 
#pragma warning( disable : 26451 )			//stb arithmetic overflow

//Third party libraries
#include <vulkan/vulkan.h>					//Graphics
#include <GLFW/glfw3.h>						//Window system

#define GLM_FORCE_RADIANS					//Use radiants intead of degrees
#define GLM_FORCE_DEPTH_ZERO_TO_ONE			//0 to 1 depth instead of OpenGL -1 to 1
#include <glm/glm.hpp>						//Shader compatible geometry
#include <glm/gtc/matrix_transform.hpp>		//GPU geometry transformations

#include <stb_image.h>						//PNG and JPG importer
#include <tiny_obj_loader.h> 				//OBJ importer 

//Default C++
#include <iostream>
#include <fstream>
#include <array>
#include <chrono>
#include <vector>
#include <set>
#include <thread>
#include <queue>


#include <cmath>
#include <assert.h>
#include <stdexcept>
#include <string>

//Junk
#include "LuxEngine/macros.h"				//Useful useless macros
#include <TermColor.hpp>					//Colored console output


//Structures
#include "Render/Structs/_VkPhysicalDevice.h"
#include "Render/Structs/Vertex.h"
#include "LuxEngine/Object/Object.h"
#include "LuxEngine/Types/Integers/Integers.h"

#include "LuxEngine/Types/Containers/LuxArray.h"

//Re enable warnings for this header
#pragma warning( default : 26451 )			//Arithmetic overflow











//Debug. Idk how, but it works
static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	else return VK_ERROR_EXTENSION_NOT_PRESENT;
}
static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) func(instance, debugMessenger, pAllocator);
}

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};






#define findDepthFormat()																	\
	findSupportedFormat(																	\
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },\
		VK_IMAGE_TILING_OPTIMAL,															\
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT										\
	);



//Validation layers in debug mode
#ifdef NDEBUG	
const bool enableValidationLayers = false;
#else			
const bool enableValidationLayers = true;
#endif






class Render {
public:
	double FPS = 0;
	float FOV;
	bool running;
	bool useVSync;
	bool initialized = false;

	void run(bool _useVSync = true, float FOV = 45.0f);


	bool sc = false;



	//void checkObjects() {
	//	for (int i = 0; i < ObjectsQueue.size(); i++) {
	//		if (ObjectsQueue.isValid(i)) {
	//			createVertexBuffer(ObjectsQueue[i]);
	//			createIndexBuffer(ObjectsQueue[i]);
	//			objects.add(ObjectsQueue[i]);
	//			ObjectsQueue.remove(i);

	//			if (sc) createDrawCommandBuffers();
	//			else {
	//				sc = true;
	//				break;
	//			}
	//		}
	//	}
	//}
private:

	//Main
	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkSurfaceKHR surface;

	//Windows
	GLFWwindow* window;								//Main engine's window
	bool framebufferResized = false;				//Updates the swapchain when the window is resized
	const uint32 WIDTH = 1920, HEIGHT = 1080;		//Default size in windowed mode
	//const uint32 WIDTH = 800, HEIGHT = 600;		//Default size in windowed mode




	//COMPUTE 
	const int COMPUTE_WIDTH = WIDTH; // Size of rendered mandelbrot set.
	const int COMPUTE_HEIGHT = HEIGHT; // Size of renderered mandelbrot set.
	const int WORKGROUP_SIZE = 32; // Workgroup size in compute shader.



	const char* VERT_PATH = "LuxEngine/Contents/Shaders/vert.spv";
	const char* FRAG_PATH = "LuxEngine/Contents/Shaders/frag.spv";
	const std::string MODEL_PATH = "LuxEngine/Contents/Models/modelloBrutto.obj";
	const std::string TEXTURE_PATH = "LuxEngine/Contents/Textures/modelloBrutto.png";



	//Devices and queues
	struct graphicsDevice {
		_VkPhysicalDevice PD;						//Main physical device for graphics
		VkDevice LD;								//Main logical device for graphics
		VkQueue graphicsQueue;						//Main graphics queue. Runs on graphicsLD
		VkQueue presentQueue;						//Main graphics queue. Runs on graphicsLD
	}graphics;

	struct computeDevice {
		_VkPhysicalDevice PD;						//Main physical device for computing
		VkDevice LD;								//Main logical device for computing
		std::vector<VkQueue> computeQueues;			//Main compute queues. Run on computeLD
	}compute;

	std::vector<computeDevice> secondary;			//Secondary devices and queues for computation



	//Swapchain
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkFramebuffer> swapChainFramebuffers;

	//Render
	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	const int MAX_FRAMES_IN_FLIGHT = 16; //Default:2


	//Geometry
public:
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;
	LuxArray<Vertex> vertices;
	LuxArray<uint32> indices;

private:



	//Textures
	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;

	VkImageView textureImageView;
	VkSampler textureSampler;

	VkImage textureImage;
	VkDeviceMemory textureImageMemory;


	//Descriptors
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;


	//Commands
	VkCommandPool graphicsCommandPool;
	std::vector<VkCommandBuffer> commandBuffers;


	//Render semaphores and frames
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;
	int64 currentFrame = 0;


	//Debug and validation layers data
	const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
	const std::vector<const char*> requiredDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };






	//Main >> this
	void initWindow();	void initVulkan();
	void mainLoop();	void FPSCounter();
	void cleanup();		void cleanupSwapChain();


	//Devices >> ./Devices.cpp
	void getPhysicalDevices();		void createLogicalDevice(_VkPhysicalDevice* PD, VkDevice* LD, VkQueue* graphicsQueue, VkQueue* presentQueue, std::vector<VkQueue>* computeQueues);
	bool isDeviceSuitable(VkPhysicalDevice device, std::string errorText);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);


	//Swapchain >> ./Swapchain.cpp
	void createSwapChain();			void recreateSwapChain();
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	void createImageViews();
	void createDepthResources();





	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);




	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);


	void createTextureSampler();


	void createTextureImageView();


	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);


	void createTextureImage();

	void createImage(uint32 width, uint32 height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);




	VkCommandBuffer beginSingleTimeCommands();

	void endSingleTimeCommands(VkCommandBuffer commandBuffer);



	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);


	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32 width, uint32 height);













	//TODO output stops when interacting with console

	void createInstance();

	void setupDebugMessenger();

	void createSurface();







	void createRenderPass();

	void createDescriptorSetLayout();

	void createGraphicsPipeline();

	void createFramebuffers();

	void createGraphicsCommandPool();

	void createVertexBuffer();

	void createIndexBuffer();

	void createDescriptorPool();

	void createDescriptorSets();

	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);


	uint32 findMemoryType(uint32 typeFilter, VkMemoryPropertyFlags properties);

	void createDrawCommandBuffers();

	void createSyncObjects();

	void drawFrame();

	VkShaderModule createShaderModule(uint32* code, uint32* size);



	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);























































































































	/*
	The application launches a compute shader that renders the mandelbrot set,
	by rendering it into a storage buffer.
	The storage buffer is then read from the GPU, and saved as .png.
	*/
private:
	// The pixels of the rendered mandelbrot set are in this format:
	struct Pixel { unsigned char r, g, b, a; };

	VkDebugReportCallbackEXT debugReportCallback;

	//We will be creating a simple compute pipeline in this application
	VkPipeline computePipeline;
	VkPipelineLayout computePipelineLayout;
	VkShaderModule computeShaderModule;


	//The command buffer is used to record commands, that will be submitted to a queue.
	//To allocate such command buffers, we use a command pool.
	VkCommandPool computeCommandPool;
	VkCommandBuffer computeCommandBuffer;


	//Descriptors represent resources in shaders. They allow us to use things like
	//uniform buffers, storage buffers and images in GLSL.

	//A single descriptor represents a single resource, and several descriptors are organized
	//into descriptor sets, which are basically just collections of descriptors.
	VkDescriptorPool computeDescriptorPool;
	VkDescriptorSet computeDescriptorSet;
	VkDescriptorSetLayout computeDescriptorSetLayout;


	//The mandelbrot set will be rendered to this buffer.
	//The memory that backs the buffer is bufferMemory.
	VkBuffer buffer;
	VkDeviceMemory bufferMemory;
	uint32 bufferSize; // size of `buffer` in bytes.


	std::vector<const char*> enabledLayers;



public:
	void run2() {
		//system("del /F mandelbrot.png");

		// Buffer size of the storage buffer that will contain the rendered mandelbrot set.
		bufferSize = sizeof(Pixel) * COMPUTE_WIDTH * COMPUTE_HEIGHT; //TODO set to char

		// Initialize vulkan:
		//findPhysicalDevice();
		//createDevice();
		createBuffer();
		createComputeDescriptorSetLayout();
		createDescriptorSet();
		createComputePipeline();
		createComputeCommandBuffer();

		// Finally, run the recorded command buffer.
		printf("Initialized\n");
	}


	static VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallbackFn(
		VkDebugReportFlagsEXT			flags,
		VkDebugReportObjectTypeEXT		objectType,
		uint64							object,
		uint64							location,
		int32							messageCode,
		const char* pLayerPrefix,
		const char* pMessage,
		void* pUserData) {

		printf("Debug Report: %s: %s\n", pLayerPrefix, pMessage);

		return VK_FALSE;
	}


	void createBuffer() {
		//We will now create a buffer. We will render the mandelbrot set into this buffer
		//in a computer shade later.
		VkBufferCreateInfo bufferCreateInfo = {};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.size = bufferSize; // buffer size in bytes. 
		bufferCreateInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT; // buffer is used as a storage buffer.
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // buffer is exclusive to a single queue family at a time. 

		Try(vkCreateBuffer(compute.LD, &bufferCreateInfo, NULL, &buffer)) Quit("Fatal error"); // create buffer.

		//But the buffer doesn't allocate memory for itself, so we must do that manually.
		//First, we find the memory requirements for the buffer.
		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(compute.LD, buffer, &memoryRequirements);

		//Now use obtained memory requirements info to allocate the memory for the buffer.
		VkMemoryAllocateInfo allocateInfo = {};
		allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocateInfo.allocationSize = memoryRequirements.size; // specify required memory.

		//There are several types of memory that can be allocated, and we must choose a memory type that:
		//1) Satisfies the memory requirements(memoryRequirements.memoryTypeBits).
		//2) Satifies our own usage requirements. We want to be able to read the buffer memory from the GPU to the CPU
		//   with vkMapMemory, so we set VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT.
		//Also, by setting VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, memory written by the device(GPU) will be easily
		//visible to the host(CPU), without having to call any extra flushing commands. So mainly for convenience, we set
		//this flag.
		allocateInfo.memoryTypeIndex = findMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
		Try(vkAllocateMemory(compute.LD, &allocateInfo, NULL, &bufferMemory)) Quit("Fatal error"); // allocate memory on device.

		// Now associate that allocated memory with the buffer. With that, the buffer is backed by actual memory. 
		Try(vkBindBufferMemory(compute.LD, buffer, bufferMemory, 0)) Quit("Fatal error");
	}

	void createComputeDescriptorSetLayout() {
		//Here we specify a binding of type VK_DESCRIPTOR_TYPE_STORAGE_BUFFER to the binding point 0.
		//This binds to
		//  layout(std140, binding = 0) buffer buf
		//in the compute shader.
		VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
		descriptorSetLayoutBinding.binding = 0; // binding = 0
		descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		descriptorSetLayoutBinding.descriptorCount = 1;
		descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
		descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutCreateInfo.bindingCount = 1; // only a single binding in this descriptor set layout. 
		descriptorSetLayoutCreateInfo.pBindings = &descriptorSetLayoutBinding;

		// Create the descriptor set layout. 
		Try(vkCreateDescriptorSetLayout(compute.LD, &descriptorSetLayoutCreateInfo, NULL, &computeDescriptorSetLayout)) Quit("Fatal error");
	}

	void createDescriptorSet() {
		//So we will allocate a descriptor set here.
		//But we need to first create a descriptor pool to do that.
		//Our descriptor pool can only allocate a single storage buffer.
		VkDescriptorPoolSize descriptorPoolSize = {};
		descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		descriptorPoolSize.descriptorCount = 1;

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCreateInfo.maxSets = 1; // we only need to allocate one descriptor set from the pool.
		descriptorPoolCreateInfo.poolSizeCount = 1;
		descriptorPoolCreateInfo.pPoolSizes = &descriptorPoolSize;

		// create descriptor pool.
		Try(vkCreateDescriptorPool(compute.LD, &descriptorPoolCreateInfo, NULL, &computeDescriptorPool)) Quit("Fatal error");

		//With the pool allocated, we can now allocate the descriptor set.
		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
		descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocateInfo.descriptorPool = computeDescriptorPool; // pool to allocate from.
		descriptorSetAllocateInfo.descriptorSetCount = 1; // allocate a single descriptor set.
		descriptorSetAllocateInfo.pSetLayouts = &computeDescriptorSetLayout;

		// allocate descriptor set.
		Try(vkAllocateDescriptorSets(compute.LD, &descriptorSetAllocateInfo, &computeDescriptorSet)) Quit("Fatal error");

		//Next, we need to connect our actual storage buffer with the descrptor.
		//We use vkUpdateDescriptorSets() to update the descriptor set.

		// Specify the buffer to bind to the descriptor.
		VkDescriptorBufferInfo descriptorBufferInfo = {};
		descriptorBufferInfo.buffer = buffer;
		descriptorBufferInfo.offset = 0;
		descriptorBufferInfo.range = bufferSize;

		VkWriteDescriptorSet writeDescriptorSet = {};
		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet = computeDescriptorSet;						// write to this descriptor set.
		writeDescriptorSet.dstBinding = 0;										// write to the first, and only binding.
		writeDescriptorSet.descriptorCount = 1;									// update a single descriptor.
		writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;	// storage buffer.
		writeDescriptorSet.pBufferInfo = &descriptorBufferInfo;

		// perform the update of the descriptor set.
		vkUpdateDescriptorSets(compute.LD, 1, &writeDescriptorSet, 0, NULL);
	}

	// Read file into array of bytes, and cast to uint32*, then return.
	// The data has been padded, so that it fits into an array uint32.
	uint32* readFile(uint32* length, const char* filename) {

		FILE* fp = fopen(filename, "rb");
		if (fp == NULL) {
			printf("Could not find or open file: %s\n", filename);
		}

		// get file size.
		fseek(fp, 0, SEEK_END);
		long filesize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		long filesizepadded = long(ceil(filesize / 4.0)) * 4;

		// read file contents.
		char* str = new char[filesizepadded];
		fread(str, filesize, sizeof(char), fp);
		fclose(fp);

		// data padding. 
		for (int i = filesize; i < filesizepadded; i++) {
			str[i] = 0;
		}

		*length = filesizepadded;
		return (uint32*)str;
	}

	//We create a compute pipeline here.
	void createComputePipeline() {
		//Create a shader module. A shader module basically just encapsulates some shader code.
		uint32 filelength;
		// the code in comp.spv was created by running the command:
		// glslangValidator.exe -V shader.comp
		uint32* code = readFile(&filelength, "LuxEngine/Contents/shaders/comp.spv"); //TODO use unique read file function for shaders
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.pCode = code;
		createInfo.codeSize = filelength;

		Try(vkCreateShaderModule(compute.LD, &createInfo, NULL, &computeShaderModule)) Quit("Fatal error");
		delete[] code;

		//Now let us actually create the compute pipeline.
		//A compute pipeline is very simple compared to a graphics pipeline.
		//It only consists of a single stage with a compute shader.

		//So first we specify the compute shader stage, and it's entry point(main).
		VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};
		shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		shaderStageCreateInfo.module = computeShaderModule;
		shaderStageCreateInfo.pName = "main";

		//The pipeline layout allows the pipeline to access descriptor sets.
		//So we just specify the descriptor set layout we created earlier.
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = 1;
		pipelineLayoutCreateInfo.pSetLayouts = &computeDescriptorSetLayout;
		Try(vkCreatePipelineLayout(compute.LD, &pipelineLayoutCreateInfo, NULL, &computePipelineLayout)) Quit("Fatal error");

		VkComputePipelineCreateInfo pipelineCreateInfo = {};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		pipelineCreateInfo.stage = shaderStageCreateInfo;
		pipelineCreateInfo.layout = computePipelineLayout;

		//Now, we finally create the compute pipeline.
		Try(vkCreateComputePipelines(compute.LD, VK_NULL_HANDLE, 1, &pipelineCreateInfo, NULL, &computePipeline)) Quit("Fatal error");
	}

	void createComputeCommandBuffer() {
		//We are getting closer to the end. In order to send commands to the device(GPU),
		//we must first record commands into a command buffer.
		//To allocate a command buffer, we must first create a command pool. So let us do that.
		VkCommandPoolCreateInfo commandPoolCreateInfo = {};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.flags = 0;
		// the queue family of this command pool. All command buffers allocated from this command pool,
		// must be submitted to queues of this family ONLY. 
		commandPoolCreateInfo.queueFamilyIndex = compute.PD.indices.computeFamilies[0];
		Try(vkCreateCommandPool(compute.LD, &commandPoolCreateInfo, NULL, &computeCommandPool)) Quit("Fatal error");

		//Now allocate a command buffer from the command pool.
		VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.commandPool = computeCommandPool; // specify the command pool to allocate from. 
		// if the command buffer is primary, it can be directly submitted to queues. 
		// A secondary buffer has to be called from some primary command buffer, and cannot be directly 
		// submitted to a queue. To keep things simple, we use a primary command buffer. 
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandBufferCount = 1; // allocate a single command buffer. 
		Try(vkAllocateCommandBuffers(compute.LD, &commandBufferAllocateInfo, &computeCommandBuffer)) Quit("Fatal error"); // allocate command buffer.

		//Now we shall start recording commands into the newly allocated command buffer.
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; // the buffer is only submitted and used once in this application.
		Try(vkBeginCommandBuffer(computeCommandBuffer, &beginInfo)) Quit("Fatal error"); // start recording commands.

		//We need to bind a pipeline, AND a descriptor set before we dispatch.

		//The validation layer will NOT give warnings if you forget these, so be very careful not to forget them.
		vkCmdBindPipeline(computeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline);
		vkCmdBindDescriptorSets(computeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipelineLayout, 0, 1, &computeDescriptorSet, 0, NULL);

		//Calling vkCmdDispatch basically starts the compute pipeline, and executes the compute shader.
		//The number of workgroups is specified in the arguments.
		//If you are already familiar with compute shaders from OpenGL, this should be nothing new to you.
		vkCmdDispatch(computeCommandBuffer, ceil((float)(COMPUTE_WIDTH) / WORKGROUP_SIZE), ceil((float)(COMPUTE_HEIGHT) / WORKGROUP_SIZE), 1); //one workgroup every 32 int32

		Try(vkEndCommandBuffer(computeCommandBuffer)) Quit("Fatal error"); // end recording commands.
	}


	void runCommandBuffer() {
		//Now we shall finally submit the recorded command buffer to a queue.
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1; // submit a single command buffer
		submitInfo.pCommandBuffers = &computeCommandBuffer; // the command buffer to submit.

		//We create a fence.
		VkFence fence;
		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = 0;
		Try(vkCreateFence(compute.LD, &fenceCreateInfo, NULL, &fence)) Quit("Fatal error");

		//We submit the command buffer on the queue, at the same time giving a fence.
		Try(vkQueueSubmit(compute.computeQueues[0], 1, &submitInfo, fence)) Quit("Fatal error");
		//The command will not have finished executing until the fence is signalled.
		//So we wait here.
		//We will directly after this read our buffer from the GPU,
		//and we will not be sure that the command has finished executing unless we wait for the fence.
		//Hence, we use a fence here.
		Try(vkWaitForFences(compute.LD, 1, &fence, VK_TRUE, 100000000000)) Quit("Fatal error");

		vkDestroyFence(compute.LD, fence, NULL);
	}


	void cleanupCompute() {
		vkFreeMemory(compute.LD, bufferMemory, NULL);
		vkDestroyBuffer(compute.LD, buffer, NULL);
		vkDestroyShaderModule(compute.LD, computeShaderModule, NULL);
		vkDestroyDescriptorPool(compute.LD, computeDescriptorPool, NULL);
		vkDestroyDescriptorSetLayout(compute.LD, computeDescriptorSetLayout, NULL);
		vkDestroyPipelineLayout(compute.LD, computePipelineLayout, NULL);
		vkDestroyPipeline(compute.LD, computePipeline, NULL);
		vkDestroyCommandPool(compute.LD, computeCommandPool, NULL);
	}
};






#define Frame while(render.running)


static Render render;
static void run(bool useVSync) {
	render.vertices.add(std::vector<Vertex>{
		{ {-1, -1, 0},  { 1,1,1 },  { 0,0 } },
		{ {-1, 1, 0},	{ 1,1,1 },	{ 0,1 } },
		{ {1, -1, 0},	{ 1,1,1 },	{ 1,0 } },
		{ {1, 1, 0},	{ 1,1,1 },	{ 1,1 } }
	});

	render.indices.add(std::vector<uint32>{0, 1, 2, 2, 1, 3});

	render.run(useVSync, 45);
}


//This function initializes the Lux Engine. Use it only once
static void luxInit(bool useVSync) {
	std::thread t(run, useVSync);
	t.detach();
	render.running = true;
}
