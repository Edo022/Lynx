#pragma once

//Disabled useless warnings
#pragma warning( disable : 26812 )			//Prefer enum class to enum
#pragma warning( disable : 26495 )			//Uninitialized variables
#pragma warning( disable : 6237  )			//Always true enableValidationLayers condition 
#pragma warning( disable : 26451 )			//stb arithmetic overflow

//Dark magic
#include <vulkan/vulkan.h>					//Graphics
#include <GLFW/glfw3.h>						//Window system
#define GLM_FORCE_RADIANS					//Use radiants intead of degrees
#define GLM_FORCE_DEPTH_ZERO_TO_ONE			//0 to 1 depth instead of OpenGL -1 to 1
#include <glm/glm.hpp>						//Shader compatible geometry


//Default C++
#include <iostream>
#include <fstream>
#include <array>
#include <chrono>
#include <vector>
#include <thread>
#include <set>

#include <cmath>
#include <string>

//Junk
#include "LuxEngine/macros.h"				//Useful useless macros
#include <TermColor.hpp>					//Colored console output


//Structures
#include "Graphics/Structs/_VkPhysicalDevice.h"
#include "Graphics/Structs/Vertex.h"
#include "LuxEngine/Object/Object.h"
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Types/Containers/LuxDynArray.h"
#include "LuxEngine/Types/Containers/LuxArray.h"


//Re enable warnings for this header
#pragma warning( default : 26451 )			//Arithmetic overflow








// Debug, structures and macros -------------------------------------------------------------------------------------------------------------//








//Debug. It's dark magic, idk why or how it works, but it does
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
	LuxArray<VkSurfaceFormatKHR> formats;
	LuxArray<VkPresentModeKHR> presentModes;
};


//Validation layers in debug mode
#ifdef NDEBUG	
const bool enableValidationLayers = false;
#else			
const bool enableValidationLayers = true;
#endif



#define findDepthFormat()																	\
	findSupportedFormat(																	\
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },\
		VK_IMAGE_TILING_OPTIMAL,															\
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT										\
	);

//Dark magic
#define populateDebugMessengerCreateInfo(createInfo)\
	createInfo = {};\
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;\
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;\
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;\
	createInfo.pfnUserCallback = debugCallback;








// Engine class -----------------------------------------------------------------------------------------------------------------------------//








class Engine {
public:
	double FPS = 0;
	float FOV;
	bool running;
	bool useVSync;

	void run(bool _useVSync = true, float FOV = 45.0f);

private:
	//Main
	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkSurfaceKHR surface;

	//Window
	GLFWwindow* window;								//Main engine's window
	const uint32 WIDTH = 1920, HEIGHT = 1080;		//Default size in windowed mode
	//const uint32 WIDTH = 800, HEIGHT = 600;		//Default size in windowed mode

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
		LuxDynArray<VkQueue> computeQueues;			//Main compute queues. Run on computeLD
	}compute;

	LuxArray<computeDevice> secondary;		//Secondary devices and queues for computation



	//Main >> this
	void mainLoop();		void FPSCounter(); 
	void initWindow();		void createInstance();

	//Devices >> ./Devices.cpp
	void getPhysicalDevices();		void createLogicalDevice(_VkPhysicalDevice* PD, VkDevice* LD, VkQueue* graphicsQueue, VkQueue* presentQueue, LuxDynArray<VkQueue>* computeQueues);
	bool isDeviceSuitable(VkPhysicalDevice device, std::string errorText);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

	//Shared functions >> this
	uint32* readShaderFromFile(uint32* length, const char* filename);
	VkShaderModule createShaderModule(VkDevice device, uint32* code, uint32* size);
	void createBuffer(VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);




// Graphics ---------------------------------------------------------------------------------------------------------------------------------//




	bool framebufferResized = false;				//Updates the swapchain when the window is resized	
	const char* VERT_PATH = "LuxEngine/Contents/Shaders/vert.spv";
	const char* FRAG_PATH = "LuxEngine/Contents/Shaders/frag.spv";

	//Swapchain
	VkSwapchainKHR swapChain;
	LuxArray<VkImage> swapChainImages;
	LuxArray<VkImageView> swapChainImageViews;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	LuxArray<VkFramebuffer> swapChainFramebuffers;

	//Render
	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	const int32 MAX_FRAMES_IN_FLIGHT = 16; //Default:2

public:
	//Geometry
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;
	LuxArray<Vertex> vertices;
	LuxArray<uint32> indices;

