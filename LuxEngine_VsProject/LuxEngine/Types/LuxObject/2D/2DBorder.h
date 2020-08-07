#pragma once
#include "LuxEngine/Core/Compute/CShader.h"
#include "LuxEngine/Types/LuxObject/LuxObject.h"



namespace lux::obj{
	struct Border2D : public Base2D{
		vec2f32 fp = { .0f, .0f };
		vec2f32 sp = { .0f, .0f };


		Border2D( ){ init( ); }
		Border2D(const vec2f32& vFp, const vec2f32& vSp) :
			fp{ vFp }, sp{ vSp }{
			init( );
		}
		void init( ){
			objectType = LUX_OBJECT_TYPE_2D_BORDER;
			shaderLayout = LUX_DEF_SHADER_BORDER_2D;
			this->allocate( );

			core::c::shaders::cshaderNew({ core::g::wnd::gpuCellWindowOutput, core::g::wnd::gpuCellWindowSize, core::g::wnd::gpuCellWindowZBuffer, gpuCell }, shaderLayout, 1, 1, 1);
			//core::c::shaders::cshaderNew({ core::g::wnd::gpuCellWindowOutput, core::g::wnd::gpuCellWindowSize, core::g::wnd::gpuCellWindowZBuffer, gpuCell }, shaderLayout, uint32(abs(fp.x - sp.x) + abs(fp.y - sp.y)), 1, 1);
		}


		//TODO convert to variable. or convert the variable to function. idk
		inline int32 getCellSize( ) const final override { return 20; }


		void update( ) final override {
			*(vec2i32*)((int8*)cellPtr + 0) = fp;			//    8    |    0  - 7     |    0 +
			*(vec2i32*)((int8*)cellPtr + 8) = sp;			//    8    |    8  - 15    |    0
			*(uint32*)((int8*)cellPtr + 16) = (uint32)ID;	//    4    |    16 - 19    |    1 +
			//12x trailing padding							//    12   |    20 - 31

			//TODO recalculate limits for children
		}
	};
}