#include "LuxEngine/Types/LuxObject/2D/2DBorder.hpp"




namespace lux::obj{
	void Border2D::init(Window& pWindow) {
		Obj2_b::init(pWindow); //Initialize all the children


		core::c::shaders::newShader(
			RtArray<vram::Alloc_b<int32>>{
				(vram::Alloc_b<int32>)pWindow.fOut_g,
				(vram::Alloc_b<int32>)pWindow.wSize_g,
				(vram::Alloc_b<int32>)pWindow.zBuff_g,
				// (vram::Alloc_b<int32>)(render.localData)
				(vram::Alloc_b<int32>)(data.objData_.vdata)
			},
			render.shaderLayout,
			(u32)pWindow.width / 32 + 1, 1, 1,
			pWindow
		);
	}
}