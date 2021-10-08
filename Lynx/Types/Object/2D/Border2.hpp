#pragma once
#include "Lynx/Core/Render/Shaders/Shader.hpp"
#include "Lynx/Types/Object/Obj_b.hpp"
#include "Generated/Shaders/Border2.gsi.hpp"



namespace lnx::obj{
	struct Border2 : public RenderObj2{
		shd::gsi::Border2 data;


		virtual ram::ptr<char> getShData() override { return data.data.data; }
		virtual vram::Alloc_b<char> getShVData() override { return data.data.vdata; }


		Border2() {
			data.data.ffp = { .0f, .0f };
			data.data.fsp = { .0f, .0f };
			data.data.ID = (uint32)ID;
		}
		Border2(const f32v2& vFp, const f32v2& vSp) {
			data.data.ffp = vFp;
			data.data.fsp = vSp;
			data.data.ID = (uint32)ID;
		}

		/**
		 * @brief Initializes the GPU data that allows the window to render the object
		 */
		virtual void onSpawn(core::RenderCore& pRenderCore) override;
	};
}