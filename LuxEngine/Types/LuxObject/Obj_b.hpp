#pragma once
#define LUX_H_LUX_OBJECT

#include "LuxEngine/Types/Vectors/Vectors.hpp"
#include "LuxEngine/Types/Containers/LuxString.hpp"
#include "LuxEngine/Core/Render/Shaders/Shader.hpp"
#include "LuxEngine/Types/VPointer.hpp"








namespace lux{
	//This namespace contains all the structures and functions of render objects
	namespace obj {
		struct Border2;


		enum class limitAlignment : int16{
			Top,
			Bottom,
			Right,
			Left,
			Center
		};




		//Base class for render objects							Description													Structure differences		Value differences
		struct Base { //			   							----------------------------------------------------------------------------------------------------------
			struct Common{
				static uint64 lastID;							//#LLID LOS000 the last assigned ID of a Lux object			| none						| none
				uint64 ID{ ++lastID };							//A unique ID that indentifies the object					| none						| object instance
				uint32 childIndex{ (uint32)-1 };				//The index of the object in the parent's children list		| none						| object instance
			} common;
			_dbg(bool debug = false;)						//Defines if the object is used for graphical debugging		| none						| object instance
			virtual bool setChildLimits(const uint32 vChildIndex) const = 0;
			virtual ram::ptr<char>       getShData(){ return nullptr; }
			virtual vram::Alloc_b<char> getShVData(){ return vram::Alloc_b<char>(); }

			struct Render{
				bool updated{ true };
				Window* parentWindow = nullptr;
			} render;
			virtual void qSelf();							//Updates the object data in the shared memory				| object type				| -
			virtual void recalculateCoords() {}
			virtual void onSpawn(Window& pWindow){}
		};
	}
}