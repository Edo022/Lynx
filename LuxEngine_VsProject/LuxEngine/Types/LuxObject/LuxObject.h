#pragma once
#ifndef __LUX_OBJ
#define __LUX_OBJ

#include "LuxEngine/Types/Vectors/Vectors.h"
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Types/EngineTypes.h"
#include "LuxEngine/Types/Containers/LuxString.h"
#include "LuxEngine/Types/Containers/LuxMap.h"
//#include "LuxEngine/Engine/Engine.h"
//
//
//class Engine;
//extern Engine engine;


namespace lux{
	namespace obj {
		enum class limitAlignment : int8{
			Top,
			Bottom,
			Right,
			Left,
			Center
		};


		//The type of the object (simulates reflection)
		//    Negative values    = base struct
		//    +-1000 to +-1999   = 1D object
		//    +-2000 to +-2999   = 2D object
		//    +-3000 to +-3999   = 3D object
		//    +-4000 to +-4999   = 2DI3D object
		enum ObjectType : int32 {
			LUX_OBJECT_TYPE__BASE = -1,

			LUX_OBJECT_TYPE_3D__BASE = -3000,
			LUX_OBJECT_TYPE_2i3D__BASE = -4000,
			LUX_OBJECT_TYPE_2D__BASE = -2000,
			LUX_OBJECT_TYPE_1D__BASE = -1000,

			LUX_OBJECT_TYPE_RENDER_SPACE_2D = +2000,
			LUX_OBJECT_TYPE_RENDER_SPACE_3D = +3000,

			LUX_OBJECT_TYPE_2D_LINE = +2001,
		};





		//Base class for render objects
		struct Base {
			//												Description													Structure differences		Value differences
			//												----------------------------------------------------------------------------------------------------------
			ObjectType objectType;							//Thte type of the object									| object type				| object type
			Base( ) : objectType(LUX_OBJECT_TYPE__BASE) { }		//														|							|
																//														|							|
			lux::String name{ "" };							//The name of the object.									| none						| object instance
			static uint64 lastID;							//#LLID LOS000 the last assigned ID of a Lux object			| none						| none
			uint64 ID{ ++lastID };							//A unique ID that indentifies the object					| none						| object instance
			uint32 childIndex{ (uint32)-1 };				//The index of the object in the parent's children list		| none						| object instance
																//														|							|
			void allocate( );								//Allocates a memory cell for the object data				| object type				| -
			bool allocated = false;							//Whether the object is allocated or not					| none						| object instance
			LuxCell gpuCell{ (uint64)-1 };					//GPU memory containing the small data of the object		| object type				| object instance
			void* cellPtr{ nullptr };						//Pointer to the GPU memory cell							| none						| object instance
			lux::String shaderName{ "" };					//The name of the shader that renders the object			| none						| object type
			inline virtual int32 getCellSize( ) const = 0;	//Size of the object data									| none						| object type
			virtual void update( ) = 0;						//Updates the object data in the shared memory				| object type				| -
																//														|							|
			//Sets the render limits of a child object																	| object type				| -
			virtual bool setChildLimits(const uint32 vChildIndex) const = 0;
		};




		//3D object in 3D space
		struct Base3D : public Base {
			Base3D( ) { objectType = LUX_OBJECT_TYPE_3D__BASE; }

			vec3f32 pos{ 0, 0, 0 };			//Position of the object. The position is relative to the origin of the object
			vec3f32 org{ 0, 0, 0 };			//Origin of the object
			float32 wIndex{ 0 };			//Index of the object. Objects with higher wIndex will be rendered on top of others
			vec3f32 rot{ 0, 0, 0 };			//Rotation of the object
			vec3f32 scl{ 0, 0, 0 };			//Scale of the object

			////TODO add absolute pixel position and scale
			//Base3D* parent{ nullptr };						//Parent of the object
			//lux::Map<Base3D*, uint32> children;				//Children of the object
			//void setChildLimits(const uint32 vChildIndex) const override {
			//	children[vChildIndex]->minLim = minLim;
			//	children[vChildIndex]->maxLim = maxLim;
			//}
			//vec3f32 minLim{ 0, 0, 0 };						//The limit of the object render. It depends on the parent of the object and its properties
			//vec3f32 maxLim{ 1, 1, 1 };						//The limit of the object render. It depends on the parent of the object and its properties
		};




		//Base class for 2D objects with 3D properties (they can be used in both 2D and 3D spaces)
		struct Base2DI3D : public Base {
			//TODO
			Base2DI3D( ) { objectType = LUX_OBJECT_TYPE_2i3D__BASE; }

			vec3f32 pos{ 0, 0, 0 };			//Position of the object. The position is relative to the origin of the object
			vec3f32 org{ 0, 0, 0 };			//Origin of the object
			float32 wIndex{ 0 };			//Index of the object for 3D space
			float32 zIndex{ 0 };			//Index of the object for 2D space
			vec3f32 rot{ 0, 0, 0 };			//Rotation of the object
			vec2f32 scl{ 0, 0 };			//Scale of the object
		};




		//Base class for 2D objects in 2D space
		struct Base2D : public Base {
			Base2D( ) { objectType = LUX_OBJECT_TYPE_2D__BASE; }

			//TODO add absolute pixel position and scale
			vec2f32 pos{ 0, 0 };			//Position of the object. The position is relative to the origin of the object
			vec2f32 org{ 0, 0 };			//Origin of the object
			float32 zIndex{ 0 };			//Index of the object. Objects with higher zIndex will be rendered on top of others
			float32 rot{ 0 };				//Rotation of the object
			vec2f32 scl{ 0, 0 };			//Scale of the object

			//TODO add absolute pixel position and scale
			Base2D* parent{ nullptr };						//Parent of the object
			lux::Map<Base2D*, uint32> children;				//Children of the object
			virtual bool setChildLimits(const uint32 vChildIndex) const override;
			vec2f32 minLim{ 0, 0 };										//The limit of the object render. It depends on the parent of the object and its properties
			vec2f32 maxLim{ 1, 1 };										//The limit of the object render. It depends on the parent of the object and its properties
			limitAlignment limitAlignment{ limitAlignment::Center }; 	//The alignment of the object within its limits
		};




		//Base class for 1D objects in 1D space
		struct Base1D : public Base {
			//TODO
			Base1D( ) { objectType = LUX_OBJECT_TYPE_1D__BASE; }

			float32 pos{ 0 };				//Position of the object. The position is relative to the origin of the object
			float32 org{ 0 };				//Origin of the object
			float32 yIndex{ 0 };			//Index of the object. Objects with higher yIndex will be rendered on top of others
			float32 scl{ 0 };				//Scale of the object

			////TODO add absolute pixel position and scale
			//Base2D* parent{ nullptr };						//Parent of the object
			//lux::Map<Base2D*, uint32> children;				//Children of the object
			//void setChildLimits(const uint32 vChildIndex) final override {
			//	children[vChildIndex]->minLim = minLim;
			//	children[vChildIndex]->maxLim = maxLim;
			//}
			//vec2f32 minLim{ 0, 0 };							//The limit of the object render. It depends on the parent of the object and its properties
			//vec2f32 maxLim{ 0, 0 };							//The limit of the object render. It depends on the parent of the object and its properties
		};



		struct RenderSpace2D;
		void addRenderSpace(RenderSpace2D* pRenderSpace);
	}
}
#endif // !__LUX_OBJ