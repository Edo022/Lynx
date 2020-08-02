
#pragma once



namespace lux::core::g{
	extern graphicsDevice		graphics;		//Main graphics device
	extern computeDevice		compute;		//Main compute device
	extern Array<computeDevice>	secondary;		//Secondary compute devices


	void	deviceGetPhysical();
	void	deviceCreateLogical(const _VkPhysicalDevice* pPD, VkDevice* pLD, lux::DynArray<VkQueue>* pComputeQueues);
	static	int32 deviceRate(const _VkPhysicalDevice* pDevice);
}
