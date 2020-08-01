
#pragma once




namespace lux::core::g{
	extern Array<VkSemaphore>	drawFrameImageAquiredSemaphore;
	extern Array<VkSemaphore>	drawFrameObjectsRenderedSemaphore;
	extern Array<VkSemaphore>	drawFrameCopySemaphore;
	extern Array<VkSemaphore>	drawFrameClearSemaphore;
	extern Array<VkFence>		drawFrameImageRenderedFence;
	extern int32				renderCurrentFrame;


	void		graphicsInit(const bool vUseVSync, const float vFOV);
	void		graphicsCreateSyncObjs();
	void		graphicsCreateDebugMessenger();
	void		graphicsDrawFrame();
	void		graphicsCleanup();
	static 	void framebufferResizeCallback(GLFWwindow* pWindow, int32 vWidth, int32 vHeight) {
		windowResizeFence.startSecond( );  //from the last call of this function
		renderFramebufferResized = true;
		windowResizeFence.endSecond( );
	}



	VkFormat	graphicsFindSupportedFormat(const lux::Array<VkFormat>* pCandidates, const VkImageTiling vTiling, const VkFormatFeatureFlags vFeatures);
	uint32		graphicsFindMemoryType(const uint32 vTypeFilter, const VkMemoryPropertyFlags vProperties);
	luxDebug(static VKAPI_ATTR VkBool32 VKAPI_CALL graphicsDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
		if((messageSeverity | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) Failure
		else if((messageSeverity | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) Warning
			printf("Validation layer error\n %s", pCallbackData->pMessage);
		return VK_FALSE;
	})
}