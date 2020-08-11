#pragma once
#include "LuxEngine/Core/Compute/CShader.h"
#include "LuxEngine/Types/LuxObject/LuxObject.h"



namespace lux::obj{
	struct Border2D : public Base2D{
	private:
		vec2f32* fp = { nullptr };
		vec2f32* sp = { nullptr };
	public:
		vec2f32 fp_tmp;
		vec2f32 sp_tmp;


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
			luxInitObject(2, BORDER);
			fp = (vec2f32*)((int8*)render.data + 0);					//    8    |    0  - 7     |    0 +
			sp = (vec2f32*)((int8*)render.data + 8);					//    8    |    8  - 15    |    0
			*(uint32*)((int8*)render.data + 16) = (uint32)common.ID;	//    4    |    16 - 19    |    1 +
			//12x trailing padding										//    12   |    20 - 31

			//TODO just update the shaders when needed with the right workgroup count
			//TODO remove useless out-of-range check in shaders
			//core::c::shaders::newShader({ core::g::wnd::gpuCellWindowOutput, core::g::wnd::gpuCellWindowSize, core::g::wnd::gpuCellWindowZBuffer, render.data }, render.shaderLayout, 1980 * 2, 1080, 1);
			core::c::shaders::newShader({ core::g::wnd::gpuCellWindowOutput, core::g::wnd::gpuCellWindowSize, core::g::wnd::gpuCellWindowZBuffer, render.localData }, render.shaderLayout, 1980 * 2, 1080, 1);
		}


		//TODO convert to variable. or convert the variable to function. idk
		inline int32 getCellSize( ) const final override { return 20; }


		void update( ) final override {
			vec2f32 f = { 1920 * 2, 1080 };

			//TODO automatize default object properties and functions
			*fp = fp_tmp * f;		//    8    |    0  - 7     |    0 +
			*sp = sp_tmp * f;		//    8    |    8  - 15    |    0
			//*(uint32*)((int8*)render.dataPtr + 16) = (uint32)common.ID;	//    4    |    16 - 19    |    1 +
			//12x trailing padding							//    12   |    20 - 31
			updateBase();
			//TODO recalculate limits for children
		}
	};
}