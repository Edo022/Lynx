#pragma once


#include "LuxEngine/Types/LuxObject/LuxObject.h"
#include "LuxEngine/Core/Compute/CBuffers.h"
#include "LuxEngine/Core/Devices.h"
#include "LuxEngine/Core/Graphics/GCommands.h"
#include "LuxEngine/Core/Graphics/Graphics.h"
#include <vulkan/vulkan.h>


//TODO per-children-group shader command buffers. static shaders are recreated when the objects changes
namespace lux::obj {
	//A bidimensional line with interpolated color and width
	//Lines with width 0 or alpha 0 are not rendered
	struct Line2D : public Base2D {
		void init( );


		//Creates a Line2D object
		//Automatically initializes the object type and the GPU memory
		Line2D( ) {
			init( );

			*fp = { 0, 0 };
			*sp = { 0, 0, };
			*fc = { 0, 0, 0, 0 };
			*sc = { 0, 0, 0, 0 };
			*fw = { 0 };
			*sw = { 0 };
		}

		//Creates a Line2D object with the specified points, colors and widths
		//Automatically initializes the object type and the GPU memory
		//Colors and widths are interpolated
		//*   pFp | first point of the line
		//*   pSp | second point of the line
		//*   pFc | color of the first point
		//*   pSc | color of the second point
		//*   vFw | width of the first point
		//*   vSw | width of the second point
		Line2D(const vec2f32& pFp, const vec2f32& pSp, const vec4f32& pFc, const vec4f32& pSc, const float32 vFw, const float32 vSw) {
			init( );
			//TODO use full 64bit int or just 32 for all objects

			setFp(pFp);
			setSp(pSp);
			*fc = pFc;
			*sc = pSc;
			*fw = vFw;
			*sw = vSw;
		}

		//TODO lux::mem::cpy
		//TODO unwrap until n < 16, than switch on n and unwrap n-times


		//TODO automatically calculate the offset of the variables with device's minStorageBufferOffset and minUniformBufferOffset
		//TODO convert to variable. or convert the variable to function. idk
		//TODO calculate offset and cell size at runtime with an everything array
		inline int32 getCellSize( ) const final override { return 60; }
		//inline int32 getCellSize( ) const final override { return 80; }


		//Updates the object data in the GPU memory
		//void update( ) final override {
		//	//TODO USE IN-SHADER MATRIX TRANSFORM
		//	vec2i32 pMinLim = vec2i32(1920 * 2, 1080) * minLim;
		//	*fp = (fp_temp - pMinLim) * dist2D(minLim, maxLim) + pMinLim;
		//	*sp = (sp_temp - pMinLim) * dist2D(minLim, maxLim) + pMinLim;

		//	//void* h = core::c::buffers::gpuCellMap(render.data);
		//	//void* h2 = core::c::buffers::gpuCellMap(render.localData);
		//	//memcpy(h2, h, getCellSize( ));


		//	//VkCommandBuffer cb = core::g::cmd::beginSingleTimeCommands( );
		//	//vkCmdUpdateBuffer(
		//	//	cb, core::c::buffers::CBuffers[getBufferIndex(render.localData)].buffer,
		//	//	getCellOffset(&core::dvc::compute.PD, render.localData), getCellSize(), h
		//	//);
		//	//core::g::cmd::endSingleTimeCommands(cb);
		//	////core::g::pendingObjectUpdatesFence.startSecond();
		//	////if(render.updated){
		//	////	render.updated = false;
		//	////	core::g::objUpdates2D.add(this);
		//	////}
		//	////core::g::pendingObjectUpdatesFence.endSecond();
		//	//if(core::c::buffers::CBuffers[getBufferIndex(data)].isMapped) vkUnmapMemory(core::dvc::compute.LD, core::c::buffers::CBuffers[getBufferIndex(data)].memory);
		//	//if(core::c::buffers::CBuffers[getBufferIndex(data)].isMapped) vkUnmapMemory(core::dvc::compute.LD, core::c::buffers::CBuffers[getBufferIndex(localData)].memory);
			//updateBase();
		//}

		inline void setFp(const vec2f32& vFp){
			_fp = vFp;
			//vec2i32 pMinLim = vec2i32(1920 * 2, 1080) * minLim;
			//*fp = (vFp - pMinLim) * dist2D(minLim, maxLim) + pMinLim;
			////*fp = vFp;
			//update( );
		}
		inline void setSp(const vec2f32& vSp){
			_sp = vSp;
			//vec2i32 pMinLim = vec2i32(1920 * 2, 1080) * minLim;
			//*sp = (vSp - pMinLim) * dist2D(minLim, maxLim) + pMinLim;
			////*sp = vSp;
			//update( );
		}


		void recalculateCoords( ) final override {
			//vec2i32 pMinLim = vec2i32(core::g::swapchain::swapchainExtent.width, core::g::swapchain::swapchainExtent.height) * minLim;
			//*fp = (_fp - pMinLim) * dist2D(minLim, maxLim) + pMinLim;
			//*sp = (_sp - pMinLim) * dist2D(minLim, maxLim) + pMinLim;

			//vec2i32 pMinLim = vec2i32(core::g::swapchain::swapchainExtent.width, core::g::swapchain::swapchainExtent.height) * minLim;
			*fp = _fp * dist2D(minLim, maxLim) + minLim;
			*sp = _sp * dist2D(minLim, maxLim) + minLim;
			//update( );
		}

		//inline void setFp(const vec2i32& vFp){ fp_temp = vFp; update(); }
		//inline void setSp(const vec2i32& vSp){ sp_temp = vSp; update(); }



	private:
		vec2f32* fp;		//First point of the line
		vec2f32* sp;		//Second point of the line
		vec2f32 _fp;		//First point of the line
		vec2f32 _sp;		//Second point of the line
		//vec2i32 fp_temp;		//TODO remove. Temporary workaround for the absence of a shader matrix transform
		//vec2i32 sp_temp;		//TODO remove. Temporary workaround for the absence of a shader matrix transform
	public:

		vec4f32* fc;		//Color of the first point
		vec4f32* sc;		//Color of the second point
		float32* fw;		//Width of the first point
		float32* sw;		//Width of the second point
	};
}

//TODO dual camera