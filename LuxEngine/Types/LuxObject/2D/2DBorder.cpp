
#include "LuxEngine/Types/LuxObject/2D/2DBorder.hpp"



namespace lux::obj{
	void Border2D::init( ) {
		cellSize = 40;
		luxInitObject(2, BORDER);							// count   | range         | chunk
		fp = (vec2f32*)(render.data + 0);					//    8    |    0  - 7     |    0 +
		sp = (vec2f32*)(render.data + 8);					//    8    |    8  - 15    |    0
		*(uint32*)(render.data + 16) = (uint32)common.ID;	//    4    |    16 - 19    |    1 +
		//4x trailing padding								//    4    |    20 - 23

		//TODO add static objects with specific workgroup count
		core::c::shaders::newShader(
			{
				core::render::wnd::gpuCellWindowOutput,
				core::render::wnd::gpuCellWindowSize,
				core::render::wnd::gpuCellWindowZBuffer,
				render.localData
			},
			render.shaderLayout,
			core::render::wnd::width / 32 + 1, 1, 1
		);
	}
}