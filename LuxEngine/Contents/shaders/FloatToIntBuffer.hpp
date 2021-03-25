
//This file was generated automatically. Changes could be overwritten without notice
#pragma once
#include <LuxEngine/Types/Vectors/Vectors.hpp>
#include <LuxEngine/Types/Pointer.hpp>
#include <LuxEngine/Types/VPointer.hpp>
#include <LuxEngine/Types/Shader_t.hpp>



namespace lux::shd{
	struct FloatToIntBuffer : public Shader_b {
		struct src__t : public ShaderElm_b<Storage> {
			src__t() {
				ShaderElm_b::bind = 0;
			}
			alwaysInline f32v4& src() { return *(f32v4*)ShaderElm_b::data; }
		} src_;


		struct dst__t : public ShaderElm_b<Storage> {
			dst__t() {
				ShaderElm_b::bind = 1;
			}
			alwaysInline u32& dst() { return *(u32*)ShaderElm_b::data; }
		} dst_;


		struct zBuffer__t : public ShaderElm_b<Storage> {
			zBuffer__t() {
				ShaderElm_b::bind = 2;
			}
			alwaysInline u32& zBuffer() { return *(u32*)ShaderElm_b::data; }
		} zBuffer_;


		struct windowSize__t : public ShaderElm_b<Storage> {
			windowSize__t() {
				ShaderElm_b::vdata.realloc(40);
				ShaderElm_b::data.realloc(40);
				ShaderElm_b::bind = 3;
			}
			alwaysInline u32& width() { return *(u32*)ShaderElm_b::data; }
			alwaysInline u32& height() { return *(u32*)(ShaderElm_b::data + 4); }
		} windowSize_;


		void create(vram::ptr<f32v4, VRam, Storage> pSrc, vram::ptr<u32, VRam, Storage> pDst, vram::ptr<u32, VRam, Storage> pZBuffer){
		}
	};
}