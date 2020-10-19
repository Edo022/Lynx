#pragma once


#include "LuxEngine/Types/LuxObject/LuxObject.h"
#include "LuxEngine/Core/Compute/CBuffers.h"
#include "LuxEngine/Core/Devices.h"
#include "LuxEngine/Core/Graphics/GCommands.h"
#include "LuxEngine/Core/Graphics/Graphics.h"
// #include "LuxEngine/Types/Vectors/Vectors.h"
#include <vulkan/vulkan.h>


//TODO per-children-group shader command buffers. static shaders are recreated when the objects changes
namespace lux::obj {
	//A bidimensional line with interpolated color and width
	//Lines with width 0 or alpha 0 are not rendered
	struct Line2D : public Base2D {
		void init( );


		//Creates a Line2D object
		//Automatically initializes the object type and the GPU memory
		Line2D( ) {
			init( );

			*fp = { 0, 0 };
			*sp = { 0, 0, };
			*fc = { 0, 0, 0, 0 };
			*sc = { 0, 0, 0, 0 };
			*fw = { 0 };
			*sw = { 0 };
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
		Line2D(const vec2f32& pFp, const vec2f32& pSp, const vec4f32& pFc, const vec4f32& pSc, const float32 vFw, const float32 vSw) {
			init( );
			//TODO use full 64bit int or just 32 for all objects

			setFp(pFp);
			setSp(pSp);
			*fc = pFc;
			*sc = pSc;
			*fw = vFw;
			*sw = vSw;
		}


		//TODO calculate offset and cell count at runtime with an everything array
		inline int32 getCellSize( ) const final override { return 60; }

		//TODO add loca-global-other coordinate system and convertions
		inline void setFp(const vec2f32& vFp){ _fp = vFp; }
		inline void setSp(const vec2f32& vSp){ _sp = vSp; }


		void recalculateCoords( ) final override {
			*fp = _fp * dist2D(minLim, maxLim) + minLim;
			*sp = _sp * dist2D(minLim, maxLim) + minLim;
		}



	private:
		vec2f32* fp;		//First point of the line
		vec2f32* sp;		//Second point of the line
		vec2f32 _fp;		//First point of the line
		vec2f32 _sp;		//Second point of the line
	public:
		vec4f32* fc;		//Color of the first point
		vec4f32* sc;		//Color of the second point
		float32* fw;		//Width of the first point
		float32* sw;		//Width of the second point
	};
}

//TODO dual camera