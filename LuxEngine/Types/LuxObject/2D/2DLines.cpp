
#include "LuxEngine/Core/Core.hpp"
#include "LuxEngine/Core/Render/Shaders/Shader.hpp"
#include "LuxEngine/Types/LuxObject/2D/2DLines.hpp"



namespace lux::obj{
	//TODO calculate offset and cell count at runtime with an everything array
	void Line2D::init() {
		cellSize = 60;
		luxInitObject(2, LINE);								// count   | range         | chunk
		fp = (vec2f32*)(render.data + 0);					//    8    |    0  - 7     |    0 +
		sp = (vec2f32*)(render.data + 8);					//    8    |    8  - 15    |    0
		fc = (vec4f32*)(render.data + 16);					//    16   |    16 - 31    |    1 +
		sc = (vec4f32*)(render.data + 32);					//    16   |    32 - 47    |    2 +
		fw = (float32*)(render.data + 48);					//    4    |    48 - 51    |    3 +
		sw = (float32*)(render.data + 52);					//    4    |    52 - 55    |    3
		*(uint32*)(render.data + 56) = (uint32)common.ID;	//    4    |    56 - 59    |    3
		//4x trailing padding								//    4    |    60 - 63

		//TODO add static objects with specific workgroup count
		core::c::shaders::newShader(
			RtArray<vram::Alloc_b<int32>>{ //FIXME REMOVE TYPE INFORMATION FROM BASE IF POSSIBLE IDK
				lux::window.gpuCellWindowOutput,
				lux::window.gpuCellWindowSize,
				lux::window.gpuCellWindowZBuffer,
				(vram::Alloc_b<int32>)(render.localData)
			},
			render.shaderLayout, 4, 1, 1
		);
	}
}
