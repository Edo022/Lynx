#pragma once
#include "Lynx/Core/Render/Shaders/Shader.hpp"
#include "Lynx/Types/Object/2D/Obj2_b.hpp"
#include "Lynx/shaders/Border2.hpp"



namespace lnx::obj{
	struct Border2 : public Obj2_b<2>{
		shd::Border2 data;


		virtual ram::ptr<char> getShData(){ return data._data.data; }
		virtual vram::Alloc_b<char> getShVData(){ return data._data.vdata; }


		Border2() {
			data._data.ffp() = { .0f, .0f };
			data._data.fsp() = { .0f, .0f };
			data._data.ID() = (uint32)common.ID;
		}
		Border2(const f32v2& vFp, const f32v2& vSp) {
			data._data.ffp() = vFp;
			data._data.fsp() = vSp;
			data._data.ID() = (uint32)common.ID;
		}

		/**
		 * @brief Initializes the GPU data that allows the window to render the object
		 */
		virtual void onSpawn(Window& pWindow) override;
	};
}