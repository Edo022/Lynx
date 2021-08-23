#include "Lynx/Types/Object/2D/RenderSpace2.hpp"
#include "Lynx/Core/Core.hpp"
#include "Lynx/Core/Render/Window/Window.hpp"



namespace lnx::obj {
	//TODO fix documentation
	//Adds an object to the render space children
	//Automatically updates the  parent and child index of the object
	//Returns the child index
	uint32 RenderSpace2::addChild(obj2_b* pObj) { //TODO RENAME AS ADD or move to children object
		pObj->parent = this;
		//setChildLimits(pObj->common.childIndex = children.add(pObj));
		// pObj->qHierarchy();
		pObj->childIndex = children.add(pObj); //FIXME USE QUEUES. ADD CHILDREN BEFORE ANYTHING ELSE
		pObj->queue(eLimit | eFlush);
		if(w) w->qSpawn(pObj);
		return pObj->childIndex;
	}




	//TODO fix documentation
	//Updates the render limit of the child at a specific index
	//It depends on the render space properties and children alignment
	//Returns false if the index is invalid
	void RenderSpace2::setChildLimits(const uint32 vChildIndex) const {
		dbg::checkParam(vChildIndex > children.count() - 1, "vChildIndex", "Invalid index");
		switch(alignment) {
			case AlignmentType::eFixedHorizontal:
			{ //FIXME ADD CHECK IN ADD FUNCTION TO CHECK THAT CHILDREN ARE OBJ2 ONLY
				auto xElmLen = abs(minLim.x - maxLim.x) / xNum;
				static_cast<obj2_b*>(children[vChildIndex])->setMinLim({ minLim.x + (xElmLen * vChildIndex), minLim.y });
				static_cast<obj2_b*>(children[vChildIndex])->setMaxLim({ minLim.x + (xElmLen * vChildIndex) + xElmLen, maxLim.y });
				break;
			}
			case AlignmentType::eFixedVertical:
			{
				auto yElmLen = abs(minLim.y - maxLim.y) / xNum;
				static_cast<obj2_b*>(children[vChildIndex])->setMinLim({ minLim.x, minLim.y + (yElmLen * vChildIndex) });
				static_cast<obj2_b*>(children[vChildIndex])->setMaxLim({ maxLim.x, minLim.y + (yElmLen * vChildIndex) + yElmLen });
				break;
			}
			//case AlignmentType::Vertical:
			//case AlignmentType::Horizontal:
			case AlignmentType::eFree:
			{
				static_cast<obj2_b*>(children[vChildIndex])->setMinLim(minLim);
				static_cast<obj2_b*>(children[vChildIndex])->setMaxLim(maxLim);
				break;
			}
			default: dbg::logError("Unknown children alignment type");
		}
	}
}