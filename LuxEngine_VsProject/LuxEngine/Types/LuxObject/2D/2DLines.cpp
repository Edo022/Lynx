
#include "LuxEngine/Core/Core.h"
#include "LuxEngine/Core/Compute/CShader.h"
#include "LuxEngine/Types/LuxObject/2D/2DLines.h"



namespace lux::obj{
	void Line2D::init( ){
		luxInitObject(2, LINE);
		//fp = (vec2i32*)((int8*)render.dataPtr + 0);						//    8    |    0  - 7     |    0 +
		//sp = (vec2i32*)((int8*)render.dataPtr + 8);						//    8    |    8  - 15    |    0
		//fc = (vec4f32*)((int8*)render.dataPtr + 16);					//    16   |    16 - 31    |    1 +
		//sc = (vec4f32*)((int8*)render.dataPtr + 32);					//    16   |    32 - 47    |    2 +
		//fw = (float32*)((int8*)render.dataPtr + 48);					//    4    |    48 - 51    |    3 +
		//sw = (float32*)((int8*)render.dataPtr + 52);					//    4    |    52 - 55    |    3
		//*(uint32*)((int8*)render.dataPtr + 56) = (uint32)common.ID;		//    4    |    56 - 59    |    3
		fp = (vec2f32*)((int8*)render.data + 0);						//    8    |    0  - 7     |    0 +
		sp = (vec2f32*)((int8*)render.data + 8);						//    8    |    8  - 15    |    0
		fc = (vec4f32*)((int8*)render.data + 16);					//    16   |    16 - 31    |    1 +
		sc = (vec4f32*)((int8*)render.data + 32);					//    16   |    32 - 47    |    2 +
		fw = (float32*)((int8*)render.data + 48);					//    4    |    48 - 51    |    3 +
		sw = (float32*)((int8*)render.data + 52);					//    4    |    52 - 55    |    3
		*(uint32*)((int8*)render.data + 56) = (uint32)common.ID;		//    4    |    56 - 59    |    3
		//*(uint32*)((int8*)render.data + 56+8) = (uint32)common.ID;		//    4    |    56 - 59    |    3
		//4x trailing padding											//    4    |    60 - 63

		//TODO just update the shaders when needed with the right workgroup count
		//TODO remove useless out-of-range check in shaders
		//core::c::shaders::newShader({ core::g::wnd::gpuCellWindowOutput, core::g::wnd::gpuCellWindowSize, core::g::wnd::gpuCellWindowZBuffer, data }, shaderLayout, 4, 1, 1);
		core::c::shaders::newShader({ core::g::wnd::gpuCellWindowOutput, core::g::wnd::gpuCellWindowSize, core::g::wnd::gpuCellWindowZBuffer, render.localData }, render.shaderLayout, 4, 1, 1);
	}
}
