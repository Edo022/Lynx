
//####################################################################################
// This file was generated automatically. Changes could be overwritten without notice
//####################################################################################

#pragma once
#include "Lynx/Core/Render/Shaders/Shader_t.hpp"



namespace lnx::shd{
	struct FloatToIntBuffer : public Shader_b {
		static Shader_b::Layout layout;
		static uint32 pipelineIndex;


		struct _src_t : public ShaderElm_b<eStorage> {
			_src_t() {
				ShaderElm_b::bind = 0;
			}
		} _src;


		struct _dst_t : public ShaderElm_b<eStorage> {
			_dst_t() {
				ShaderElm_b::bind = 1;
			}
		} _dst;


		struct _wsize_t : public ShaderElm_b<eStorage> {
			_wsize_t() {
				ShaderElm_b::bind = 2;
			}
		} _wsize;


		struct _zbuff_t : public ShaderElm_b<eStorage> {
			_zbuff_t() {
				ShaderElm_b::bind = 3;
			}
		} _zbuff;


		void create(vram::ptr<f32v4, eVRam, eStorage> pSrc, vram::ptr<u32, eVRam, eStorage> pDst, vram::ptr<u32v2, eVRam, eStorage> pWsize, vram::ptr<u32, eVRam, eStorage> pZbuff, const u32v3 vGroupCount, core::RenderCore& pRenderCore);
		void createDescriptorSets();
		void createCommandBuffers(const u32v3 vGroupCount, core::RenderCore& pRenderCore);
		void updateCommandBuffers(const u32v3 vGroupCount, core::RenderCore& pRenderCore);
		void destroy();
	};
}//TODO remove local data in external bindings