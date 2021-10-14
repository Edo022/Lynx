#pragma once

#include "Lynx/Types/Vectors/Vectors.hpp"
#include "Lynx/Types/Containers/String.hpp"
#include "Lynx/Core/Render/Shaders/Shader.hpp"
#include "Lynx/Types/VPointer.hpp"
#include "Lynx/Core/Input/MouseInput.hpp"


//TODO REPLACE C STYLE CASTS WITH C++ CASTS







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








		// Base object class ----------------------------------------------------------------------------------------------------------------------//








		enum class ObjType{
			eRender,
			eStruct,
			eBase
		};


		/**
		 * @brief Base class of any render object
		 */
		struct obj_bb {
		protected:
			ObjType objType = ObjType::eBase;


		public:
			obj_bb* parent = nullptr;				//Parent of the object //FIXME move to common
			RaArray<obj_bb*> children;


			_dbg(const char* dbgName);
			static std::atomic<uint64> lastID;			//#LLID LOS000 the last assigned ID of a Lynx object
			uint64 ID{ ++lastID };						//A unique ID that indentifies the object
			uint32 childIndex{ (uint32)-1 };			//The index of the object in the parent's children list


			virtual void setChildLimits(const uint32 vChildIndex) const {
				dbg::logError("Unable to call this function on a base class");
			}
			virtual ram::ptr     <char> getShData();
			virtual vram::Alloc_b<char> getShVData();

			std::atomic<UpdateBits> updates;			//Update requests sent to the render thread //FIXME MAKE NON ATOMIC
			Window* w = nullptr;						//Parent window object that contains the render thread and the window data

			virtual void onSpawn(core::RenderCore& pRenderCore);
			virtual void onLimit();
			virtual void onFlush(vk::CommandBuffer pCB);

			//TODO comment
			void queue(UpdateBits vUpdates);
		};








		// 1D objects base class ------------------------------------------------------------------------------------------------------------------//








		struct obj1_b : public obj_bb {
			float32 pos{ 0 };				//Position of the object. The position is relative to the origin of the object
			float32 yIndex{ 0 };			//Index of the object. Objects with higher yIndex will be rendered on top of others
			float32 scl{ 0 };				//Scale of the object
			//TODO write all the members
		};


		struct RenderObj1 : public obj1_b {
			RenderObj1(){
				obj_bb::objType = ObjType::eRender;
			}
		};
		struct StructObj1 : public obj1_b {
			StructObj1(){
				obj_bb::objType = ObjType::eStruct;
			}
		};








		// 2D objects base class ------------------------------------------------------------------------------------------------------------------//








		struct MouseCallbacks_b{
			virtual void onClick(const f32v2 vPos, MouseButton vButton){};
			virtual void onEnter(const f32v2 vPos){};
			virtual void  onExit(const f32v2 vPos){};
			virtual void  onMove(const f32v2 vPos){};
			virtual void  onAxis(const i32  vAxis){};
		};




		struct obj2_b : public obj_bb, public MouseCallbacks_b {
        	f32v2 pos = { 0, 0 };			//Position of the object. The position is relative to the origin of the object
        	float32 zIndex = 0;				//Index of the object. Objects with higher zIndex will be rendered on top of others
        	float32 rot = 0;				//Rotation of the object
        	f32v2 scl = { 0, 0 };			//Scale of the object

        	f32v2 minLim{ 0, 0 };			//The limit of the object render. It depends on the parent of the object and its properties
        	f32v2 maxLim{ 1, 1 };			//The limit of the object render. It depends on the parent of the object and its properties
			inline void setMinLim(f32v2 vMinLim) { minLim = vMinLim; };
			inline void setMaxLim(f32v2 vMaxLim) { maxLim = vMaxLim; };

        	limitAlignment limitAlignment_ = limitAlignment::eCenter; 	//The alignment of the object within its limits


			virtual void setChildLimits(const uint32 vChildIndex) const override;
			virtual void onSpawn(core::RenderCore& pRenderCore) override;
			virtual void onLimit() override;
    	};


		struct RenderObj2 : public obj2_b {
			RenderObj2(){
				obj_bb::objType = ObjType::eRender;
			}
		};
		struct StructObj2 : public obj2_b {
			StructObj2(){
				obj_bb::objType = ObjType::eStruct;
			}
		};








		// 3D objects base class ------------------------------------------------------------------------------------------------------------------//








		struct obj3_b : public obj_bb {
			f32v3 pos{ 0, 0, 0 };			//Position of the object. The position is relative to the origin of the object
			float32 wIndex{ 0 };			//Index of the object. Objects with higher wIndex will be rendered on top of others
			f32v3 rot{ 0, 0, 0 };			//Rotation of the object
			f32v3 scl{ 0, 0, 0 };			//Scale of the object
			//TODO write all the members
    	};


		struct RenderObj3 : public obj3_b {
			RenderObj3(){
				obj_bb::objType = ObjType::eRender;
			}
		};
		struct StructObj3 : public obj3_b {
			StructObj3(){
				obj_bb::objType = ObjType::eStruct;
			}
		};
	}
}
