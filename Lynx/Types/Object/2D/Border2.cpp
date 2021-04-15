#include "Lynx/Types/Object/2D/Border2.hpp"




namespace lnx::obj{
	void Border2::onSpawn(Window& pWindow) {
		Obj2_b::onSpawn(pWindow); //Initialize all the children
		data.create(
			pWindow.fOut_g,
			pWindow.wSize_g,
			pWindow.zBuff_g,
			{ pWindow.swp.createInfo.imageExtent.width / 32 + 1, 1u, 1u }, //FIXME UPDATE WITH WINDOW OR USE WINDOW SIZE BUFFER PROPERLY. or just run once for any border and use the maximum size
			pWindow
		);
	}
}