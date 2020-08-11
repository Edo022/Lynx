#pragma once
#ifndef __LUX_OBJ
#define __LUX_OBJ

#include "LuxEngine/Types/Vectors/Vectors.h"
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Types/EngineTypes.h"
#include "LuxEngine/Types/Containers/LuxString.h"
#include "LuxEngine/Types/Containers/LuxMap.h"
#include "LuxEngine/Core/Compute/CShader_t.h"
#include "LuxEngine/macros.h"





//TODO reuse objects dont destroy them
//TODO restore() function to restore an object and reuse it instead of destroying it
//TODO restore function tipo in destroy function description
namespace lux{
	namespace obj {
		struct Border2D;


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
			LUX_OBJECT_TYPE__BASE           = -1,

			LUX_OBJECT_TYPE_3D__BASE        = -3000,
			LUX_OBJECT_TYPE_2i3D__BASE      = -4000,
			LUX_OBJECT_TYPE_2D__BASE        = -2000,
			LUX_OBJECT_TYPE_1D__BASE        = -1000,

			LUX_OBJECT_TYPE_2D_RENDER_SPACE = +2000,
			LUX_OBJECT_TYPE_3D_RENDER_SPACE = +3000,

			LUX_OBJECT_TYPE_2D_LINE         = +2001,
			LUX_OBJECT_TYPE_2D_BORDER       = +2002,
		};
		#define BASE
		#define RENDER_SPACE
		#define RENDER_SPACE
		#define LINE
		#define BORDER



		//Base class for render objects							Description													Structure differences		Value differences
		struct Base { //			   							----------------------------------------------------------------------------------------------------------
			Base( ) { common.objectType = LUX_OBJECT_TYPE__BASE; }	//														|							|
			struct Common{
				ObjectType objectType;							//Thte type of the object									| object type				| object type
				lux::String name{ "" };							//The name of the object.									| none						| object instance
				static uint64 lastID;							//#LLID LOS000 the last assigned ID of a Lux object			| none						| none
				uint64 ID{ ++lastID };							//A unique ID that indentifies the object					| none						| object instance
				uint32 childIndex{ (uint32)-1 };				//The index of the object in the parent's children list		| none						| object instance
			} common;
			luxDebug(bool debug = false;)						//Defines if the object is used for graphical debugging		| none						| object instance
			virtual bool setChildLimits(const uint32 vChildIndex) const = 0;


			struct Render{
				ShaderLayout shaderLayout;						//Thte shader layout of the object's render shader			| object type				| object type
				void* data{ nullptr };							//Object tdata stored in RAM								| none						| object instance
				LuxCell localData{ (uint64)-1 };				//Local GPU copy of data									| object type				| object instance
				bool updated{ true };
				LuxCell cache{ (uint64)-1 };					//Object cache that avoids draws when not needed			| object type				| object instance
			} render;
			inline virtual int32 getCellSize( ) const = 0;		//Size of the object data									| none						| object type
			virtual void update( ) = 0;							//Updates the object data in the shared memory				| object type				| -
			void allocate( );									//Allocates a memory cell for the object data				| object type				| -
			void updateBase( );
			virtual void recalculateCoords( ){}
		};



		//TODO automatic enum creation
		#define luxInitObject(dimensions_, objectType_)								\
			common.objectType = LUX_OBJECT_TYPE_##dimensions_##D_##objectType_;		\
			render.shaderLayout = LUX_DEF_SHADER_##dimensions_##D_##objectType_;	\
			this->allocate( );








		//3D object in 3D space
		struct Base3D : public Base {
			Base3D( ) { common.objectType = LUX_OBJECT_TYPE_3D__BASE; }

			vec3f32 pos{ 0, 0, 0 };			//Position of the object. The position is relative to the origin of the object
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
			Base2DI3D( ) { common.objectType = LUX_OBJECT_TYPE_2i3D__BASE; }

			vec3f32 pos{ 0, 0, 0 };			//Position of the object. The position is relative to the origin of the object
			float32 wIndex{ 0 };			//Index of the object for 3D space
			float32 zIndex{ 0 };			//Index of the object for 2D space
			vec3f32 rot{ 0, 0, 0 };			//Rotation of the object
			vec2f32 scl{ 0, 0 };			//Scale of the object
		};








		//Base class for 2D objects in 2D space
		struct Base2D : public Base {
			Base2D( );

			//TODO add absolute pixel position and scale
			vec2f32 pos{ 0, 0 };			//Position of the object. The position is relative to the origin of the object
			float32 zIndex{ 0 };			//Index of the object. Objects with higher zIndex will be rendered on top of others
			float32 rot{ 0 };				//Rotation of the object
			vec2f32 scl{ 0, 0 };			//Scale of the object

			//TODO add absolute pixel position and scale
			Base2D* parent{ nullptr };						//Parent of the object
			lux::Map<Base2D*, uint32> children;				//Children of the object
			virtual bool setChildLimits(const uint32 vChildIndex) const override {
				if(vChildIndex >= children.size( )) return false;
				children[vChildIndex]->setMinLim(minLim);
				children[vChildIndex]->setMaxLim(maxLim);
				return true;
				//TODO update per-object data
			//TODO add  recalculateCoords( ) to all objects
			}
			void update( ) final override{
				updateBase( );
				for(int i = 0; i < children.size( ); i++) if(children.isValid(i)) {
					setChildLimits(i);
					children[i]->recalculateCoords( );
					children[i]->update( );
				}
			}
			limitAlignment limitAlignment{ limitAlignment::Center }; 	//The alignment of the object within its limits
			vec2f32 minLim{ 0, 0 };										//The limit of the object render. It depends on the parent of the object and its properties
			vec2f32 maxLim{ 1, 1 };										//The limit of the object render. It depends on the parent of the object and its properties
			luxRelease(inline) void __vectorcall setMinLim(vec2f32 vMinLim)luxRelease({ minLim = vMinLim; });
			luxRelease(inline) void __vectorcall setMaxLim(vec2f32 vMaxLim)luxRelease({ maxLim = vMaxLim; });
			luxDebug(Border2D* debugBorder = nullptr;)					//Debug. Used to draw the object limits
		};








		//Base class for 1D objects in 1D space
		struct Base1D : public Base {
			//TODO
			Base1D( ) { common.objectType = LUX_OBJECT_TYPE_1D__BASE; }

			float32 pos{ 0 };				//Position of the object. The position is relative to the origin of the object
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