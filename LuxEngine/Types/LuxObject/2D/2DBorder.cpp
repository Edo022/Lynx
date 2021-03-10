#include "LuxEngine/Types/LuxObject/2D/2DBorder.hpp"



namespace lux::obj{
	void Border2D::init(Window& pWindow) {
		Base2D::init(pWindow); //Initialize all the children

		// cellSize = 40;	//FIXME MOVED TO CONSTRUCTOR
		// luxInitObject(2, BORDER);							// count   | range         | chunk	//FIXME MOVED TO CONSTRUCTOR
		// fp = (f32v2*)(render.data + 0);					//    8    |    0  - 7     |    0 +	//FIXME MOVED TO CONSTRUCTOR
		// sp = (f32v2*)(render.data + 8);					//    8    |    8  - 15    |    0	//FIXME MOVED TO CONSTRUCTOR
		// *(uint32*)(render.data + 16) = (uint32)common.ID;	//    4    |    16 - 19    |    1 +	//FIXME MOVED TO CONSTRUCTOR
		// //4x trailing padding								//    4    |    20 - 23	//FIXME MOVED TO CONSTRUCTOR

		//TODO add static objects with specific workgroup count
		// core::c::shaders::newShader(
		// 	RtArray<vram::Alloc_b<int32>>{
		// 		lux::window.fOut_G,
		// 		lux::window.wSize_g,
		// 		lux::window.zBuff_g,
		// 		(vram::Alloc_b<int32>)(render.localData)
		// 	},
		// 	render.shaderLayout,
		// 	lux::window.width / 32 + 1, 1, 1,
		// 	lux::window
		// );
		core::c::shaders::newShader(
			RtArray<vram::Alloc_b<int32>>{
				pWindow.fOut_G,
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