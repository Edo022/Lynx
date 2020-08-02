
#include "LuxEngine/Core/LuxCore.h"
#include "LuxEngine/Types/LuxObject/2D/2DLines.h"



namespace lux::obj{
	void Line2D::init( ){
		objectType = LUX_OBJECT_TYPE_2D_LINE;	//Set object type
		shaderLayout = LUX_DEF_SHADER_LINE_2D;	//Set shader layout
		this->allocate( );						//Allocate object data

		lux::core::c::cshaderNew({ lux::core::g::gpuCellWindowOutput, lux::core::g::gpuCellWindowSize, gpuCell }, shaderLayout, 4, 1, 1);
	}
}