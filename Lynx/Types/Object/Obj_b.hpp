#pragma once
#define LNX_H_LNX_OBJECT

#include "Lynx/Types/Vectors/Vectors.hpp"
#include "Lynx/Types/Containers/String.hpp"
#include "Lynx/Core/Render/Shaders/Shader.hpp"
#include "Lynx/Types/VPointer.hpp"








namespace lnx{
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




		/**
		 * @brief Base class of Obj_bt
		 */
		struct Obj_b { //
			struct Common{
				static uint64 lastID;							//#LLID LOS000 the last assigned ID of a Lynx object
				uint64 ID{ ++lastID };							//A unique ID that indentifies the object
				uint32 childIndex{ (uint32)-1 };				//The index of the object in the parent's children list
			} common;
			_dbg(bool debug = false;)						//Defines if the object is used for graphical debugging
			virtual void setChildLimits(const uint32 vChildIndex) const = 0;
			virtual ram::ptr<char>       getShData(){ return nullptr; }
			virtual vram::Alloc_b<char> getShVData(){ return vram::Alloc_b<char>(); }

			struct Render{
				bool updated{ true };
				Window* parentWindow = nullptr;
			} render;
			virtual void qSelf();	//Queues the object to make the render thread update it between the current and the next frame draw
			virtual void recalculateCoords() {}
			virtual void onSpawn(Window& pWindow){}
		};




		/**
		 * @brief Base class of any render object
		 * @tparam chType Type of the children objects. Can be any subclass of Obj_bt
		 */
		template<class chType> struct Obj_bt : virtual public Obj_b {
			lnx::RaArray<chType*, uint32> children;
		};
	}
}