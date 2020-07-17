
#include "LuxObject.h"
#include "LuxEngine/Engine/Engine.h"






//Adds a render space with no parent to the engine
namespace lux::obj{
	void addRenderSpace(RenderSpace2D* pRenderSpace){
		lux::engine( ).CRenderSpaces.add(pRenderSpace);
	}




	//This function allocates the object data in the shared memory of the GPU, eventually initializing the engine
	//An object needs to be allocated before being read or written
	void Base::allocate( ){
		if(objectType >= 3000 || objectType < 2000) return;
		//TODO add initialization for 1d, 2.3d and 3d non base objects
		lux::engine( ).init(false);											//Initialize the engine
		gpuCell = lux::engine( ).gpuCellCreate(getCellSize( ), true);		//Create the cell taht contains the object data
		cellPtr = lux::engine( ).gpuCellMap(gpuCell);						//Map the cell pointer to the cell
		initPtrs( );														//Initialize the pointers to the data
		allocated = true;													//Set allocation state to allocated
	}
}