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
		Line2D(const f32v2& pFp, const f32v2& pSp, const f32v4& pFc, const f32v4& pSc, const float32 vFw, const float32 vSw) {
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
		inline void setFp(const f32v2& vFp) { _fp = vFp; } //FIXME why tho? add an update function or an option to keep it updated by using a shared memory
		inline void setSp(const f32v2& vSp) { _sp = vSp; } //FIXME why tho? add an update function or an option to keep it updated by using a shared memory


		void recalculateCoords() final {
			*fp = _fp * adist(minLim, maxLim) + minLim;
			*sp = _sp * adist(minLim, maxLim) + minLim;
		}



	private:
		f32v2* fp;		//First point of the line
		f32v2* sp;		//Second point of the line
		f32v2 _fp;		//First point of the line
		f32v2 _sp;		//Second point of the line
	public:
		f32v4* fc;		//Color of the first point
		f32v4* sc;		//Color of the second point
		float32* fw;	//Width of the first point
		float32* sw;	//Width of the second point
	};
}

//TODO dual camera