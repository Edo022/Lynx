#include "Lynx/Types/Object/2D/Border2.hpp"




namespace lnx::obj{
	void Border2::onSpawn(Window& pWindow) {
		Obj2_b::onSpawn(pWindow); //Initialize all the children


		pWindow.addObject_m.lock();
			data.create(pWindow.fOut_g, (vram::ptr<u32v2, VRam, Storage>)pWindow.wSize_g, pWindow.zBuff_g);		//FIXME unify window size type
			data.createDescriptorSets();
			data.createCommandBuffers((u32)pWindow.width / 32 + 1, 1, 1, pWindow);

			pWindow.swp.shadersCBs.add(data.commandBuffers[0]);
		pWindow.addObject_m.unlock();
	}
}