#pragma once
#define LUX_H_2DLINES
#include "LuxEngine/Types/LuxObject/Obj_b.hpp"
#include "LuxEngine/Types/LuxObject/2D/Obj2_b.hpp"
#include "LuxEngine/Core/Render/Buffers.hpp"
#include "LuxEngine/Core/Devices.hpp"
#include "LuxEngine/Core/Render/GCommands.hpp"
#include "LuxEngine/Core/Render/Render.hpp"
#include "LuxEngine/shaders/Line2.hpp"
#include <vulkan/vulkan.hpp>



//FIXME USE GENERATED BINDING INSTEAD OF BUFFER POSITION
//TODO per-children-group shader command buffers. static shaders are recreated when the objects change
namespace lux::obj {
	/**
	 * @brief A bidimensional line with interpolated color and width.
	 *		Lines with 0 width or 0 alpha are not rendered
	 */
	struct Line2 : public Obj2_b {
		/**
		 * @brief Initializes the GPU data that allows the window to render the object
		 */
		virtual void onSpawn(Window& pWindow) override;

		f32v2 _fp0;		//First point of the line
		f32v2 _fp1;		//Second point of the line

		virtual ram::ptr<char> getShData(){ return data.lineData_.data; }
		virtual vram::Alloc_b<char> getShVData(){ return data.lineData_.vdata; }


		/**
		 * @brief Creates a Line2 object with the specified points, colors and widths.
		 *		Colors and widths are interpolated
		 * @param pFp First point of the line
		 * @param pSp Second point of the line
		 * @param pFc Color of the first point
		 * @param pSc Color of the second point
		 * @param vFw Width of the first point
		 * @param vSw Width of the second point
		 */
		Line2(const f32v2& pFp, const f32v2& pSp, const f32v4& pFc, const f32v4& pSc, const float32 vFw, const float32 vSw) {
			setFp(pFp);
			setSp(pSp);
			data.lineData_.col0() = pFc;
			data.lineData_.col1() = pSc;
			data.lineData_.wd0() = vFw;
			data.lineData_.wd1() = vSw;
			data.lineData_.ID() = (uint32)common.ID;

		}


		//TODO add local-global-other coordinate system and conversions
		inline void setFp(const f32v2& vFp) { _fp0 = vFp; } //FIXME why tho? add an update function or an option to keep it updated by using a shared memory
		inline void setSp(const f32v2& vSp) { _fp1 = vSp; } //FIXME why tho? add an update function or an option to keep it updated by using a shared memory


		void recalculateCoords() final {
			data.lineData_.fp0() = _fp0 * adist(minLim, maxLim) + minLim;
			data.lineData_.fp1() = _fp1 * adist(minLim, maxLim) + minLim;
		}



		shd::Line2 data;
	};
}

//TODO dual camera