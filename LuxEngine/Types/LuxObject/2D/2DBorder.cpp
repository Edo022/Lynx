#include "LuxEngine/Types/LuxObject/2D/2DBorder.hpp"



namespace lux::obj{
	void Border2D::init() {
		cellSize = 40;
		luxInitObject(2, BORDER);							// count   | range         | chunk
		fp = (f32v2*)(render.data + 0);					//    8    |    0  - 7     |    0 +
		sp = (f32v2*)(render.data + 8);					//    8    |    8  - 15    |    0
		*(uint32*)(render.data + 16) = (uint32)common.ID;	//    4    |    16 - 19    |    1 +
		//4x trailing padding								//    4    |    20 - 23

		//TODO add static objects with specific workgroup count
		core::c::shaders::newShader(
			RtArray<vram::Alloc_b<int32>>{
				lux::window.fOut_G,
				lux::window.wSize_g,
				lux::window.zBuff_g,
				(vram::Alloc_b<int32>)(render.localData)
			},
			render.shaderLayout,
			lux::window.width / 32 + 1, 1, 1,
			lux::window
		);
	}
}