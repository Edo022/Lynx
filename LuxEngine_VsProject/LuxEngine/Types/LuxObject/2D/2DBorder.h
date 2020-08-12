#pragma once
#include "LuxEngine/Core/Compute/CShader.h"
#include "LuxEngine/Types/LuxObject/LuxObject.h"



namespace lux::obj{
	struct Border2D : public Base2D{
		//private:
		vec2f32* fp = { nullptr };
		vec2f32* sp = { nullptr };
		//public:
		//	vec2f32 fp_tmp;
		//	vec2f32 sp_tmp;


		Border2D( ){
			init( );
			*fp = { .0f, .0f };
			*sp = { .0f, .0f };
		}
		Border2D(const vec2f32& vFp, const vec2f32& vSp){
			//fp{ vFp }, sp{ vSp }{
			init( );
			*fp = vFp;
			*sp = vSp;
		}

		void init( ){
			//TODO calculate offset and cell size at runtime with an everything array
			//core::dvc::compute.PD.properties.limits.minUniformBufferOffsetAlignment;
			luxInitObject(2, BORDER);							// size    | range         | chunk
			fp = (vec2f32*)(render.data + 0);					//    8    |    0  - 7     |    0 +
			//8x padding										//    8    |    8  - 16    |    0
			sp = (vec2f32*)(render.data + 16);					//    8    |    17 - 24    |    1 +
			//8x padding										//    8    |    25 - 32    |    1
			*(uint32*)(render.data + 32) = (uint32)common.ID;	//    4    |    33 - 36    |    2 +
			//4x trailing padding								//    4    |    37 - 40

			//TODO just update the shaders when needed with the right workgroup count
			//TODO remove useless out-of-range check in shaders
			//core::c::shaders::newShader({ core::g::wnd::gpuCellWindowOutput, core::g::wnd::gpuCellWindowSize, core::g::wnd::gpuCellWindowZBuffer, render.data }, render.shaderLayout, 1980 * 2, 1080, 1);
			//core::c::shaders::newShader({ core::g::wnd::gpuCellWindowOutput, core::g::wnd::gpuCellWindowSize, core::g::wnd::gpuCellWindowZBuffer, render.localData }, render.shaderLayout, 1980 * 2, 1080, 1);
			core::c::shaders::newShader({ core::g::wnd::gpuCellWindowOutput, core::g::wnd::gpuCellWindowSize, core::g::wnd::gpuCellWindowZBuffer, render.localData }, render.shaderLayout, 1980 * 2, 1, 1);
			//TODO remove
			printf("\nCreated debug border");
			flushall();
		}


		//TODO convert to variable. or convert the variable to function. idk
		//TODO calculate offset and cell size at runtime with an everything array
		inline int32 getCellSize( ) const final override { return 40; }
		//inline int32 getCellSize( ) const final override { return 20; }


		//void update( ) final override {
		//	vec2f32 f = { 1920 * 2, 1080 };

		//	//TODO automatize default object properties and functions
		//	*fp = fp_tmp * f;		//    8    |    0  - 7     |    0 +
		//	*sp = sp_tmp * f;		//    8    |    8  - 15    |    0
		//	//*(uint32*)((int8*)render.dataPtr + 16) = (uint32)common.ID;	//    4    |    16 - 19    |    1 +
		//	//12x trailing padding							//    12   |    20 - 31
		//	updateBase();
		//	//TODO recalculate limits for children
		//}
	};
}