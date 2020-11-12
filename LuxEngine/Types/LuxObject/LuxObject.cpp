
#include "LuxEngine/Core/Core.h"
#include "LuxEngine/Core/Compute/CShader.h"
#include "LuxEngine/Types/LuxObject/LuxObject.h"
#include "LuxEngine/Types/LuxObject/2D/2DBorder.h"
#include "LuxEngine/Memory/Ram/Memory.h"






namespace lux::obj{
	uint64 Base::Common::lastID = 0;	//#LLID LOS000 initialize the last object ID


	//Adds a render space with no parent to the screen
	void addRenderSpace(RenderSpace2D* pRenderSpace){
		core::c::shaders::CRenderSpaces.add(pRenderSpace);
	}


	//This function allocates the object data in the shared memory of the GPU, eventually initializing the engine
	//An object needs to be allocated before being read or written
	void Base::allocate( ){
		if(common.objectType >= 3000 || common.objectType < 2000) return;
		//TODO add initialization for 1d, 2.5d and 3d non base objects
		//core::init(false);																	//Initialize the engine
		// render.data = (int8*)malloc(getCellSize( ));										//Create the cell taht contains the object data
		render.data = (int8*)malloc(cellSize);										//Create the cell taht contains the object data
		// render.localData = rem::allocBck(getCellSize( ), CellClass::AUTO, AllocType::SHARED_UNIFORM);	//Create the cell taht contains the object data
		render.localData = rem::allocBck(cellSize, CellClass::AUTO, AllocType::SHARED_UNIFORM);	//Create the cell taht contains the object data
	}


	//Update function common to all the objects that adds them to the update queue
	//Only render threads can directly access object memory and command buffers
	void Base::updateBase( ){
		if(common.objectType == ObjectType::LUX_OBJECT_TYPE_2D_RENDER_SPACE || common.objectType == ObjectType::LUX_OBJECT_TYPE_3D_RENDER_SPACE) return;
		core::g::pendingObjectUpdatesFence.startSecond( );
		if(render.updated){
			render.updated = false;
			core::g::objUpdates2D.add(this);
			//TODO remove debug junk
			if(core::g::objUpdates2D[0]->common.objectType == ObjectType::LUX_OBJECT_TYPE_2D_LINE){
				sleep(500);
				int hfrewfvgrefre = 0;
			}
		}
		// sleep(500);
		core::g::pendingObjectUpdatesFence.endSecond( );
		// sleep(500);
		// sleep(500);
		// sleep(500);
		// sleep(500);
		// sleep(500);
		// sleep(500);
	}




	//In debug mode, setMinLim and setMaxLim functions of non debug objects and update the debug border
	#ifdef LUX_DEBUG
	//TODO set debug border only in debug mode
	void Base2D::setMinLim(vec2f32 vMinLim){
		minLim = vMinLim;
		if(!debug){
			if(!debugBorder) {
				debugBorder = new Border2D( );
				debugBorder->debug = true;
			}
			*debugBorder->fp = vMinLim;
			debugBorder->update( );
		}
	}
	void Base2D::setMaxLim(vec2f32 vMaxLim){
		maxLim = vMaxLim;
		if(!debug){
			if(!debugBorder) {
				debugBorder = new Border2D( );
				debugBorder->debug = true;
			}
			*debugBorder->sp = vMaxLim;
			debugBorder->update( );
		}
	}
	#endif
}