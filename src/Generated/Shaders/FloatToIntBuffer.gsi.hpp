
//####################################################################################
// This file was generated by Tools/Build/Generators/GenInterfaces
// Changes could be overwritten without notice
//####################################################################################

#pragma once
#include "Lynx/Core/Render/Shaders/ShaderInterface_t.hpp"
#include "Lynx/Core/Init.hpp"




namespace lnx::shd::gsi{
	_lnx_init_var_dec((InterfaceLayout), FloatToIntBuffer_layout);
	_lnx_init_var_dec((uint32),          FloatToIntBuffer_pipelineIndex);
	struct FloatToIntBuffer : public ShaderInterface_b {


		struct l_src : public ShaderElm_b<eStorage> {
			alwaysInline l_src(const bool vExt) : ShaderElm_b() {}
			inline l_src() : ShaderElm_b(0) {}
			inline l_src(const l_src& pSrc) {
				ShaderElm_b:: data = pSrc. data;
				ShaderElm_b::vdata = pSrc.vdata;
			}
			inline l_src& operator=(const l_src& pSrc) {
				ShaderElm_b:: data = pSrc. data;
				ShaderElm_b::vdata = pSrc.vdata;
				return *this;
				//FIXME automatically update render data after calling this function
			}
			inline l_src(const vram::ptr<auto, eVRam, eStorage>& pVPtr){
				vdata = (vram::ptr<char, eVRam, eStorage>)pVPtr;
			}
			inline auto& operator=(const vram::ptr<auto, eVRam, eStorage>& pVPtr){
				vdata = (vram::ptr<char, eVRam, eStorage>)pVPtr;
				return *this;
			}
			f32v4& src = *(f32v4*)(ShaderElm_b::data + 0);
			uint64 src_tmp_size = 0;
		};
		l_src src{ true };


		struct l_dst : public ShaderElm_b<eStorage> {
			alwaysInline l_dst(const bool vExt) : ShaderElm_b() {}
			inline l_dst() : ShaderElm_b(0) {}
			inline l_dst(const l_dst& pDst) {
				ShaderElm_b:: data = pDst. data;
				ShaderElm_b::vdata = pDst.vdata;
			}
			inline l_dst& operator=(const l_dst& pDst) {
				ShaderElm_b:: data = pDst. data;
				ShaderElm_b::vdata = pDst.vdata;
				return *this;
				//FIXME automatically update render data after calling this function
			}
			inline l_dst(const vram::ptr<auto, eVRam, eStorage>& pVPtr){
				vdata = (vram::ptr<char, eVRam, eStorage>)pVPtr;
			}
			inline auto& operator=(const vram::ptr<auto, eVRam, eStorage>& pVPtr){
				vdata = (vram::ptr<char, eVRam, eStorage>)pVPtr;
				return *this;
			}
			u32& dst = *(u32*)(ShaderElm_b::data + 0);
			uint64 dst_tmp_size = 0;
		};
		l_dst dst{ true };


		struct l_wsize : public ShaderElm_b<eStorage> {
			alwaysInline l_wsize(const bool vExt) : ShaderElm_b() {}
			inline l_wsize() : ShaderElm_b(256) {}
			inline l_wsize(const l_wsize& pWsize) {
				ShaderElm_b:: data = pWsize. data;
				ShaderElm_b::vdata = pWsize.vdata;
			}
			inline l_wsize& operator=(const l_wsize& pWsize) {
				ShaderElm_b:: data = pWsize. data;
				ShaderElm_b::vdata = pWsize.vdata;
				return *this;
				//FIXME automatically update render data after calling this function
			}
			inline l_wsize(const vram::ptr<auto, eVRam, eStorage>& pVPtr){
				vdata = (vram::ptr<char, eVRam, eStorage>)pVPtr;
			}
			inline auto& operator=(const vram::ptr<auto, eVRam, eStorage>& pVPtr){
				vdata = (vram::ptr<char, eVRam, eStorage>)pVPtr;
				return *this;
			}
			u32v2& wsize = *(u32v2*)(ShaderElm_b::data + 0);
		};
		l_wsize wsize{ true };


		struct l_zbuff : public ShaderElm_b<eStorage> {
			alwaysInline l_zbuff(const bool vExt) : ShaderElm_b() {}
			inline l_zbuff() : ShaderElm_b(0) {}
			inline l_zbuff(const l_zbuff& pZbuff) {
				ShaderElm_b:: data = pZbuff. data;
				ShaderElm_b::vdata = pZbuff.vdata;
			}
			inline l_zbuff& operator=(const l_zbuff& pZbuff) {
				ShaderElm_b:: data = pZbuff. data;
				ShaderElm_b::vdata = pZbuff.vdata;
				return *this;
				//FIXME automatically update render data after calling this function
			}
			inline l_zbuff(const vram::ptr<auto, eVRam, eStorage>& pVPtr){
				vdata = (vram::ptr<char, eVRam, eStorage>)pVPtr;
			}
			inline auto& operator=(const vram::ptr<auto, eVRam, eStorage>& pVPtr){
				vdata = (vram::ptr<char, eVRam, eStorage>)pVPtr;
				return *this;
			}
			u32& zbuff = *(u32*)(ShaderElm_b::data + 0);
			uint64 zbuff_tmp_size = 0;
		};
		l_zbuff zbuff{ true };


		void spawn(
			const l_src& pSrc,
			const l_dst& pDst,
			const l_wsize& pWsize,
			const l_zbuff& pZbuff,
			const u32v3 vGroupCount, core::RenderCore& pRenderCore
		);
		void createDescriptorSets();
		void createCommandBuffers(const u32v3 vGroupCount, core::RenderCore& pRenderCore);
		void updateCommandBuffers(const u32v3 vGroupCount, core::RenderCore& pRenderCore);
		void destroy();
	};


	_lnx_init_fun_dec(LNX_H_FLOATTOINTBUFFER);
}