#pragma once
#include "LuxEngine/Core/Render/Shaders/Shader.hpp"
#include "LuxEngine/Types/LuxObject/LuxObject.hpp"



namespace lux::obj{
	struct Border2D : public Base2D{
		vec2f32* fp{ nullptr };
		vec2f32* sp{ nullptr };


		Border2D() {
			init();
			*fp = { .0f, .0f };
			*sp = { .0f, .0f };
		}
		Border2D(const vec2f32& vFp, const vec2f32& vSp) {
			init();
			*fp = vFp;
			*sp = vSp;
		}
		void init();
	};
}