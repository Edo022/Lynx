#include "Lynx/Types/Object/2D/RenderSpace3.hpp"
#include "Lynx/Core/Core.hpp"
#include "Lynx/Core/Render/Window/Window.hpp"




namespace lnx::obj {
    //TODO fix documentation
	//Adds an object to the render space children
	//Automatically updates the  parent and child index of the object
	//Returns the child index
	uint32 RenderSpace3::addChild(obj3_b* pObj) { //TODO RENAME AS ADD or move to children object
		pObj->parent = this;
		//setChildLimits(pObj->common.childIndex = children.add(pObj));
		// pObj->qHierarchy();
		pObj->childIndex = children.add(pObj); //FIXME USE QUEUES. ADD CHILDREN BEFORE ANYTHING ELSE
		pObj->queue(eLimit | eFlush);
		if(w) w->qSpawn(pObj);
		return pObj->childIndex;
	}
}