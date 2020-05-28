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
#include "macros.h"							//Useful useless macros
#include <TermColor.hpp>					//Colored console output


//Structures
#include "Structs/_VkPhysicalDevice.h"
#include "Structs/Vertex.h"
#include "Object/Object.h"
#include "Types/Integers/Integers.h"

#include "Types/Containers/mdrArray.h"

//Re enable warnings for this header
#pragma warning( default : 26451 )			//Arithmetic overflow






//TODO delete
// Used for validating return values of Vulkan API calls.
#define VK_CHECK_RESULT(f){                                                                             \
	VkResult res = (f);																					\
	if (res != VK_SUCCESS){							        											\
		printf("Fatal : VkResult is %d in %s at line %d\n", res,  __FILE__, __LINE__);                  \
		assert(res == VK_SUCCESS);																		\
	}																									\
}





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
//Object for Vulkan descriptor
struct UniformBufferObject {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};











#define findDepthFormat()																	\
	findSupportedFormat(																	\
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },\
		VK_IMAGE_TILING_OPTIMAL,															\
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT										);


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



	void checkObjects() {
		for(int i=0;i<ObjectsQueue.size();i++) {
			if (ObjectsQueue.isValid(i)) {
				createVertexBuffer(ObjectsQueue[i]);
				createIndexBuffer(ObjectsQueue[i]);
				objects.add(ObjectsQueue[i]);
				ObjectsQueue.remove(i);

				if (sc) createDrawCommandBuffers();
				else {
					sc = true;
					break;
				}
			}
		}
	}
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



	const std::string VERT_PATH = "Contents/Shaders/vert.spv";
	const std::string FRAG_PATH = "Contents/Shaders/frag.spv";
	const std::string MODEL_PATH = "Contents/Models/modelloBrutto.obj";
	const std::string TEXTURE_PATH = "Contents/Textures/modelloBrutto.png";



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
	MdrArray<MdrObject*> ObjectsQueue;
private:
	MdrArray<MdrObject*> objects;
	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;


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

	void createVertexBuffer(MdrObject* object);

	void createIndexBuffer(MdrObject* object);

	void createUniformBuffers();

	void createDescriptorPool();

	void createDescriptorSets();

	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);


	uint32 findMemoryType(uint32 typeFilter, VkMemoryPropertyFlags properties);

	void createDrawCommandBuffers();

	void createSyncObjects();

	void updateUniformBuffer(uint32 currentImage, MdrObject* object);

	void drawFrame();

	VkShaderModule createShaderModule(const std::vector<char>& code);



	static std::vector<char> readShaderFromFile(const std::string& filename);

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);


	//void updateUniformBuffer2(uint32 currentImage) {

	//	UniformBufferObject ubo2{};
	//	//"time;"
	//	ubo2.model =
	//		glm::scale(
	//			glm::translate(
	//				glm::rotate(glm::rotate(glm::rotate(glm::mat4(1.0f),
	//					glm::radians((float)(objects[0].rot.x)), glm::vec3(1.0f, 0.0f, 0.0f)),
	//					glm::radians((float)(objects[0].rot.y)), glm::vec3(0.0f, 1.0f, 0.0f)),
	//					glm::radians((float)(objects[0].rot.z)), glm::vec3(0.0f, 0.0f, 1.0f)),
	//				glm::vec3(objects[0].pos.x / 10, objects[0].pos.y / 10, /*objects[0].pos.z / 10*/1)
	//			),
	//			glm::vec3(objects[0].scl.x, objects[0].scl.y, objects[0].scl.z)
	//		);


	//	ubo2.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	//	ubo2.proj = glm::perspective(glm::radians(FOV), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 10.0f);
	//	ubo2.proj[1][1] *= -1;

	//	void* data2;
	//	//printf("\n\n%d\n\n", vkMapMemory(graphicsLD, uniformBuffersMemory[currentImage], 0, sizeof(ubo2), 0, &data2));
	//	vkMapMemory(graphicsLD, uniformBuffersMemory[currentImage], 0, sizeof(ubo2), 0, &data2);
	//	memcpy(data2, &ubo2, sizeof(ubo2));
	//	vkUnmapMemory(graphicsLD, uniformBuffersMemory[currentImage]);
	//}
























































































































