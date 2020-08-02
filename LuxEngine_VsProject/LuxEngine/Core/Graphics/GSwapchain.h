
#pragma once






namespace lux::core::g{
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR		capabilities;
		lux::Array<VkSurfaceFormatKHR>	formats;
		lux::Array<VkPresentModeKHR>	presentModes;
	};




	//extern graphicsDevice		graphics;		//Main graphics device
	//extern computeDevice		compute;		//Main compute device
	//extern Array<computeDevice>	secondary;		//Secondary compute devices


	void						swapchainCreate();
	void						swapchainRecreate(const bool vWindowResized);
	void						swapchainCleanup();
	VkSurfaceFormatKHR			swapchainChooseSurfaceFormat(const lux::Array<VkSurfaceFormatKHR>* pAvailableFormats);
	VkPresentModeKHR			swapchainChoosePresentMode(const lux::Array<VkPresentModeKHR>* pAvailablePresentModes);
	VkExtent2D					swapchainChooseExtent(const VkSurfaceCapabilitiesKHR* pCapabilities);
	SwapChainSupportDetails		swapchainQuerySupport(const VkPhysicalDevice vDevice);
}
