#include "LuxEngine/Types/LuxObject/2D/2DBorder.hpp"




namespace lux::obj{
	void Border2D::init(Window& pWindow) {
		Base2D::init(pWindow); //Initialize all the children


		core::c::shaders::newShader(
			RtArray<vram::Alloc_b<int32>>{
				pWindow.fOut_g,
				pWindow.wSize_g,
				pWindow.zBuff_g,
				(vram::Alloc_b<int32>)(render.localData)
			},
			render.shaderLayout,
			pWindow.width / 32 + 1, 1, 1,
			pWindow
		);
	}
}