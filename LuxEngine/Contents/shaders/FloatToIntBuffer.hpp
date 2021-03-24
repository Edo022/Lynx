
//This file was generated automatically. Changes could be overwritten without notice
#pragma once
#include <LuxEngine/Types/Vectors/Vectors.hpp>
#include <LuxEngine/Types/Pointer.hpp>
#include <LuxEngine/Types/VPointer.hpp>
#include <LuxEngine/Types/Shader_t.hpp>



namespace lux::shd{
	struct FloatToIntBuffer{
		struct src_ : public Shader_b<Storage> {
			src_() {
				Shader_b::bind = 0;
			}
			alwaysInline f32v4& src() { return *(f32v4*)Shader_b::data; }
		};


		struct dst_ : public Shader_b<Storage> {
			dst_() {
				Shader_b::bind = 1;
			}
			alwaysInline u32& dst() { return *(u32*)Shader_b::data; }
		};


		struct zBuffer_ : public Shader_b<Storage> {
			zBuffer_() {
				Shader_b::bind = 2;
			}
			alwaysInline u32& zBuffer() { return *(u32*)Shader_b::data; }
		};


		struct windowSize_ : public Shader_b<Storage> {
			windowSize_() {
				Shader_b::vdata.realloc(8);
				Shader_b::data.realloc(8);
				Shader_b::bind = 3;
			}
			alwaysInline u32& width() { return *(u32*)Shader_b::data; }
			alwaysInline u32& height() { return *(u32*)(Shader_b::data + 4); }
		};
	};
}