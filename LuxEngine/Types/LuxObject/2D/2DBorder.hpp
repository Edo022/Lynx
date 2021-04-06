#pragma once
#include "LuxEngine/Core/Render/Shaders/Shader.hpp"
#include "LuxEngine/Types/LuxObject/2D/Obj2_b.hpp"
#include "LuxEngine/Contents/shaders/Border2D.hpp"



namespace lux::obj{
	struct Border2D : public Obj2_b{
		shd::Border2D data;


		virtual ram::ptr<char> getShData(){ return data.objData_.data; }
		virtual vram::Alloc_b<char> getShVData(){ return data.objData_.vdata; }


		Border2D() {
			common.objectType = LUX_OBJECT_TYPE_2D_BORDER;

			data.objData_.ffp() = { .0f, .0f };
			data.objData_.fsp() = { .0f, .0f };
			data.objData_.ID() = (uint32)common.ID;
		}
		Border2D(const f32v2& vFp, const f32v2& vSp) {
			common.objectType = LUX_OBJECT_TYPE_2D_BORDER;

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