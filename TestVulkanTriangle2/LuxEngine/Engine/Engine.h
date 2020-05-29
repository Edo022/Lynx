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

	void createBuffer(VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

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
	void run2();

	void createComputeDescriptorSetLayout();
	void createDescriptorSet();
	// Read file into array of bytes, and cast to uint32*, then return.
	// The data has been padded, so that it fits into an array uint32.
	uint32* readShaderFromFile(uint32* length, const char* filename);
	//We create a compute pipeline here.
	void createComputePipeline();
	void createComputeCommandBuffer();
	void runCommandBuffer();
	void cleanupCompute();
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
