
#include "Lynx/Core/Core.hpp"
#include "Lynx/Core/Render/Shaders/Shader.hpp"
#include "Lynx/Types/Object/2D/Line2.hpp"

//TODO DRAW OBJECT-RENDERCORE INTERACTION GRAPH
//TODO object-RenderCore. similar to client-server


namespace lnx::obj{
	void Line2::onSpawn(Window& pWindow) {//FIXME PASS RENDER CORE
		obj2::onSpawn(pWindow); //Initialize all the children
		data.create(pWindow.renderCore.fOut_g, pWindow.renderCore.wSize_g, pWindow.renderCore.zBuff_g, { 512u, 1u, 1u }, pWindow); //FIXME CALCULATE GROUP SIZE AT RUNTIME
	}
}