#include "Lynx/Types/Object/2D/Border2.hpp"
#include "Lynx/Core/Render/Render.hpp"

//TODO USE RenderCore STRUCT INSIDE WINDOWS TO RENDER DATA


namespace lnx::obj{
	void Border2::onSpawn(core::RenderCore& pRenderCore) { //FIXME PASS RENDER CORE
		obj2_b::onSpawn(pRenderCore); //Initialize all the children
		data.create(
			pRenderCore.fOut_g,
			pRenderCore.wSize_g,
			pRenderCore.zBuff_g,
			{ 4u, 1u, 1u }, //FIXME UPDATE WITH WINDOW OR USE WINDOW SIZE BUFFER PROPERLY. or just run once for any border and use the maximum size
			pRenderCore
		);
	}
}