private:
	//Textures
	VkImage textureImage;
	VkImageView textureImageView;
	VkDeviceMemory textureImageMemory;
	VkSampler textureSampler;

	//Images
	VkImage depthImage;
	VkImageView depthImageView;
	VkDeviceMemory depthImageMemory;

	//Descriptors
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorPool descriptorPool;
	LuxArray<VkDescriptorSet> descriptorSets;

	//Commands
	VkCommandPool graphicsCommandPool;
	LuxArray<VkCommandBuffer> commandBuffers;

	//Render semaphores and frames
	LuxArray<VkSemaphore> imageAvailableSemaphores;
	LuxArray<VkSemaphore> renderFinishedSemaphores;
	LuxArray<VkFence> inFlightFences;
	LuxArray<VkFence> imagesInFlight;
	int64 currentFrame = 0;

	//Debug and validation layers data
	LuxArray<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
	LuxArray<const char*> requiredDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };



	//Graphics >> Graphics/Graphics.cpp
	void runGraphics(bool _useVSync = true, float FOV = 45.0f);
	void initVulkan();
	void createSurface();
	void createSyncObjects();
	void setupDebugMessenger();
	void drawFrame();
	static void framebufferResizeCallback(GLFWwindow* window, int32 width, int32 height);
	void cleanupGraphics();

	//Graphics swapchain >> Graphics/Swapchain.cpp
	void createSwapChain();			void recreateSwapChain();
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(LuxArray<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(LuxArray<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	void createImageViews();
	void createDepthResources();
	void cleanupSwapChain();


	//Graphics textures and images >> Graphics/Images.cpp
	void createVertexBuffer();
	void createIndexBuffer();

	void createTextureImage();
	void createTextureImageView();
	void createTextureSampler();

	void createImage(uint32 width, uint32 height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32 width, uint32 height);
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);


	//Graphics commands >> Graphics/Commands.cpp
	void createGraphicsCommandPool();
	void createDrawCommandBuffers();
	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);


	//Graphics render and descriptors >> Graphics/Pipeline.cpp
	void createGraphicsPipeline();
	void createRenderPass();
	void createFramebuffers();

	void createDescriptorPool();
	void createDescriptorSetLayout();
	void createDescriptorSets();


	//Graphics other >> Graphics/Graphics.cpp
	VkFormat findSupportedFormat(LuxArray<VkFormat> candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	uint32 findMemoryType(uint32 typeFilter, VkMemoryPropertyFlags properties);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);




// Compute ----------------------------------------------------------------------------------------------------------------------------------//




	//COMPUTE 
	const int32 COMPUTE_WIDTH = WIDTH;
	const int32 COMPUTE_HEIGHT = HEIGHT;
	const int32 WORKGROUP_SIZE = 32; // Workgroup size in compute shader.

	struct Pixel { unsigned char r, g, b, a; };

	VkDebugReportCallbackEXT debugReportCallback;

	//Compute pipeline
	VkPipeline computePipeline;
	VkPipelineLayout computePipelineLayout;
	VkShaderModule computeShaderModule;

	//commands
	VkCommandPool computeCommandPool;
	VkCommandBuffer computeCommandBuffer;

	//A single descriptor represents a single resource, and several descriptors are organized
	//into descriptor sets, which are basically just collections of descriptors.
	VkDescriptorPool computeDescriptorPool;
	VkDescriptorSet computeDescriptorSet;
	VkDescriptorSetLayout computeDescriptorSetLayout;

	//Buffer
	VkBuffer buffer;
	VkDeviceMemory bufferMemory;
	uint32 bufferSize;



	//Compute >> Compute/Compute.cpp
	void RunCompute();
	void cleanupCompute();

	//Compute pipeline and descriptors >> Compute/Pipeline.cpp
	void createComputePipeline();
	void createComputeDescriptorSetLayout();
	void createDescriptorSet();

	//Compute command buffers >> Compute/Commands.cpp
	void createComputeCommandBuffer();
	void runCommandBuffer();

};








// Init -------------------------------------------------------------------------------------------------------------------------------------//








#define Frame while(engine.running)


static Engine engine;
//This function is used by the engine. You shouldn't call it
static void __lp_lux_init_run_thr(bool useVSync) {
	engine.vertices = {
		{ {-1, -1, 0}, { 1,1,1 }, { 0,0 } },
		{ {-1, 1, 0},	{ 1,1,1 },	{ 0,1 } },
		{ {1, -1, 0},	{ 1,1,1 },	{ 1,0 } },
		{ {1, 1, 0},	{ 1,1,1 },	{ 1,1 } }
	};
	engine.indices = {0, 1, 2, 2, 1, 3};
	engine.run(useVSync, 45);
}


//This function initializes the Lux Engine. Call it only once
static void luxInit(bool useVSync) {
	std::thread t(__lp_lux_init_run_thr, useVSync);
	t.detach();
	engine.running = true;
}
