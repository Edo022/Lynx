
//####################################################################################
// This file was generated automatically. Changes could be overwritten without notice
//####################################################################################

#pragma once
#include "Lynx/Core/Render/Shaders/Shader_t.hpp"



namespace lnx::shd{
	struct FloatToIntBuffer : public Shader_b {
		static Shader_b::Layout layout;
		static uint32 pipelineIndex;


		struct _src_t : public ShaderElm_b<Storage> {
			_src_t() {
				ShaderElm_b::bind = 0;
			}
		} _src;


		struct _dst_t : public ShaderElm_b<Storage> {
			_dst_t() {
				ShaderElm_b::bind = 1;
			}
		} _dst;


		struct _wsize_t : public ShaderElm_b<Storage> {
			_wsize_t() {
				ShaderElm_b::bind = 2;
			}
		} _wsize;


		struct _zbuff_t : public ShaderElm_b<Storage> {
			_zbuff_t() {
				ShaderElm_b::bind = 3;
			}
		} _zbuff;


		void create(vram::ptr<f32v4, VRam, Storage> pSrc, vram::ptr<u32, VRam, Storage> pDst, vram::ptr<u32v2, VRam, Storage> pWsize, vram::ptr<u32, VRam, Storage> pZbuff, const u32v3 vGroupCount, Window& pWindow);
		void createDescriptorSets();
		void createCommandBuffers(const u32v3 vGroupCount, Window& pWindow);
		void updateCommandBuffers(const u32v3 vGroupCount, Window& pWindow);
		void destroy();
	};
}//TODO remove local data in external bindings