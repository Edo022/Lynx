#pragma once

#include "LuxEngine/Types/LuxObject/LuxObject.h"




namespace lux::obj {
	inline void spawnObject(Base* pObject);



	//A 2D space where to render lux objects
	//The position of the object is relative to the size and position of the render space
	//    It goes from 0, 0 to +1, +1
	//    Out of view objects will be cropped if the renderSpaces does not allowOutOfViewRender
	//This render space can only be placed in other 2D render spaces or 
	struct RenderSpace2D : public Base2D {
		static const ObjectType objectType = LUX_OBJECT_TYPE_RENDER_SPACE_2D;
		lux::Map<const Base*, uint32> objects;
		bool allowOutOfViewRender = false;


		//TODO allocate on different thread
		//TODO allocation is alwais waited
		//This function spawn an object in the render space
		//    A 2D render space only allows 2D objects. Trying to spawn a non 2D object will result in an error
		//*   pObject: a pointer to the object to spawn. It must be a sub struct of lux::obj::Base2D
		//*   vWaitAllocation: whether to wait or not the allocation of the object
		//*       Members of non allocated objects cannot be read or written
		//*       Trying to access a non allocated member will cause a runtime exception
		//*       You can check if an object has been allocated through is "allocated" bool variable
		//*   Returns true if the object is spawned correctly, false if the object is invalid
		bool spawnObject(Base* pObject, const bool vWaitAllocation = true);
		

		void initPtrs() final override {}
		inline int32 getCellSize() final override { return 0; }


	};
}
//TODO add 3d render space
//TODO add 2d render space for 3d space
//TODO add 3d render space for 3d space