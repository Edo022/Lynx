
#include "Lynx/Core/Core.hpp"
#include "Lynx/Core/Render/Shaders/Shader.hpp"
#include "Lynx/Types/Object/2D/Line2.hpp"




namespace lnx::obj{
	void Line2::onSpawn(Window& pWindow) {
		// Obj2_b<2>::onSpawn(pWindow); //Initialize all the children
		Obj2_b::onSpawn(pWindow); //Initialize all the children
		data.create(pWindow.fOut_g, pWindow.wSize_g, pWindow.zBuff_g, { 512u, 1u, 1u }, pWindow); //FIXME CALCULATE GROUP SIZE AT RUNTIME
		//BUG^ UNCOMMENT
	}
}