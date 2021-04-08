#include "Lynx/Types/Object/2D/RenderSpace2.hpp"
#include "Lynx/Core/Core.hpp"



namespace lnx::obj {
	//Adds an object to the render space children
	//Automatically updates the  parent and child index of the object
	//Returns the child index
	bool RenderSpace2::addChild(Obj2_b* pObj) { //TODO RENAME AS ADD
		pObj->parent = this;
		setChildLimits(pObj->common.childIndex = children.add(pObj));
		pObj->qHierarchy();
		if(render.parentWindow) pObj->onSpawn(*render.parentWindow);
		return pObj->common.childIndex;
	}




	//Updates the render limit of the child at a specific index
	//It depends on the render space properties and children alignment
	//Returns false if the index is invalid
	bool RenderSpace2::setChildLimits(const uint32 vChildIndex) const {
		if(vChildIndex >= children.count()) return false;
		switch(alignment) {
			case AlignmentType::FixedHorizontal:
			{
				auto xElmLen = abs(minLim.x - maxLim.x) / xNum;
				children[vChildIndex]->setMinLim({ minLim.x + (xElmLen * vChildIndex), minLim.y });
				children[vChildIndex]->setMaxLim({ minLim.x + (xElmLen * vChildIndex) + xElmLen, maxLim.y });
				break;
			}
			case AlignmentType::FixedVertical:
			{
				auto yElmLen = abs(minLim.y - maxLim.y) / xNum;
				children[vChildIndex]->setMinLim({ minLim.x, minLim.y + (yElmLen * vChildIndex) });
				children[vChildIndex]->setMaxLim({ maxLim.x, minLim.y + (yElmLen * vChildIndex) + yElmLen });
				break;
			}
			//case AlignmentType::Vertical:
			//case AlignmentType::Horizontal:
			case AlignmentType::Free:
			{
				children[vChildIndex]->setMinLim(minLim);
				children[vChildIndex]->setMaxLim(maxLim);
				break;
			}
			default: dbg::printError("Unknown children alignment type");
		}
		return true;
	}
}