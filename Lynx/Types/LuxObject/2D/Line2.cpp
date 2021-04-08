
#include "Lynx/Core/Core.hpp"
#include "Lynx/Core/Render/Shaders/Shader.hpp"
#include "Lynx/Types/LuxObject/2D/Line2.hpp"




namespace lux::obj{
	void Line2::onSpawn(Window& pWindow) {
		Obj2_b::onSpawn(pWindow); //Initialize all the children


		pWindow.addShaderFence.lock();
			data.create(pWindow.fOut_g, pWindow.wSize_g, pWindow.zBuff_g);
			data.createDescriptorSets();
			data.createCommandBuffers(LUX_DEF_SHADER_2D_LINE, 512, 1, 1, pWindow);		//FIXME 4 group size? wtf?
			//FIXME                                        ^ CALCULATE GROUP SIZE AT RUNTIME

			pWindow.swp.shadersCBs.add(data.commandBuffers[0]);
		pWindow.addShaderFence.unlock();
	}
}