
#include "2DBorder.h"



namespace lux::obj{
	void Border2D::init( ){
		cellSize = 40;
		luxInitObject(2, BORDER);							// count    | range         | chunk
		fp = (vec2f32*)(render.data + 0);					//    8    |    0  - 7     |    0 +
		sp = (vec2f32*)(render.data + 8);					//    8    |    8  - 15    |    0
		*(uint32*)(render.data + 16) = (uint32)common.ID;	//    4    |    16 - 19    |    1 +
		//4x trailing padding								//    4    |    20 - 23

		//TODO add static objects with specific workgroup count
		core::c::shaders::newShader({ core::g::wnd::gpuCellWindowOutput, core::g::wnd::gpuCellWindowSize, core::g::wnd::gpuCellWindowZBuffer, render.localData }, render.shaderLayout, core::g::wnd::width / 32 + 1, 1, 1);
	}
}