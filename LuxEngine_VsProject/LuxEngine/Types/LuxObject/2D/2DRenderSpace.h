#pragma once

#include "LuxEngine/Types/LuxObject/LuxObject.h"




namespace lux::obj {
	//A 2D space that allows objects to be rendered
	//The position of the object is relative to the size and position of the render space
	//    It goes from 0, 0 (beginning of the render space) to +1, +1 (end of the render space)
	//    Out of view objects will be cropped if the renderSpaces does not allowOutOfViewRender
	//    Use setChildLimits() to get the region of screen assigned to a child of the render space
	//This render space can only be a child of other 2D render spaces
	//Render spaces with no parent will be rendered directly in the window
	struct RenderSpace2D : public Base2D {
		static const ObjectType objectType = LUX_OBJECT_TYPE_RENDER_SPACE_2D;
		bool allowOutOfViewRender = false;


		//Adds an object to the render space children
		//Automatically updates the  parent and child index of the object
		//Returns the child index
		bool addChild(Base2D* pObject){
			pObject->parent = this;
			setChildLimits(pObject->childIndex = children.add(pObject));
			pObject->update( );
			return pObject->childIndex;
		}

		//Updates the render limit of the child with a specific index
		//It depends on the render space properties and children alignment
		void setChildLimits(const uint32 vChildIndex) const final override {
			children[vChildIndex]->minLim = minLim;
			children[vChildIndex]->maxLim = maxLim;
		}


		void update( ) final override { }
		inline int32 getCellSize( ) const final override { return 0; }
	};
}
//TODO add 3d render space
//TODO add 2d render space for 3d space
//TODO add 3d render space for 3d space