
//This file was generated automatically. Changes could be overwritten without notice
#pragma once
#include <LuxEngine/Types/Vectors/Vectors.hpp>
#include <LuxEngine/Types/Pointer.hpp>
#include <LuxEngine/Types/VPointer.hpp>
#include <LuxEngine/Types/Shader_t.hpp>



namespace lux::shd{
	struct Border2D : public Shader_b {
		struct colorOutput__t : public ShaderElm_b<Storage> {
			colorOutput__t() {
				ShaderElm_b::bind = 0;
			}
			alwaysInline f32v4& colorOutput() { return *(f32v4*)ShaderElm_b::data; }
		} colorOutput_;


		struct windowSize__t : public ShaderElm_b<Storage> {
			windowSize__t() {
				ShaderElm_b::vdata.realloc(8);
				ShaderElm_b::data.realloc(8);
				ShaderElm_b::bind = 1;
			}
			alwaysInline u32v2& window() { return *(u32v2*)ShaderElm_b::data; }
		} windowSize_;


		struct zBuffer__t : public ShaderElm_b<Storage> {
			zBuffer__t() {
				ShaderElm_b::bind = 2;
			}
			alwaysInline u32& zBuffer() { return *(u32*)ShaderElm_b::data; }
		} zBuffer_;


		struct objData__t : public ShaderElm_b<Uniform> {
			objData__t() {
				ShaderElm_b::vdata.realloc(32);
				ShaderElm_b::data.realloc(32);
				ShaderElm_b::bind = 3;
			}
			//Position of the top-left corner
			alwaysInline f32v2& ffp() { return *(f32v2*)ShaderElm_b::data; }
			//Position of the bottom-right corner
			alwaysInline f32v2& fsp() { return *(f32v2*)(ShaderElm_b::data + 8); }
			//TODO
			alwaysInline u32& ID() { return *(u32*)(ShaderElm_b::data + 16); }
		} objData_;


		void create(vram::ptr<f32v4, VRam, Storage> pColorOutput, vram::ptr<u32, VRam, Storage> pZBuffer){

		}
	};
}