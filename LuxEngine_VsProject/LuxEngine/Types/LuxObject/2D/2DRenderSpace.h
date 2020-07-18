#pragma once

#include "LuxEngine/Types/LuxObject/LuxObject.h"




namespace lux::obj {
	//A 2D space that allows objects to be rendered
	//The position of the object is relative to the size and position of the render space
	//    It goes from 0, 0 to +1, +1
	//    Out of view objects will be cropped if the renderSpaces does not allowOutOfViewRender
	//This render space can only be a child of other 2D render spaces
	//Render spaces with no parent will be rendered directly in the window
	struct RenderSpace2D : public Base2D {
		static const ObjectType objectType = LUX_OBJECT_TYPE_RENDER_SPACE_2D;
		lux::Map<Base*, uint32> children;
		bool allowOutOfViewRender = false;


		//Adds an object to the render space children
		//Automatically updates the object's parent
		//Returns the index of the object in the children array
		bool addChild(Base* pObject){
			pObject->parent = this;
			return children.add(pObject);
		}


		void update() final override {}
		inline int32 getCellSize() const final override { return 0; }
	};
}
//TODO add 3d render space
//TODO add 2d render space for 3d space
//TODO add 3d render space for 3d space