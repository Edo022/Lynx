
#include "LuxObject.h"
#include "LuxEngine/Core/Engine.h"






//Adds a render space with no parent to the engine
namespace lux::obj{
	void addRenderSpace(RenderSpace2D* pRenderSpace){
		lux::getEngine().CRenderSpaces.add(pRenderSpace);
	}




	//This function allocates the object data in the shared memory of the GPU, eventually initializing the engine
	//An object needs to be allocated before being read or written
	void Base::allocate( ){
		if(objectType >= 3000 || objectType < 2000) return;
		//TODO add initialization for 1d, 2.3d and 3d non base objects
		lux::getEngine( ).init(false);											//Initialize the engine
		//TODO use GPU local memory
		gpuCell = lux::core::c::gpuCellCreate(getCellSize( ), true);		//Create the cell taht contains the object data
		cellPtr = lux::core::c::gpuCellMap(gpuCell);						//Map the cell pointer to the cell
	}
}