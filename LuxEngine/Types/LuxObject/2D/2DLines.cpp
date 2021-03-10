
#include "LuxEngine/Core/Core.hpp"
#include "LuxEngine/Core/Render/Shaders/Shader.hpp"
#include "LuxEngine/Types/LuxObject/2D/2DLines.hpp"



namespace lux::obj{
	//TODO calculate offset and cell count at runtime
	void Line2D::init(Window& pWindow) {
		Base2D::init(pWindow); //Initialize all the children

		// cellSize = 60;	//FIXME MOVED TO CONSTRUCTOR
		// luxInitObject(2, LINE);								// count   | range         | chunk	//FIXME MOVED TO CONSTRUCTOR
		// fp = (f32v2*)(render.data + 0);					//    8    |    0  - 7     |    0 +	//FIXME MOVED TO CONSTRUCTOR
		// sp = (f32v2*)(render.data + 8);					//    8    |    8  - 15    |    0	//FIXME MOVED TO CONSTRUCTOR
		// fc = (f32v4*)(render.data + 16);					//    16   |    16 - 31    |    1 +	//FIXME MOVED TO CONSTRUCTOR
		// sc = (f32v4*)(render.data + 32);					//    16   |    32 - 47    |    2 +	//FIXME MOVED TO CONSTRUCTOR
		// fw = (float32*)(render.data + 48);					//    4    |    48 - 51    |    3 +	//FIXME MOVED TO CONSTRUCTOR
		// sw = (float32*)(render.data + 52);					//    4    |    52 - 55    |    3	//FIXME MOVED TO CONSTRUCTOR
		// *(uint32*)(render.data + 56) = (uint32)common.ID;	//    4    |    56 - 59    |    3	//FIXME MOVED TO CONSTRUCTOR
		// //4x trailing padding								//    4    |    60 - 63	//FIXME MOVED TO CONSTRUCTOR

		//TODO add static objects with specific workgroup count
		// core::c::shaders::newShader(
		// 	RtArray<vram::Alloc_b<int32>>{ //FIXME REMOVE TYPE INFORMATION FROM BASE IF POSSIBLE IDK
		// 		lux::window.fOut_G,
		// 		lux::window.wSize_g,
		// 		lux::window.zBuff_g,
		// 		(vram::Alloc_b<int32>)(render.localData)
		// 	},
		// 	render.shaderLayout, 4, 1, 1,
		// 	lux::window
		// );
		core::c::shaders::newShader(
			RtArray<vram::Alloc_b<int32>>{ //FIXME REMOVE TYPE INFORMATION FROM BASE IF POSSIBLE IDK
				pWindow.fOut_G,
				pWindow.wSize_g,
				pWindow.zBuff_g,
				(vram::Alloc_b<int32>)(render.localData)
			},
			render.shaderLayout, 4, 1, 1,
			pWindow
		);
	}
}
