#pragma once


#include "LuxEngine/Types/LuxObject/LuxObject.h"
#include <vulkan/vulkan.h>



namespace lux::obj {
	//A bidimensional line with interpolated color and width
	//Lines with width 0 or alpha 0 are not rendered
	struct Line2D : public Base2D {
		void init( );


		//Creates a Line2D object
		//Automatically initializes the object type and the GPU memory
		Line2D( ) {
			init();
		}

		//Creates a Line2D object with the specified points, colors and widths
		//Automatically initializes the object type and the GPU memory
		//Colors and widths are interpolated
		//*   pFp | first point of the line
		//*   pSp | second point of the line
		//*   pFc | color of the first point
		//*   pSc | color of the second point
		//*   vFw | width of the first point
		//*   vSw | width of the second point
		Line2D(const vec2i32& pFp, const vec2i32& pSp, const vec4f32& pFc, const vec4f32& pSc, const float32 vFw, const float32 vSw) :
			fp{ pFp }, sp{ pSp }, fc{ pFc }, sc{ pSc }, fw{ vFw }, sw{ vSw } {
			init( );
		}




		//Updates the object data in the GPU memory
		void update() final override {
			//if(parent != nullptr) parent->setChildLimits(childIndex);
			vec2i32 pMinLim = vec2i32(1920 * 2, 1080) * minLim;
			vec2i32 pMaxLim = vec2i32(1920 * 2, 1080) * maxLim;

			*(vec2i32*)((int8*)cellPtr + 0) = (fp - pMinLim) * dist2D(minLim, maxLim) + pMinLim;		//    8    |    0  - 7     |    0 +
			*(vec2i32*)((int8*)cellPtr + 8) = (sp - pMinLim) * dist2D(minLim, maxLim) + pMinLim;		//    8    |    8  - 15    |    0
			*(vec4f32*)((int8*)cellPtr + 16) = fc;														//    16   |    16 - 31    |    1 +
			*(vec4f32*)((int8*)cellPtr + 32) = sc;														//    16   |    32 - 47    |    2 +
			*(float32*)((int8*)cellPtr + 48) = fw;														//    4    |    48 - 51    |    3 +
			*(float32*)((int8*)cellPtr + 52) = sw;														//    4    |    52 - 55    |    3
			//7x trailing padding																		//    7    |    56 - 63

			//TODO recalculate limits for children
		}

		//TODO convert to variable. or convert the variable to function. idk
		inline int32 getCellSize() const final override { return 56; }

		vec2i32 fp{ 0, 0 };			//First point of the line
		vec2i32 sp{ 0, 0 };			//Second point of the line
		vec4f32 fc{ 0, 0, 0, 0 };	//Color of the first point
		vec4f32 sc{ 0, 0, 0, 0 };	//Color of the second point
		float32 fw{ 0 };			//Width of the first point
		float32 sw{ 0 };			//Width of the second point
	};
}

//TODO dual camera