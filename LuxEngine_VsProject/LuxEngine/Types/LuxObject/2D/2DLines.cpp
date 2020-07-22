
#include "LuxEngine/Types/LuxObject/2D/2DLines.h"




namespace lux::obj {
	//Updates the object data in the GPU memory
	//This function should be called only after the object is begin spawned
	void Line2D::update( ) {
		//if(parent != nullptr) parent->setChildLimits(childIndex);
		vec2i32 pMinLim = vec2i32(1920 * 2, 1080) * minLim;
		vec2i32 pMaxLim = vec2i32(1920 * 2, 1080) * maxLim;

		vec2f32 hh = dist2D(minLim, maxLim);
		vec2f32 hh1 = (p0 - pMinLim);
		vec2i32 hh2 = (p0 - pMinLim) * dist2D(minLim, maxLim);
		*(vec2i32*)((int8*)cellPtr + 0) = (p0 - pMinLim) * dist2D(minLim, maxLim) + pMinLim;		//    8    |    0  - 7     |    0 +
		*(vec2i32*)((int8*)cellPtr + 8) = (p1 - pMinLim) * dist2D(minLim, maxLim) + pMinLim;		//    8    |    8  - 15    |    0
		*(vec4f32*)((int8*)cellPtr + 16) = col0;	//    16   |    16 - 31    |    1 +
		*(vec4f32*)((int8*)cellPtr + 32) = col1;	//    16   |    32 - 47    |    2 +
		*(float32*)((int8*)cellPtr + 48) = wd0;		//    4    |    48 - 51    |    3 +
		*(float32*)((int8*)cellPtr + 52) = wd1;		//    4    |    52 - 55    |    3
		//7x trailing padding						//    7    |    56 - 63

		//recalculateBoundary( );
		//recalculate boundary function in object base
	}
}
