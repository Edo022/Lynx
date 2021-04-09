
#include "Lynx/Core/Core.hpp"
#include "Lynx/Core/Render/Shaders/Shader.hpp"
#include "Lynx/Types/Object/2D/Line2.hpp"




namespace lnx::obj{
	void Line2::onSpawn(Window& pWindow) {
		Obj2_b::onSpawn(pWindow); //Initialize all the children


		pWindow.addObject_m.lock();
			data.create(pWindow.fOut_g, pWindow.wSize_g, pWindow.zBuff_g);
			data.createDescriptorSets();
			data.createCommandBuffers(LNX_DEF_SHADER_2D_LINE, 512, 1, 1, pWindow);		//FIXME 4 group size? wtf?
			//FIXME                                        ^ CALCULATE GROUP SIZE AT RUNTIME

			pWindow.swp.shadersCBs.add(data.commandBuffers[0]);
		pWindow.addObject_m.unlock();
	}
}