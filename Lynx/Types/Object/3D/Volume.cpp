#include "Lynx/Core/Core.hpp"
#include "Lynx/Core/Render/Shaders/Shader.hpp"
#include "Lynx/Core/Render/Render.hpp"
#include "Lynx/Types/Object/3D/Volume.hpp"




namespace lnx{
    void Volume::onSpawn(core::RenderCore& pRenderCore) {
        //TODO specify that those functions must be called or automatize it
		obj3_b::onSpawn(pRenderCore); //Initialize all the children
		data.spawn(pRenderCore.fOut_g, pRenderCore.wSize_g, { 1u, 1u, 1u }, pRenderCore); //FIXME CALCULATE GROUP SIZE AT RUNTIME
	}
}
