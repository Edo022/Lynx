#pragma once
#define LNX_H_LNX_OBJECT

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








		/**
		 * @brief Base class of any render object
		 */
		struct obj_bb { //
		    inline void specStructOnFlush(vk::CommandBuffer pCB) {
	            obj_bb::onFlush(pCB);
	        }

			//FIXME USE VRAM PTR INSTEAD OF ALLOC_B
	        inline      ram::ptr<char>  specStructGetShData() { dbg::printError("Unable to call this function on structural objects"); return nullptr; }
	        inline vram::Alloc_b<char> specStructGetShVData() { dbg::printError("Unable to call this function on structural objects"); return vram::Alloc_b<char>(); }



			inline void specRenderOnFlush(vk::CommandBuffer pCB) {
				pCB.updateBuffer(
					getShVData().cell->csc.buffer,
					getShVData().cell->localOffset,
					getShVData().cell->cellSize,
					(void*)getShData()
				);
			}

			//! Implementation depends on the shader used by the object
			//TODO write documentation
			inline      ram::ptr<char>  specRenderGetShData() { return obj_bb::getShData(); }
			inline vram::Alloc_b<char> specRenderGetShVData() { return obj_bb::getShVData(); }




		public:
			obj_bb* parent{ nullptr };				//Parent of the object //FIXME move to common
			RaArray<obj_bb*> children;


			_dbg(const char* dbgName);
			static std::atomic<uint64> lastID;			//#LLID LOS000 the last assigned ID of a Lynx object
			uint64 ID{ ++lastID };						//A unique ID that indentifies the object
			uint32 childIndex{ (uint32)-1 };			//The index of the object in the parent's children list


			virtual void setChildLimits(const uint32 vChildIndex) const { dbg::printError("Unable to call this function on a base class"); }
			virtual ram::ptr<char>       getShData() {dbg::printError("Function called on base class or not implemented"); return nullptr; }
			virtual vram::Alloc_b<char> getShVData() {dbg::printError("Function called on base class or not implemented"); return vram::Alloc_b<char>(); }

			std::atomic<UpdateBits> updates;			//Update requests sent to the render thread //FIXME MAKE NON ATOMIC
			Window* w = nullptr;						//Parent window object that contains the render thread and the window data

			virtual void onSpawn(Window& pWindow);
			virtual void onLimit();
			virtual void onFlush(vk::CommandBuffer pCB);

			//TODO comment
			void queue(UpdateBits vUpdates);
		};








		// Base object types ----------------------------------------------------------------------------------------------------------------------//








		enum ObjType{
			eRender,
			eStruct
		};








		// 1D objects base class ------------------------------------------------------------------------------------------------------------------//








		struct obj1_b : public obj_bb {
			float32 pos{ 0 };				//Position of the object. The position is relative to the origin of the object
			float32 yIndex{ 0 };			//Index of the object. Objects with higher yIndex will be rendered on top of others
			float32 scl{ 0 };				//Scale of the object
			//TODO write all the members
		};


		template<ObjType tObjType> struct obj1_spec_b{};
		template<> struct obj1_spec_b<ObjType::eRender> : public obj1_b{
			void                onFlush(vk::CommandBuffer pCB) override {        obj_bb::specRenderOnFlush(pCB); }
			ram::ptr<char>      getShData()                    override { return obj_bb::specRenderGetShData();  }
			vram::Alloc_b<char> getShVData()                   override { return obj_bb::specRenderGetShVData(); }
		};
		template<> struct obj1_spec_b<ObjType::eStruct> : public obj1_b{
			void                onFlush(vk::CommandBuffer pCB) override {        obj_bb::specStructOnFlush(pCB); }
			ram::ptr<char>      getShData()                    override { return obj_bb::specStructGetShData();  }
			vram::Alloc_b<char> getShVData()                   override { return obj_bb::specStructGetShVData(); }
		};


		struct RenderObj1 : public obj1_spec_b<ObjType::eRender> {};
		struct StructObj1 : public obj1_spec_b<ObjType::eStruct> {};








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
			virtual void onSpawn(Window& pWindow) override;
			virtual void onLimit() override;
    	};




		template<ObjType tObjType> struct obj2_spec_b{};
		template<> struct obj2_spec_b<ObjType::eRender> : public obj2_b{
			void                onFlush(vk::CommandBuffer pCB) override {        obj_bb::specRenderOnFlush(pCB); }
			ram::ptr<char>      getShData()                    override { return obj_bb::specRenderGetShData();  }
			vram::Alloc_b<char> getShVData()                   override { return obj_bb::specRenderGetShVData(); }
		};
		template<> struct obj2_spec_b<ObjType::eStruct> : public obj2_b{
			void                onFlush(vk::CommandBuffer pCB) override {        obj_bb::specStructOnFlush(pCB); }
			ram::ptr<char>      getShData()                    override { return obj_bb::specStructGetShData();  }
			vram::Alloc_b<char> getShVData()                   override { return obj_bb::specStructGetShVData(); }
		};




		struct RenderObj2 : public obj2_spec_b<ObjType::eRender>{};
		struct StructObj2 : public obj2_spec_b<ObjType::eStruct>{};








		// 3D objects base class ------------------------------------------------------------------------------------------------------------------//








		struct obj3_b : public obj_bb {
			f32v3 pos{ 0, 0, 0 };			//Position of the object. The position is relative to the origin of the object
			float32 wIndex{ 0 };			//Index of the object. Objects with higher wIndex will be rendered on top of others
			f32v3 rot{ 0, 0, 0 };			//Rotation of the object
			f32v3 scl{ 0, 0, 0 };			//Scale of the object
			//TODO write all the members
    	};


		template<ObjType tObjType> struct obj3_spec_b{};
		template<> struct obj3_spec_b<ObjType::eRender> : public obj3_b{
			void                onFlush(vk::CommandBuffer pCB) override {        obj_bb::specRenderOnFlush(pCB); }
			ram::ptr<char>      getShData()                    override { return obj_bb::specRenderGetShData();  }
			vram::Alloc_b<char> getShVData()                   override { return obj_bb::specRenderGetShVData(); }
		};
		template<> struct obj3_spec_b<ObjType::eStruct> : public obj3_b{
			void                onFlush(vk::CommandBuffer pCB) override {        obj_bb::specStructOnFlush(pCB); }
			ram::ptr<char>      getShData()                    override { return obj_bb::specStructGetShData();  }
			vram::Alloc_b<char> getShVData()                   override { return obj_bb::specStructGetShVData(); }
		};


		struct RenderObj3 : public obj3_spec_b<eRender> {};
		struct StructObj3 : public obj3_spec_b<eStruct> {};
	}
}
