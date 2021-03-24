#include "LuxEngine/Core/Core.hpp"
#include "LuxEngine/Core/Render/Shaders/Shader.hpp"
#include "LuxEngine/Types/LuxObject/LuxObject.hpp"
#include "LuxEngine/Types/LuxObject/2D/2DBorder.hpp"






namespace lux::obj{
	uint64 Base::Common::lastID = 0;	//#LLID LOS000 initialize the last object ID


	//Adds a render space with no parent to the screen //FIXME MOVED TO WINDOW.G
	// void addRenderSpace(RenderSpace2D* pRenderSpace) {
	// 	core::c::shaders::CRenderSpaces.add(pRenderSpace);
	// }


	//This function allocates the object data in the shared memory of the GPU, eventually initializing the engine
	//An object needs to be allocated before being read or written
	void Base::allocate() {
		if(common.objectType >= 3000 || common.objectType < 2000) return;
		//TODO add initialization for 1d, 2.5d and 3d non base objects
		render.data = (int8*)malloc(cellSize);										//Create the cell taht contains the object data
		render.localData.realloc(cellSize);	//Create the cell taht contains the object data
	}


	//Update function common to all the objects that adds them to the update queue
	//Only render threads can directly access object memory and command buffers
	void Base::updateBase() {
		if(common.objectType == ObjectType::LUX_OBJECT_TYPE_2D_RENDER_SPACE || common.objectType == ObjectType::LUX_OBJECT_TYPE_3D_RENDER_SPACE) return;
		if(!render.parentWindow) return;
		render.parentWindow->objUpdates2D_f.lock();
		if(render.updated) {
			render.updated = false;
			// core::render::objUpdates2D.add(this);
			render.parentWindow->objUpdates2D.add(this);
		}
		render.parentWindow->objUpdates2D_f.unlock();
	}
}