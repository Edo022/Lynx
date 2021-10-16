#pragma once
#include "Lynx/Core/Render/Shaders/Shader.hpp"
#include "Lynx/Types/Object/Obj_b.hpp"
#include "Generated/Shaders/Border2.hpp"



namespace lnx::obj{
	struct Border2 : public RenderObj2{
		shd::gsi::Border2 data;


		virtual ram::ptr<char> getShData() override { return data.data.data; }
		virtual vram::Alloc_b<char> getShVData() override { return data.data.vdata; }


		Border2() {
			data.data.lFfp() = { .0f, .0f };
			data.data.lFsp() = { .0f, .0f };
			data.data.lID() = (uint32)ID;
		}
		Border2(const f32v2& vFp, const f32v2& vSp) {
			data.data.lFfp() = vFp;
			data.data.lFsp() = vSp;
			data.data.lID() = (uint32)ID;
		}

		/**
		 * @brief Initializes the GPU data that allows the window to render the object
		 */
		virtual void onSpawn(core::RenderCore& pRenderCore) override;
	};
}