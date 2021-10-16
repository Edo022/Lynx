#pragma once
#include "Lynx/Types/Object/Obj_b.hpp"
#include "Lynx/Core/Render/Buffers.hpp"
#include "Lynx/Core/Render/GCommands.hpp"
#include "Lynx/Core/Render/Render.hpp"
#include "Generated/Shaders/Line2.ilsl.hpp"
#include "Lynx/Core/IncludeVulkan.hpp"



//TODO per-children-group shader command buffers. static shaders are recreated when the objects change
namespace lnx::obj {
	/**
	 * @brief A bidimensional line with interpolated color and width.
	 *		Lines with 0 width or 0 alpha are not rendered
	 */
	struct Line2 : public RenderObj2 {
		/**
		 * @brief Initializes the GPU data that allows the window to render the object
		 */
		virtual void onSpawn(core::RenderCore& pRenderCore) override;

		f32v2 _fp0;		//First point of the line
		f32v2 _fp1;		//Second point of the line

		virtual ram::ptr<char> getShData() override { return data.data.data; }
		virtual vram::Alloc_b<char> getShVData() override { return data.data.vdata; }


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
			data.data.lCol0() = pFc;
			data.data.lCol1() = pSc;
			data.data.lWd0() = vFw;
			data.data.lWd1() = vSw;
			data.data.lID() = (uint32)ID;

		}


		//TODO add local-global-other coordinate system and conversions
		inline void setFp(const f32v2& vFp) { _fp0 = vFp; } //FIXME why tho? add an update function or an option to keep it updated by using a shared memory
		inline void setSp(const f32v2& vSp) { _fp1 = vSp; } //FIXME why tho? add an update function or an option to keep it updated by using a shared memory

		// void recalculateCoords() final {
		void onLimit() final override;

		shd::gsi::Line2 data;
	};
}

//TODO dual camera