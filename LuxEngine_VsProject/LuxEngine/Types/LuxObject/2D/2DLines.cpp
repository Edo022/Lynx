
#include "LuxEngine/Core/Core.h"
#include "LuxEngine/Core/Compute/CShader.h"
#include "LuxEngine/Types/LuxObject/2D/2DLines.h"



namespace lux::obj{
	void Line2D::init( ){
		luxInitObject(2, LINE);

		//TODO just update the shaders when needed with the right workgroup count
		//TODO remove useless out-of-range check in shaders
		core::c::shaders::cshaderNew({ core::g::wnd::gpuCellWindowOutput, core::g::wnd::gpuCellWindowSize, core::g::wnd::gpuCellWindowZBuffer, gpuCell }, shaderLayout, 4, 1, 1);
	}
}