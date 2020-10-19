
#pragma once
#include "vulkan/vulkan.h"
#include "LuxEngine/macros.h"
#include "LuxEngine/Types/Integers/Integers.hpp"
#include "LuxEngine/Types/LuxFenceDE.h"
#include "LuxEngine/Types/Containers/LuxArray.h"
#include "LuxEngine/Core/Graphics/Window.h"
#include "LuxEngine/Core/Graphics/GOutput.h"
#include "LuxEngine/Types/LuxObject/LuxObject.h"
#define LUX_H_GRAPHICS

//TODO LIK ALL CPPS ONE BY ONE
//TODO REMOVE PREINIT MEMBER
//TODO USE LINK ORDER INSTEAD



namespace lux::core::g{
	extern DynArray<VkSemaphore>	drawFrameImageAquiredSemaphore;
	extern DynArray<VkSemaphore>	drawFrameObjectsRenderedSemaphore;
	extern DynArray<VkSemaphore>	drawFrameCopySemaphore;
	extern DynArray<VkSemaphore>	drawFrameClearSemaphore;
	extern DynArray<VkFence>		drawFrameImageRenderedFence;
	extern int32				renderCurrentFrame;
	extern DynArray<obj::Base*>	objUpdates2D;
	extern FenceDE				pendingObjectUpdatesFence;




	void			preInit( );
	void			init(const bool vUseVSync, const float vFOV);
	void			createSyncObjs( );
	luxDebug(void	createDebugMessenger( ));
	void			drawFrame( );
	void			cleanup( );
	static void	framebufferResizeCallback(GLFWwindow* pWindow, int32 vWidth, int32 vHeight) {
		wnd::windowResizeFence.startSecond( );  //from the last call of this function
		out::renderFramebufferResized = true;
		wnd::windowResizeFence.endSecond( );
	}



	VkFormat	findSupportedFormat(const DynArray<VkFormat>* pCandidates, const VkImageTiling vTiling, const VkFormatFeatureFlags vFeatures);
	uint32		findMemoryType(const uint32 vTypeFilter, const VkMemoryPropertyFlags vProperties);
	luxDebug(static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
		if((messageSeverity | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) Failure
		else if((messageSeverity | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) Warning
			printf("Validation layer error\n %s", pCallbackData->pMessage);
		return VK_FALSE;
	})
}