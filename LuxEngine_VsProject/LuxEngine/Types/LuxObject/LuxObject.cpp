
#include "LuxEngine/Core/Core.h"
#include "LuxEngine/Core/Compute/CShader.h"
#include "LuxEngine/Types/LuxObject/LuxObject.h"
#include "LuxEngine/Types/LuxObject/2D/2DBorder.h"






//Adds a render space with no parent to the engine
namespace lux::obj{
	uint64 Base::lastID = 0;	//#LLID LOS000 initialize the last object ID


	void addRenderSpace(RenderSpace2D* pRenderSpace){
		core::c::shaders::CRenderSpaces.add(pRenderSpace);
	}


	//This function allocates the object data in the shared memory of the GPU, eventually initializing the engine
	//An object needs to be allocated before being read or written
	void Base::allocate( ){
		if(objectType >= 3000 || objectType < 2000) return;
		//TODO add initialization for 1d, 2.5d and 3d non base objects
		core::init(false);											//Initialize the engine
		//TODO use GPU local memory
		gpuCell = core::c::buffers::gpuCellCreate(getCellSize( ), true);		//Create the cell taht contains the object data
		cellPtr = core::c::buffers::gpuCellMap(gpuCell);						//Map the cell pointer to the cell
	}




	#ifdef LUX_DEBUG
	void __vectorcall Base2D::setMinLim(vec2f32 vMinLim){
		minLim = vMinLim;
		if(!debug){
			if(debugBorder == nullptr) {
				debugBorder = new Border2D( );
				debugBorder->debug = true;
			}
			debugBorder->sp = vMinLim;
			debugBorder->update( );
		}
	}
	void __vectorcall Base2D::setMaxLim(vec2f32 vMaxLim){
		maxLim = vMaxLim;
		if(!debug){
			if(debugBorder == nullptr) {
				debugBorder = new Border2D( );
				debugBorder->debug = true;
			}
			debugBorder->sp = vMaxLim;
			debugBorder->update( );
		}
	}
	#endif




	Base2D::Base2D( ){
		objectType = LUX_OBJECT_TYPE_2D__BASE;
	}
}