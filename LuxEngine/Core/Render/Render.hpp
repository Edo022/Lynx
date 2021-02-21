#pragma once
#define LUX_H_GRAPHICS
#include "vulkan/vulkan.h"
#include "LuxEngine/macros.hpp"
#include "LuxEngine/Types/Integers/Integers.hpp"
#include "LuxEngine/Types/LuxFenceDE.hpp"
#include "LuxEngine/Types/Containers/CtArray.hpp"
#include "LuxEngine/Core/Render/Window.hpp"
#include "LuxEngine/Core/Render/GOutput.hpp"
#include "LuxEngine/Types/LuxObject/LuxObject.hpp"








namespace lux::core::render{
	extern RtArray<VkSemaphore>	s_imageAquired; 			extPadding(s_imageAquired)
	extern RtArray<VkSemaphore>	s_objectsRendered; 			extPadding(s_objectsRendered)
	extern RtArray<VkSemaphore>	s_copy; 					extPadding(s_copy)
	extern RtArray<VkSemaphore>	s_clear; 					extPadding(s_clear)
	extern RtArray<VkFence>		imageRenderedFence; 		extPadding(imageRenderedFence)
	extern int32				renderCurrentFrame;		 	extPadding(renderCurrentFrame)
	extern RtArray<obj::Base*>	objUpdates2D; 				extPadding(objUpdates2D)
	extern FenceDE				pendingObjectUpdatesFence; 	extPadding(pendingObjectUpdatesFence)




	void init(const bool vUseVSync);
	luxDebug(void createDebugMessenger( ));
	void drawFrame( );
	void cleanup( );
	static void	framebufferResizeCallback(GLFWwindow* pWindow, int32 vWidth, int32 vHeight) {
		wnd::windowResizeFence.startSecond( );  //from the last call of this function
		out::renderFramebufferResized = true;
		wnd::windowResizeFence.endSecond( );
	}



	VkFormat findSupportedFormat(const RtArray<VkFormat>* pCandidates, const VkImageTiling vTiling, const VkFormatFeatureFlags vFeatures);
	uint32   findMemoryType(const uint32 vTypeFilter, const VkMemoryPropertyFlags vProperties);
	luxDebug(static VKAPI_ATTR VkBool32 VKAPI_CALL vulkanOutputCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
		if((messageSeverity | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
			Failure printf("Validation layer error: ");
		}
		else if((messageSeverity | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
			Warning printf("Validation layer warning: ");
		}
		else { Normal printf("Validation layer info: "); }
		printf("%s", pCallbackData->pMessage); NormalNoNl;
		return VK_FALSE;
	})
}