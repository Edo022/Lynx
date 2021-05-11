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



		enum UpdateBits : uint32 {
			none    = 0,
			spawn   = 0b0001,
			limit   = 0b0100,
			updateg = 0b0010
		};
		alwaysInline UpdateBits operator~ (UpdateBits  a){ return (UpdateBits)((u32)~ (u32)a); }
		alwaysInline UpdateBits operator| (UpdateBits  a, UpdateBits b){ return (UpdateBits )((u32 )a |  (u32)b); }
		alwaysInline UpdateBits operator& (UpdateBits  a, UpdateBits b){ return (UpdateBits )((u32 )a &  (u32)b); }
		alwaysInline UpdateBits operator^ (UpdateBits  a, UpdateBits b){ return (UpdateBits )((u32 )a ^  (u32)b); }
		alwaysInline UpdateBits operator|=(UpdateBits& a, UpdateBits b){ return (UpdateBits&)((u32&)a |= (u32)b); }
		alwaysInline UpdateBits operator&=(UpdateBits& a, UpdateBits b){ return (UpdateBits&)((u32&)a &= (u32)b); }
		alwaysInline UpdateBits operator^=(UpdateBits& a, UpdateBits b){ return (UpdateBits&)((u32&)a ^= (u32)b); }


		/**
		 * @brief Members common to any Obj_bt instantiation
		 */
		struct Obj_bb { //
			struct Common{
				static uint64 lastID;							//#LLID LOS000 the last assigned ID of a Lynx object
				uint64 ID{ ++lastID };							//A unique ID that indentifies the object
				uint32 childIndex{ (uint32)-1 };				//The index of the object in the parent's children list
			} common;
			virtual void setChildLimits(const uint32 vChildIndex) const = 0;
			virtual ram::ptr<char>       getShData(){ return nullptr; }
			virtual vram::Alloc_b<char> getShVData(){ return vram::Alloc_b<char>(); }

			struct Render{									//Structure containing rendering helper members
				_dbg(bool isDbgObj = false;)					//True if the object is used for graphical debugging
				std::atomic<UpdateBits> updates;				//Update requests sent to the render thread
				Window* parentWindow = nullptr;					//Parent window object that contains the render thread and the window data
			} render;
			// virtual void qSelf();	//Queues the object to make the render thread update it between the current and the next frame draw
			// virtual void recalculateCoords() {}
			virtual void onSpawn(Window& pWindow){}
			virtual void onLimit(){}
			virtual void onUpdateg(){}

			//TODO comment
			void queue(UpdateBits vUpdates){
				UpdateBits old = render.updates;				//Save old updates bits
				render.updates = render.updates | vUpdates;		//Update updates bits
				if(render.parentWindow) { 						//If the object has a binded window //FIXME UPDATE ALL IN WINDOW SPAWN
					render.parentWindow->requests_m.lock();			//Lock requests mutex
					if(old) render.parentWindow->requests.add(this);//If it isn't already in it, add the object to the update queue
					render.parentWindow->requests_m.unlock();		//Unlock requests mutex
				}
			}
		};




		/**
		 * @brief Base class of any render object of any dimension
		 * @tparam chType Type of the children objects. Can be any subclass of Obj_b
		 */
		template<class chType> struct Obj_bt : virtual public Obj_bb {
			lnx::RaArray<chType*, uint32> children;
		};
	}
}