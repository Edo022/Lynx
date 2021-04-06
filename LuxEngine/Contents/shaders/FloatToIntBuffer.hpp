
//####################################################################################
// This file was generated automatically. Changes could be overwritten without notice
//####################################################################################

#pragma once
#include "LuxEngine/Core/Render/Shaders/Shader_t.hpp"



namespace lux::shd{
	struct FloatToIntBuffer : public Shader_b {
		static Shader_b::Layout layout;


		struct src__t : public ShaderElm_b<Storage> {
			src__t() {
				ShaderElm_b::bind = 0;
			}
		} src_;


		struct dst__t : public ShaderElm_b<Storage> {
			dst__t() {
				ShaderElm_b::bind = 1;
			}
		} dst_;


		struct zBuffer__t : public ShaderElm_b<Storage> {
			zBuffer__t() {
				ShaderElm_b::bind = 2;
			}
		} zBuffer_;


		struct windowSize__t : public ShaderElm_b<Storage> {
			windowSize__t() {
				ShaderElm_b::bind = 3;
			}
		} windowSize_;


		void create(vram::ptr<f32v4, VRam, Storage> pSrc, vram::ptr<u32, VRam, Storage> pDst, vram::ptr<u32, VRam, Storage> pZBuffer, vram::ptr<u32, VRam, Storage> pWidth){
			src_.vdata = (vram::ptr<char, VRam, Storage>)pSrc;
			dst_.vdata = (vram::ptr<char, VRam, Storage>)pDst;
			zBuffer_.vdata = (vram::ptr<char, VRam, Storage>)pZBuffer;
			windowSize_.vdata = (vram::ptr<char, VRam, Storage>)pWidth;
		}


		void createDescriptorSets();
		void createCommandBuffers(const ShaderLayout vShaderLayout, const uint32 vGroupCountX, const uint32 vGroupCountY, const uint32 vGroupCountZ, Window& pWindow);
		void updateCommandBuffers(const ShaderLayout vShaderLayout, const uint32 vGroupCountX, const uint32 vGroupCountY, const uint32 vGroupCountZ, Window& pWindow);
		void destroy();
	};
}//TODO remove local data in external bindings