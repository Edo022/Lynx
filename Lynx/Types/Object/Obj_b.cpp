#include "Lynx/Core/Core.hpp"
#include "Lynx/Core/Render/Shaders/Shader.hpp"
#include "Lynx/Types/Object/Obj_b.hpp"
#include "Lynx/Types/Object/2D/Border2.hpp"






namespace lnx::obj{
	uint64 Base::Common::lastID = 0;	//#LLID LOS000 initialize the last object ID //FIXME REMOVE or do something idk


	//Update function common to all the objects that adds them to the update queue
	//Only render threads can directly access object memory and command buffers<
	void Base::qSelf() {
		if(!render.parentWindow) return;
		render.parentWindow->objUpdates_m.lock();
		if(render.updated) {
			render.updated = false;
			render.parentWindow->objUpdates.add(this);
		}
		render.parentWindow->objUpdates_m.unlock();
	}
}