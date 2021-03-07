#pragma once
#define LUX_H_2DLINES
#include "LuxEngine/Types/LuxObject/LuxObject.hpp"
#include "LuxEngine/Core/Render/Buffers.hpp"
#include "LuxEngine/Core/Devices.hpp"
#include "LuxEngine/Core/Render/GCommands.hpp"
#include "LuxEngine/Core/Render/Render.hpp"
#include <vulkan/vulkan.h>




//TODO per-children-group shader command buffers. static shaders are recreated when the objects changes
namespace lux::obj {
	/**
	 * @brief A bidimensional line with interpolated color and width.
	 *		Lines with 0 width or 0 alpha are not rendered
	 */
	struct Line2D : public Base2D {
		void init();




		/**
		 * @brief Creates a Line2D object with the specified points, colors and widths.
		 *		Colors and widths are interpolated
		 * @param pFp First point of the line
		 * @param pSp Second point of the line
		 * @param pFc Color of the first point
		 * @param pSc Color of the second point
		 * @param vFw Width of the first point
		 * @param vSw Width of the second point
		 */
		Line2D(const vec2f32& pFp, const vec2f32& pSp, const vec4f32& pFc, const vec4f32& pSc, const float32 vFw, const float32 vSw) {
			init();

			setFp(pFp);
			setSp(pSp);
			*fc = pFc;
			*sc = pSc;
			*fw = vFw;
			*sw = vSw;
		}


		//TODO calculate offset and cell count at runtime
		// inline int32 getCellSize() const final { return 60; }

		//TODO add loca-global-other coordinate system and convertions
		inline void setFp(const vec2f32& vFp) { _fp = vFp; } //FIXME why tho? add an update function or an option to keep it updated by using a shared memory
		inline void setSp(const vec2f32& vSp) { _sp = vSp; } //FIXME why tho? add an update function or an option to keep it updated by using a shared memory


		void recalculateCoords() final {
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