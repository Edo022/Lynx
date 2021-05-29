#pragma once
#define LNX_H_2DLINES
#include "Lynx/Types/Object/Obj_b.hpp"
#include "Lynx/Types/Object/2D/Obj2_b.hpp"
#include "Lynx/Core/Render/Buffers.hpp"
#include "Lynx/Core/Devices.hpp"
#include "Lynx/Core/Render/GCommands.hpp"
#include "Lynx/Core/Render/Render.hpp"
#include "Lynx/shaders/Line2.hpp"
#include "Lynx/Core/IncludeVulkan.hpp"



//TODO per-children-group shader command buffers. static shaders are recreated when the objects change
namespace lnx::obj {
	/**
	 * @brief A bidimensional line with interpolated color and width.
	 *		Lines with 0 width or 0 alpha are not rendered
	 */
	struct Line2 : public Obj2_b<2> {
		/**
		 * @brief Initializes the GPU data that allows the window to render the object
		 */
		virtual void onSpawn(Window& pWindow) override;

		f32v2 _fp0;		//First point of the line
		f32v2 _fp1;		//Second point of the line

		virtual ram::ptr<char> getShData() override { return data._data.data; }
		virtual vram::Alloc_b<char> getShVData() override { return data._data.vdata; }


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
			data._data.col0() = pFc;
			data._data.col1() = pSc;
			data._data.wd0() = vFw;
			data._data.wd1() = vSw;
			data._data.ID() = (uint32)common.ID;

		}


		//TODO add local-global-other coordinate system and conversions
		inline void setFp(const f32v2& vFp) { _fp0 = vFp; } //FIXME why tho? add an update function or an option to keep it updated by using a shared memory
		inline void setSp(const f32v2& vSp) { _fp1 = vSp; } //FIXME why tho? add an update function or an option to keep it updated by using a shared memory


		// void recalculateCoords() final {
		void onLimit() final override {
			Obj2_b::onLimit();
			dbg::checkCond(render.parentWindow && thr::self::thr() != render.parentWindow->renderCore.t.thr, "This function can only be called by the render thread.");
			data._data.fp0() = _fp0 * adist(this->minLim, this->maxLim) + this->minLim;
			data._data.fp1() = _fp1 * adist(this->minLim, this->maxLim) + this->minLim;
		}



		shd::Line2 data;
	};
}

//TODO dual camera