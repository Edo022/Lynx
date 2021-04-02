#pragma once
#define LUX_H_LUX_OBJECT

#include "LuxEngine/Types/Vectors/Vectors.hpp"
#include "LuxEngine/Types/Containers/LuxString.hpp"
#include "LuxEngine/Core/Render/Shaders/Shader.hpp"
#include "LuxEngine/Types/VPointer.hpp"








namespace lux{
	//This namespace contains all the structures and functions of render objects
	namespace obj {
		struct Border2D;


		enum class limitAlignment : int16{
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
			Base() { common.objectType = LUX_OBJECT_TYPE__BASE; }	//														|							|
			struct Common{
				ObjectType objectType;							//Thte type of the object									| object type				| object type
				static uint64 lastID;							//#LLID LOS000 the last assigned ID of a Lux object			| none						| none
				uint64 ID{ ++lastID };							//A unique ID that indentifies the object					| none						| object instance
				uint32 childIndex{ (uint32)-1 };				//The index of the object in the parent's children list		| none						| object instance
			} common;
			_dbg(bool debug = false;)						//Defines if the object is used for graphical debugging		| none						| object instance
			virtual bool setChildLimits(const uint32 vChildIndex) const = 0;
			virtual ram::ptr<char>       getShData(){ return nullptr; }
			virtual vram::Alloc_b<char> getShVData(){ return vram::Alloc_b<char>(); }

			struct Render{
				ShaderLayout shaderLayout;						//Thte shader layout of the object's render shader			| object type				| object type
				bool updated{ true };
				Window* parentWindow = nullptr;
			} render;
			virtual void update() = 0;							//Updates the object data in the shared memory				| object type				| -
			void allocate();									//Allocates a memory cell for the object data				| object type				| -
			void updateBase();
			virtual void recalculateCoords() {}
			virtual void init(Window& pWindow){}
		};




		//TODO automatic enum creation
		#define luxInitObject(dimensions_, objectType_)								\
			common.objectType = LUX_OBJECT_TYPE_##dimensions_##D_##objectType_;		\
			render.shaderLayout = LUX_DEF_SHADER_##dimensions_##D_##objectType_;
	}
}