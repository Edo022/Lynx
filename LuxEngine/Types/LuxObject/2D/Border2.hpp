#pragma once
#include "LuxEngine/Core/Render/Shaders/Shader.hpp"
#include "LuxEngine/Types/LuxObject/2D/Obj2_b.hpp"
#include "LuxEngine/Contents/shaders/Border2.hpp"



namespace lux::obj{
	struct Border2 : public Obj2_b{
		shd::Border2 data;


		virtual ram::ptr<char> getShData(){ return data.objData_.data; }
		virtual vram::Alloc_b<char> getShVData(){ return data.objData_.vdata; }


		Border2() {
			data.objData_.ffp() = { .0f, .0f };
			data.objData_.fsp() = { .0f, .0f };
			data.objData_.ID() = (uint32)common.ID;
		}
		Border2(const f32v2& vFp, const f32v2& vSp) {
			data.objData_.ffp() = vFp;
			data.objData_.fsp() = vSp;
			data.objData_.ID() = (uint32)common.ID;
		}

		/**
		 * @brief Initializes the GPU data that allows the window to render the object
		 */
		virtual void onSpawn(Window& pWindow) override;
	};
}