#include "Lynx/Types/Object/2D/Border2.hpp"




namespace lnx::obj{
	void Border2::onSpawn(Window& pWindow) {
		Obj2_b::onSpawn(pWindow); //Initialize all the children
		data.create(pWindow.fOut_g, (vram::ptr<u32v2, VRam, Storage>)pWindow.wSize_g, pWindow.zBuff_g, { pWindow.width / 32 + 1, 1u, 1u }, pWindow);
		//FIXME                     ^ UNIFY WINDOW TYPE
	}
}