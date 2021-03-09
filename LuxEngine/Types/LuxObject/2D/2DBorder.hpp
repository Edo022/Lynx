#pragma once
#include "LuxEngine/Core/Render/Shaders/Shader.hpp"
#include "LuxEngine/Types/LuxObject/LuxObject.hpp"



namespace lux::obj{
	struct Border2D : public Base2D{
		f32v2* fp = nullptr;
		f32v2* sp = nullptr;


		Border2D() {
			init();
			*fp = { .0f, .0f };
			*sp = { .0f, .0f };
		}
		Border2D(const f32v2& vFp, const f32v2& vSp) {
			init();
			*fp = vFp;
			*sp = vSp;
		}
		void init();
	};
}