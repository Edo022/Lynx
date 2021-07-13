#pragma once
#define LNX_H_LNX_OBJECT

#include "Lynx/Types/Vectors/Vectors.hpp"
#include "Lynx/Types/Containers/String.hpp"
#include "Lynx/Core/Render/Shaders/Shader.hpp"
#include "Lynx/Types/VPointer.hpp"
#include "Lynx/Core/Input/MouseInput.hpp"
//BUG REPLACE C STYLE CASTS WITH C++ CASTS







namespace lnx{
	//This namespace contains all the structures and functions of render objects
	namespace obj {
		struct Border2;


		enum class limitAlignment : int16{
			eTop,
			eBottom,
			eRight,
			eLeft,
			eCenter
		};



		enum UpdateBits : uint32 {
			eNone    = 0,
			eSpawn   = 0b0001,
			eLimit   = 0b0100,
			eFlush = 0b0010
		};
		alwaysInline UpdateBits operator~ (UpdateBits  a){ return (UpdateBits)((u32)~ (u32)a); }
		alwaysInline UpdateBits operator| (UpdateBits  a, UpdateBits b){ return (UpdateBits )((u32 )a |  (u32)b); }
		alwaysInline UpdateBits operator& (UpdateBits  a, UpdateBits b){ return (UpdateBits )((u32 )a &  (u32)b); }
		alwaysInline UpdateBits operator^ (UpdateBits  a, UpdateBits b){ return (UpdateBits )((u32 )a ^  (u32)b); }
		alwaysInline UpdateBits operator|=(UpdateBits& a, UpdateBits b){ return (UpdateBits&)((u32&)a |= (u32)b); }
		alwaysInline UpdateBits operator&=(UpdateBits& a, UpdateBits b){ return (UpdateBits&)((u32&)a &= (u32)b); }
		alwaysInline UpdateBits operator^=(UpdateBits& a, UpdateBits b){ return (UpdateBits&)((u32&)a ^= (u32)b); }




		// struct Structural : virtual public Obj_bb {
		// 	virtual ram::ptr<char>       getShData() override { dbg::printError("Unable to call getShData on structural object"); return nullptr; }
		// 	virtual vram::Alloc_b<char> getShVData() override { dbg::printError("Unable to call getShVData on structural object"); return vram::Alloc_b<char>(); }
		// };
		// struct Opaque {};




		template<class tChType> struct obj_b;

		/**
		 * @brief Base class of any render object
		 */
		struct obj_bb { //
			obj_bb* parent{ nullptr };				//Parent of the object //FIXME move to common
			RaArray<obj_bb*> children;


			_dbg(const char* dbgName);
			static std::atomic<uint64> lastID;			//#LLID LOS000 the last assigned ID of a Lynx object
			uint64 ID{ ++lastID };						//A unique ID that indentifies the object
			uint32 childIndex{ (uint32)-1 };			//The index of the object in the parent's children list


			virtual void setChildLimits(const uint32 vChildIndex) const = 0;
			virtual ram::ptr<char>       getShData() = 0;
			virtual vram::Alloc_b<char> getShVData() = 0;

			std::atomic<UpdateBits> updates;			//Update requests sent to the render thread //FIXME MAKE NON ATOMIC
			Window* w = nullptr;						//Parent window object that contains the render thread and the window data

			virtual void onSpawn(Window& pWindow);
			virtual void onLimit();
			virtual void onFlush(vk::CommandBuffer pCB);

			//TODO comment
			void queue(UpdateBits vUpdates);
		};






		struct MouseCallbacks_b{
			virtual void onClick(const f32v2 vPos, MouseButton vButton){};
			virtual void onEnter(const f32v2 vPos){};
			virtual void  onExit(const f32v2 vPos){};
			virtual void  onMove(const f32v2 vPos){};
			virtual void  onAxis(const i32  vAxis){};
		};




		struct obj1 : public obj_bb {
			float32 pos{ 0 };				//Position of the object. The position is relative to the origin of the object
			float32 yIndex{ 0 };			//Index of the object. Objects with higher yIndex will be rendered on top of others
			float32 scl{ 0 };				//Scale of the object
			//TODO write all the members
		};




		struct obj2 : public obj_bb, public MouseCallbacks_b {
        	f32v2 pos = { 0, 0 };			//Position of the object. The position is relative to the origin of the object
        	float32 zIndex = 0;				//Index of the object. Objects with higher zIndex will be rendered on top of others
        	float32 rot = 0;				//Rotation of the object
        	f32v2 scl = { 0, 0 };			//Scale of the object

        	f32v2 minLim{ 0, 0 };			//The limit of the object render. It depends on the parent of the object and its properties
        	f32v2 maxLim{ 1, 1 };			//The limit of the object render. It depends on the parent of the object and its properties
        	_rls(inline) void setMinLim(f32v2 vMinLim)_rls({ minLim = vMinLim; });
        	_rls(inline) void setMaxLim(f32v2 vMaxLim)_rls({ maxLim = vMaxLim; });

        	limitAlignment limitAlignment_ = limitAlignment::eCenter; 	//The alignment of the object within its limits

	        virtual void setChildLimits(const uint32 vChildIndex) const override;
        	virtual void onSpawn(Window& pWindow) override;

        	virtual void onLimit() override;
        	virtual void onFlush(vk::CommandBuffer pCB) override;
    	};




		struct obj3 : public obj_bb {
			f32v3 pos{ 0, 0, 0 };			//Position of the object. The position is relative to the origin of the object
			float32 wIndex{ 0 };			//Index of the object. Objects with higher wIndex will be rendered on top of others
			f32v3 rot{ 0, 0, 0 };			//Rotation of the object
			f32v3 scl{ 0, 0, 0 };			//Scale of the object
			//TODO write all the members
    	};
	}
}
