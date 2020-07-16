
#include "Engine.h"
#include "LuxEngine/Types/LuxObject/2D/2DRenderSpace.h"




namespace lux::obj{
	//Deprecated function 
	//TODO remove. use specific spawn of the render space
	inline void spawnObject(Base* pObject) {
		Engine& engine_ = lux::engine( );
		if(pObject->objectType > 0) { engine_.objs.add(pObject); }
		else Exit("invalid object");
		engine_.cshaderNew(lux::Array<LuxCell>{ engine_.gpuCellWindowOutput, engine_.gpuCellWindowSize, engine_.objs[0]->gpuCell }, (engine_.shaderPath + pObject->shaderName + ".comp.spv").begin( ), 4, 1, 1);
	}
}


//Deprecated function 
//TODO remove. use specific spawn of the render space
bool lux::obj::RenderSpace2D::spawnObject(Base* pObject, const bool vWaitAllocation) {
	lux::obj::spawnObject(pObject);
	return true;
}


//This function allocates the object data in the shared memory of the GPU, eventually initializing the engine
//An object needs to be allocated before being read or written
void lux::obj::Base::allocate( ){
	if(objectType >= 3000 || objectType < 2000) return;							
	//TODO add initialization for 1d, 2.3d and 3d non base objects
	lux::engine( ).init(false);											//Initialize the engine
	gpuCell = lux::engine( ).gpuCellCreate(getCellSize( ), true);		//Create the cell taht contains the object data
	cellPtr = lux::engine( ).gpuCellMap(gpuCell);						//Map the cell pointer to the cell
	initPtrs( );														//Initialize the pointers to the data
	allocated = true;													//Set allocation state to allocated
}