/*
The application launches a compute shader that renders the mandelbrot set,
by rendering it into a storage buffer.
The storage buffer is then read from the GPU, and saved as .png.
*/
private:
	// The pixels of the rendered mandelbrot set are in this format:
	struct Pixel {
		unsigned char r, g, b, a;
	};

	/*
	In order to use Vulkan, you must create an instance.
	*/ //moved

	VkDebugReportCallbackEXT debugReportCallback;
	/*
	The physical device is some device on the system that supports usage of Vulkan.
	Often, it is simply a graphics card that supports Vulkan.
	*/
	//VkPhysicalDevice physicalDevice;
	/*
	Then we have the logical device VkDevice, which basically allows
	us to interact with the physical device.
	*/
	//VkDevice device;

	/*
	The pipeline specifies the pipeline that all graphics and compute commands pass though in Vulkan.

	We will be creating a simple compute pipeline in this application.
	*/
	VkPipeline computePipeline;
	VkPipelineLayout computePipelineLayout;
	VkShaderModule computeShaderModule;

	/*
	The command buffer is used to record commands, that will be submitted to a queue.

	To allocate such command buffers, we use a command pool.
	*/
	VkCommandPool computeCommandPool;
	VkCommandBuffer computeCommandBuffer;

	/*

	Descriptors represent resources in shaders. They allow us to use things like
	uniform buffers, storage buffers and images in GLSL.

	A single descriptor represents a single resource, and several descriptors are organized
	into descriptor sets, which are basically just collections of descriptors.
	*/
	VkDescriptorPool computeDescriptorPool;
	VkDescriptorSet computeDescriptorSet;
	VkDescriptorSetLayout computeDescriptorSetLayout;

	/*
	The mandelbrot set will be rendered to this buffer.

	The memory that backs the buffer is bufferMemory.
	*/
	VkBuffer buffer;
	VkDeviceMemory bufferMemory;

	uint32_t bufferSize; // size of `buffer` in bytes.

	std::vector<const char*> enabledLayers;

	/*
	In order to execute commands on a device(GPU), the commands must be submitted
	to a queue. The commands are stored in a command buffer, and this command buffer
	is given to the queue.

	There will be different kinds of queues on the device. Not all queues support
	graphics operations, for instance. For this application, we at least want a queue
	that supports compute operations.
	*/
	//VkQueue queue; // a queue supporting compute operations.

	/*
	Groups of queues that have the same capabilities(for instance, they all supports graphics and computer operations),
	are grouped into queue families.

	When submitting a command buffer, you must specify to which queue in the family you are submitting to.
	This variable keeps track of the index of that queue in its family.
	*/
	uint32_t queueFamilyIndex;

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
		//runCommandBuffer();

		// The former command rendered a mandelbrot set to a buffer.
		// Save that buffer as a png on disk.
		//saveRenderedImage();

		// Clean up all vulkan resources.
		//cleanup();
	}

	//void saveRenderedImage() {
	//    printf("\nsaving");
	//    void* mappedMemory = NULL;
	//    // Map the buffer memory, so that we can read from it on the CPU.
	//    vkMapMemory(compute.LD, bufferMemory, 0, bufferSize, 0, &mappedMemory);
	//    Pixel* pmappedMemory = (Pixel*)mappedMemory;

	//    // Get the color data from the buffer, and cast it to bytes.
	//    // We save the data to a vector.
	//    std::vector<Pixel> image;
	//    image.reserve(COMPUTE_WIDTH * COMPUTE_HEIGHT * 4);
	//    for (int i = 0; i < COMPUTE_WIDTH * COMPUTE_HEIGHT; i++) {
	//        image.push_back({
	//            (pmappedMemory[i].r),
	//            (pmappedMemory[i].g),
	//            (pmappedMemory[i].b),
	//            (pmappedMemory[i].a)
	//        });
	//    }

	//    std::vector<unsigned char> image2;
	//    for (auto i : image) {
	//        image2.push_back((unsigned char)(i.r));
	//        image2.push_back((unsigned char)(i.g));
	//        image2.push_back((unsigned char)(i.b));
	//        image2.push_back((unsigned char)(i.a));
	//    }

	//    // Done reading, so unmap.
	//    vkUnmapMemory(compute.LD, bufferMemory);

	//    // Now we save the acquired color data to a .png.
	//    unsigned error = lodepng::encode("mandelbrot.png", image2, COMPUTE_WIDTH, COMPUTE_HEIGHT);
	//    if (error) printf("encoder error %d: %s", error, lodepng_error_text(error));
	//    printf("\nsaved");
	//}

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallbackFn(
		VkDebugReportFlagsEXT                       flags,
		VkDebugReportObjectTypeEXT                  objectType,
		uint64_t                                    object,
		size_t                                      location,
		int32_t                                     messageCode,
		const char* pLayerPrefix,
		const char* pMessage,
		void* pUserData) {

		printf("Debug Report: %s: %s\n", pLayerPrefix, pMessage);

		return VK_FALSE;
	}

	//void findPhysicalDevice() {
	//    /*
	//    In this function, we find a physical device that can be used with Vulkan.
	//    */

	//    /*
	//    So, first we will list all physical devices on the system with vkEnumeratePhysicalDevices .
	//    */
	//    uint32_t deviceCount;
	//    vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);
	//    if (deviceCount == 0) {
	//        throw std::runtime_error("could not find a device with vulkan support");
	//    }

	//    std::vector<VkPhysicalDevice> devices(deviceCount);
	//    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	//    /*
	//    Next, we choose a device that can be used for our purposes.

	//    With VkPhysicalDeviceFeatures(), we can retrieve a fine-grained list of physical features supported by the device.
	//    However, in this demo, we are simply launching a simple compute shader, and there are no
	//    special physical features demanded for this task.

	//    With VkPhysicalDeviceProperties(), we can obtain a list of physical device properties. Most importantly,
	//    we obtain a list of physical device limitations. For this application, we launch a compute shader,
	//    and the maximum size of the workgroups and total number of compute shader invocations is limited by the physical device,
	//    and we should ensure that the limitations named maxComputeWorkGroupCount, maxComputeWorkGroupInvocations and
	//    maxComputeWorkGroupSize are not exceeded by our application.  Moreover, we are using a storage buffer in the compute shader,
	//    and we should ensure that it is not larger than the device can handle, by checking the limitation maxStorageBufferRange.

	//    However, in our application, the workgroup size and total number of shader invocations is relatively small, and the storage buffer is
	//    not that large, and thus a vast majority of devices will be able to handle it. This can be verified by looking at some devices at_
	//    http://vulkan.gpuinfo.org/

	//    Therefore, to keep things simple and clean, we will not perform any such checks here, and just pick the first physical
	//    device in the list. But in a real and serious application, those limitations should certainly be taken into account.

	//    */
	//    for (VkPhysicalDevice device : devices) {
	//        if (true) { // As above stated, we do no feature checks, so just accept.
	//            compute.PD.device = compute.LD;
	//            break;
	//        }
	//    }
	//}

	// Returns the index of a queue family that supports compute operations. 
	//uint32_t getComputeQueueFamilyIndex() {
	//    uint32_t queueFamilyCount;

	//    vkGetPhysicalDeviceQueueFamilyProperties(compute.PD.device, &queueFamilyCount, NULL);

	//    // Retrieve all queue families.
	//    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	//    vkGetPhysicalDeviceQueueFamilyProperties(compute.PD.device, &queueFamilyCount, queueFamilies.data());

	//    // Now find a family that supports compute.
	//    uint32_t i = 0;
	//    for (; i < queueFamilies.size(); ++i) {
	//        VkQueueFamilyProperties props = queueFamilies[i];

	//        if (props.queueCount > 0 && (props.queueFlags & VK_QUEUE_COMPUTE_BIT)) {
	//            // found a queue with compute. We're done!
	//            break;
	//        }
	//    }

	//    if (i == queueFamilies.size()) {
	//        throw std::runtime_error("could not find a queue family that supports operations");
	//    }

	//    return i;
	//}

	//void createDevice() {
	//    /*
	//    We create the logical device in this function.
	//    */

	//    /*
	//    When creating the device, we also specify what queues it has.
	//    */
	//    VkDeviceQueueCreateInfo queueCreateInfo = {};
	//    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	//    queueFamilyIndex = getComputeQueueFamilyIndex(); // find queue family with compute capability.
	//    queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
	//    queueCreateInfo.queueCount = 1; // create one queue in this family. We don't need more.
	//    float queuePriorities = 1.0;  // we only have one queue, so this is not that imporant. 
	//    queueCreateInfo.pQueuePriorities = &queuePriorities;

	//    /*
	//    Now we create the logical device. The logical device allows us to interact with the physical
	//    device.
	//    */
	//    VkDeviceCreateInfo deviceCreateInfo = {};

	//    // Specify any desired device features here. We do not need any for this application, though.
	//    VkPhysicalDeviceFeatures deviceFeatures = {};

	//    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	//    deviceCreateInfo.enabledLayerCount = enabledLayers.size();  // need to specify validation layers here as well.
	//    deviceCreateInfo.ppEnabledLayerNames = enabledLayers.data();
	//    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo; // when creating the logical device, we also specify what queues it has.
	//    deviceCreateInfo.queueCreateInfoCount = 1;
	//    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

	//    VK_CHECK_RESULT(vkCreateDevice(compute.PD.device, &deviceCreateInfo, NULL, &device)); // create logical device.

	//    // Get a handle to the only member of the queue family.
	//    vkGetDeviceQueue(compute.LD, queueFamilyIndex, 0, &queue);
	//}

	//// find memory type with desired properties.
	//uint32_t findMemoryType(uint32_t memoryTypeBits, VkMemoryPropertyFlags properties) {
	//    VkPhysicalDeviceMemoryProperties memoryProperties;

	//    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

	//    /*
	//    How does this search work?
	//    See the documentation of VkPhysicalDeviceMemoryProperties for a detailed description.
	//    */
	//    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
	//        if ((memoryTypeBits & (1 << i)) &&
	//            ((memoryProperties.memoryTypes[i].propertyFlags & properties) == properties))
	//            return i;
	//    }
	//    return -1;
	//}

	void createBuffer() {
		/*
		We will now create a buffer. We will render the mandelbrot set into this buffer
		in a computer shade later.
		*/

		VkBufferCreateInfo bufferCreateInfo = {};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.size = bufferSize; // buffer size in bytes. 
		bufferCreateInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT; // buffer is used as a storage buffer.
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // buffer is exclusive to a single queue family at a time. 

		VK_CHECK_RESULT(vkCreateBuffer(compute.LD, &bufferCreateInfo, NULL, &buffer)); // create buffer.

		/*
		But the buffer doesn't allocate memory for itself, so we must do that manually.
		*/

		/*
		First, we find the memory requirements for the buffer.
		*/
		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(compute.LD, buffer, &memoryRequirements);

		/*
		Now use obtained memory requirements info to allocate the memory for the buffer.
		*/
		VkMemoryAllocateInfo allocateInfo = {};
		allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocateInfo.allocationSize = memoryRequirements.size; // specify required memory.
		/*
		There are several types of memory that can be allocated, and we must choose a memory type that:

		1) Satisfies the memory requirements(memoryRequirements.memoryTypeBits).
		2) Satifies our own usage requirements. We want to be able to read the buffer memory from the GPU to the CPU
		   with vkMapMemory, so we set VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT.
		Also, by setting VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, memory written by the device(GPU) will be easily
		visible to the host(CPU), without having to call any extra flushing commands. So mainly for convenience, we set
		this flag.
		*/
		allocateInfo.memoryTypeIndex = findMemoryType(
			memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

		VK_CHECK_RESULT(vkAllocateMemory(compute.LD, &allocateInfo, NULL, &bufferMemory)); // allocate memory on device.

		// Now associate that allocated memory with the buffer. With that, the buffer is backed by actual memory. 
		VK_CHECK_RESULT(vkBindBufferMemory(compute.LD, buffer, bufferMemory, 0));
	}

	void createComputeDescriptorSetLayout() {
		/*
		Here we specify a descriptor set layout. This allows us to bind our descriptors to
		resources in the shader.

		*/

		/*
		Here we specify a binding of type VK_DESCRIPTOR_TYPE_STORAGE_BUFFER to the binding point
		0. This binds to

		  layout(std140, binding = 0) buffer buf

		in the compute shader.
		*/
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
		VK_CHECK_RESULT(vkCreateDescriptorSetLayout(compute.LD, &descriptorSetLayoutCreateInfo, NULL, &computeDescriptorSetLayout));
	}

	void createDescriptorSet() {
		/*
		So we will allocate a descriptor set here.
		But we need to first create a descriptor pool to do that.
		*/

		/*
		Our descriptor pool can only allocate a single storage buffer.
		*/
		VkDescriptorPoolSize descriptorPoolSize = {};
		descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		descriptorPoolSize.descriptorCount = 1;

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCreateInfo.maxSets = 1; // we only need to allocate one descriptor set from the pool.
		descriptorPoolCreateInfo.poolSizeCount = 1;
		descriptorPoolCreateInfo.pPoolSizes = &descriptorPoolSize;

		// create descriptor pool.
		VK_CHECK_RESULT(vkCreateDescriptorPool(compute.LD, &descriptorPoolCreateInfo, NULL, &computeDescriptorPool));

		/*
		With the pool allocated, we can now allocate the descriptor set.
		*/
		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
		descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocateInfo.descriptorPool = computeDescriptorPool; // pool to allocate from.
		descriptorSetAllocateInfo.descriptorSetCount = 1; // allocate a single descriptor set.
		descriptorSetAllocateInfo.pSetLayouts = &computeDescriptorSetLayout;

		// allocate descriptor set.
		VK_CHECK_RESULT(vkAllocateDescriptorSets(compute.LD, &descriptorSetAllocateInfo, &computeDescriptorSet));

		/*
		Next, we need to connect our actual storage buffer with the descrptor.
		We use vkUpdateDescriptorSets() to update the descriptor set.
		*/

		// Specify the buffer to bind to the descriptor.
		VkDescriptorBufferInfo descriptorBufferInfo = {};
		descriptorBufferInfo.buffer = buffer;
		descriptorBufferInfo.offset = 0;
		descriptorBufferInfo.range = bufferSize;

		VkWriteDescriptorSet writeDescriptorSet = {};
		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet = computeDescriptorSet; // write to this descriptor set.
		writeDescriptorSet.dstBinding = 0; // write to the first, and only binding.
		writeDescriptorSet.descriptorCount = 1; // update a single descriptor.
		writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; // storage buffer.
		writeDescriptorSet.pBufferInfo = &descriptorBufferInfo;

		// perform the update of the descriptor set.
		vkUpdateDescriptorSets(compute.LD, 1, &writeDescriptorSet, 0, NULL);
	}

	// Read file into array of bytes, and cast to uint32_t*, then return.
	// The data has been padded, so that it fits into an array uint32_t.
	uint32_t* readFile(uint32_t& length, const char* filename) {

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

		length = filesizepadded;
		return (uint32_t*)str;
	}

	void createComputePipeline() {
		/*
		We create a compute pipeline here.
		*/

		/*
		Create a shader module. A shader module basically just encapsulates some shader code.
		*/
		uint32_t filelength;
		// the code in comp.spv was created by running the command:
		// glslangValidator.exe -V shader.comp
		uint32_t* code = readFile(filelength, "Contents/shaders/comp.spv"); //TODO use unique read file function for shaders
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.pCode = code;
		createInfo.codeSize = filelength;

		VK_CHECK_RESULT(vkCreateShaderModule(compute.LD, &createInfo, NULL, &computeShaderModule));
		delete[] code;

		/*
		Now let us actually create the compute pipeline.
		A compute pipeline is very simple compared to a graphics pipeline.
		It only consists of a single stage with a compute shader.

		So first we specify the compute shader stage, and it's entry point(main).
		*/
		VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};
		shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		shaderStageCreateInfo.module = computeShaderModule;
		shaderStageCreateInfo.pName = "main";

		/*
		The pipeline layout allows the pipeline to access descriptor sets.
		So we just specify the descriptor set layout we created earlier.
		*/
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = 1;
		pipelineLayoutCreateInfo.pSetLayouts = &computeDescriptorSetLayout;
		VK_CHECK_RESULT(vkCreatePipelineLayout(compute.LD, &pipelineLayoutCreateInfo, NULL, &computePipelineLayout));

		VkComputePipelineCreateInfo pipelineCreateInfo = {};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		pipelineCreateInfo.stage = shaderStageCreateInfo;
		pipelineCreateInfo.layout = computePipelineLayout;

		/*
		Now, we finally create the compute pipeline.
		*/
		VK_CHECK_RESULT(vkCreateComputePipelines(
			compute.LD, VK_NULL_HANDLE,
			1, &pipelineCreateInfo,
			NULL, &computePipeline));
	}

	void createComputeCommandBuffer() {
		/*
		We are getting closer to the end. In order to send commands to the device(GPU),
		we must first record commands into a command buffer.
		To allocate a command buffer, we must first create a command pool. So let us do that.
		*/
		VkCommandPoolCreateInfo commandPoolCreateInfo = {};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.flags = 0;
		// the queue family of this command pool. All command buffers allocated from this command pool,
		// must be submitted to queues of this family ONLY. 
		commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;
		VK_CHECK_RESULT(vkCreateCommandPool(compute.LD, &commandPoolCreateInfo, NULL, &computeCommandPool));

		/*
		Now allocate a command buffer from the command pool.
		*/
		VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.commandPool = computeCommandPool; // specify the command pool to allocate from. 
		// if the command buffer is primary, it can be directly submitted to queues. 
		// A secondary buffer has to be called from some primary command buffer, and cannot be directly 
		// submitted to a queue. To keep things simple, we use a primary command buffer. 
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandBufferCount = 1; // allocate a single command buffer. 
		VK_CHECK_RESULT(vkAllocateCommandBuffers(compute.LD, &commandBufferAllocateInfo, &computeCommandBuffer)); // allocate command buffer.

		/*
		Now we shall start recording commands into the newly allocated command buffer.
		*/
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; // the buffer is only submitted and used once in this application.
		VK_CHECK_RESULT(vkBeginCommandBuffer(computeCommandBuffer, &beginInfo)); // start recording commands.

		/*
		We need to bind a pipeline, AND a descriptor set before we dispatch.

		The validation layer will NOT give warnings if you forget these, so be very careful not to forget them.
		*/
		vkCmdBindPipeline(computeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline);
		vkCmdBindDescriptorSets(computeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipelineLayout, 0, 1, &computeDescriptorSet, 0, NULL);

		/*
		Calling vkCmdDispatch basically starts the compute pipeline, and executes the compute shader.
		The number of workgroups is specified in the arguments.
		If you are already familiar with compute shaders from OpenGL, this should be nothing new to you.
		*/
		vkCmdDispatch(computeCommandBuffer, ceil((float)(COMPUTE_WIDTH * COMPUTE_HEIGHT) / WORKGROUP_SIZE), 1, 1); //one workgroup every 32 int32
		//vkCmdDispatch(commandBuffer, (uint32_t)ceil(((float)(COMPUTE_WIDTH)) / WORKGROUP_SIZE), (uint32_t)ceil(((float)(COMPUTE_HEIGHT)) / WORKGROUP_SIZE), 1);
		//vkCmdDispatch(commandBuffer, 550, 550, 1);


		VK_CHECK_RESULT(vkEndCommandBuffer(computeCommandBuffer)); // end recording commands.
	}

	void runCommandBuffer() {
		printf("aaa");
		/*
		Now we shall finally submit the recorded command buffer to a queue.
		*/

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1; // submit a single command buffer
		submitInfo.pCommandBuffers = &computeCommandBuffer; // the command buffer to submit.

		/*
		  We create a fence.
		*/
		VkFence fence;
		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = 0;
		VK_CHECK_RESULT(vkCreateFence(compute.LD, &fenceCreateInfo, NULL, &fence));

		/*
		We submit the command buffer on the queue, at the same time giving a fence.
		*/
		VK_CHECK_RESULT(vkQueueSubmit(compute.computeQueues[0], 1, &submitInfo, fence));
		/*
		The command will not have finished executing until the fence is signalled.
		So we wait here.
		We will directly after this read our buffer from the GPU,
		and we will not be sure that the command has finished executing unless we wait for the fence.
		Hence, we use a fence here.
		*/
		VK_CHECK_RESULT(vkWaitForFences(compute.LD, 1, &fence, VK_TRUE, 100000000000));

		vkDestroyFence(compute.LD, fence, NULL);
		printf("bbb");
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


//Adds a mdrObject to the render queue, that will be initialized and spawned before the next frame. Only allows objects with a defined geometry
static void mdrSpawnObject(Render* render, MdrObject* object) {
	render->ObjectsQueue.add(object);
	//TODO check if the object is valid
}
