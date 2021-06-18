#include "Lynx/Types/Object/2D/Border2.hpp"

//TODO USE RenderCore STRUCT INSIDE WINDOWS TO RENDER DATA


namespace lnx::obj{
	void Border2::onSpawn(Window& pWindow) { //FIXME PASS RENDER CORE
		obj2::onSpawn(pWindow); //Initialize all the children
		data.create(
			pWindow.renderCore.fOut_g,
			pWindow.renderCore.wSize_g,
			pWindow.renderCore.zBuff_g,
			{ 4u, 1u, 1u }, //FIXME UPDATE WITH WINDOW OR USE WINDOW SIZE BUFFER PROPERLY. or just run once for any border and use the maximum size
			pWindow
		);
	}
}