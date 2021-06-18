#pragma once
#define LNX_H_LNX_OBJECT

#include "Lynx/Types/Vectors/Vectors.hpp"
#include "Lynx/Types/Containers/String.hpp"
#include "Lynx/Core/Render/Shaders/Shader.hpp"
#include "Lynx/Types/VPointer.hpp"
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
			eUpdateg = 0b0010
		};
		alwaysInline UpdateBits operator~ (UpdateBits  a){ return (UpdateBits)((u32)~ (u32)a); }
		alwaysInline UpdateBits operator| (UpdateBits  a, UpdateBits b){ return (UpdateBits )((u32 )a |  (u32)b); }
		alwaysInline UpdateBits operator& (UpdateBits  a, UpdateBits b){ return (UpdateBits )((u32 )a &  (u32)b); }
		alwaysInline UpdateBits operator^ (UpdateBits  a, UpdateBits b){ return (UpdateBits )((u32 )a ^  (u32)b); }
		alwaysInline UpdateBits operator|=(UpdateBits& a, UpdateBits b){ return (UpdateBits&)((u32&)a |= (u32)b); }
		alwaysInline UpdateBits operator&=(UpdateBits& a, UpdateBits b){ return (UpdateBits&)((u32&)a &= (u32)b); }
		alwaysInline UpdateBits operator^=(UpdateBits& a, UpdateBits b){ return (UpdateBits&)((u32&)a ^= (u32)b); }








		// template<uint32 tVal> struct GetObjClass{};
		// template<> struct GetObjClass<1>{ using value = Obj1_bb; };
		// template<> struct GetObjClass<2>{ using value = Obj2_bb; };
		// template<> struct GetObjClass<3>{ using value = Obj3_bb; };

		// template<uint32 tDim, uint32 tChDim, class tBase> struct ObjSelector : public GetObjClass<tDim>::value, public tBase {
		// 	static_assert(tDim < 1 || tDim > 3, "Invalid tDim value");
		// 	static_assert(tChDim < 1 || tChDim > 3, "Invalid tDim value");
		// 	static_assert(!std::is_same<tBase, Opaque> || !std::is_same<tBase, Structural>, "Invalid base class");
		// 	RaArray<GetObjClass<tChDim>::value> children;
		// };








		// struct Structural : virtual public Obj_bb {
		// 	virtual ram::ptr<char>       getShData() override { dbg::printError("Unable to call getShData on structural object"); return nullptr; }
		// 	virtual vram::Alloc_b<char> getShVData() override { dbg::printError("Unable to call getShVData on structural object"); return vram::Alloc_b<char>(); }
		// };
		// struct Opaque {};







		struct NoChType_t{};
		template<class tChType> struct obj_b;

		/**
		 * @brief Members common to any Obj_bt instantiation
		 */
		struct obj_bb { //
			_dbg(const char* dbgName;)
			struct Common{
				static std::atomic<uint64> lastID;							//#LLID LOS000 the last assigned ID of a Lynx object
				uint64 ID{ ++lastID };							//A unique ID that indentifies the object
				uint32 childIndex{ (uint32)-1 };				//The index of the object in the parent's children list
			} common;
			virtual void setChildLimits(const uint32 vChildIndex) const = 0;
			virtual ram::ptr<char>       getShData() = 0;
			virtual vram::Alloc_b<char> getShVData() = 0;

			virtual obj_b<NoChType_t>* getChildren(uint32 vIndex) = 0;			//FIXME UNIFY CHILDREN ACCESS FUNCTIONS
			virtual uint32  getChildrenCount() = 0;					//FIXME UNIFY CHILDREN ACCESS FUNCTIONS
			virtual bool    getChildrenIsValid(uint32 vIndex) = 0; 	//FIXME UNIFY CHILDREN ACCESS FUNCTIONS

			struct Render{									//Structure containing rendering helper members
				// _dbg(bool isDbgObj = false;)					//True if the object is used for graphical debugging
				std::atomic<UpdateBits> updates;				//Update requests sent to the render thread //FIXME MAKE NON ATOMIC
				Window* parentWindow = nullptr;					//Parent window object that contains the render thread and the window data
			} render;

			virtual void onSpawn(Window& pWindow){
				dbg::checkCond(render.parentWindow && thr::self::thr() != render.parentWindow->renderCore.t.thr, "This function can only be called by the render thread.");
			}
			virtual void onLimit(){
				dbg::checkCond(render.parentWindow && thr::self::thr() != render.parentWindow->renderCore.t.thr, "This function can only be called by the render thread.");
			}
			virtual void onUpdateg(vk::CommandBuffer pCB){
				dbg::checkCond(render.parentWindow && thr::self::thr() != render.parentWindow->renderCore.t.thr, "This function can only be called by the render thread.");
			}

			//TODO comment
			void queue(UpdateBits vUpdates){
				UpdateBits old = render.updates;						//Save old updates bits
				if(render.parentWindow) { 								//If the object has a binded window
					render.parentWindow->renderCore.requests_m.lock();					//Lock requests mutex
						render.updates = render.updates | vUpdates;			//Update updates bits
						if(!old) render.parentWindow->renderCore.requests.add(this);	//If it isn't already in it, add the object to the update queue
					render.parentWindow->renderCore.requests_m.unlock();				//Unlock requests mutex
				}														//If not
				else render.updates = render.updates | vUpdates;			//Update updates bits
			}
		};







		template<class tChType = NoChType_t> struct obj_b : public obj_bb{
			RtArray<tChType*, uint32> children;
			virtual obj_b<>* getChildren(uint32 vIndex) override { return static_cast<obj_b<>*>(children[vIndex]); } 	//FIXME UNIFY CHILDREN ACCESS FUNCTIONS
			virtual uint32   getChildrenCount() override { return children.count(); } 								//FIXME UNIFY CHILDREN ACCESS FUNCTIONS
			virtual bool     getChildrenIsValid(uint32 vIndex) override { return children.isValid(vIndex); } 		//FIXME UNIFY CHILDREN ACCESS FUNCTIONS
		};

		template<> struct obj_b<NoChType_t> : public obj_bb{
			virtual obj_b<>* getChildren(uint32 vIndex) override {        dbg::printError("Children access function called on object of a type that has no children"); return nullptr; } //FIXME UNIFY CHILDREN ACCESS FUNCTIONS //FIXME or return nullptr instead of printing an error or something, idk
			virtual uint32   getChildrenCount() override {                dbg::printError("Children access function called on object of a type that has no children"); return 0; }                       //FIXME UNIFY CHILDREN ACCESS FUNCTIONS //FIXME or return nullptr instead of printing an error or something, idk
			virtual bool     getChildrenIsValid(uint32 vIndex) override { dbg::printError("Children access function called on object of a type that has no children"); return false; }               //FIXME UNIFY CHILDREN ACCESS FUNCTIONS //FIXME or return nullptr instead of printing an error or something, idk
		};






// //FIXME
// 		/**
// 		 * @brief Base class of any render object of any dimension
// 		 * @tparam chType Type of the children objects. Can be any subclass of Obj_b
// 		 */
// 		template<class chType, class tType> struct Obj_bt : virtual public Obj_bb, public tType {
//                         static_assert(std::is_same<tType, Opaque> || std::is_same<tType, Structural>, "Object type can only be \"Structural\" or \"Opaque\"");
// 			RaArray<chType*, uint32> children;
// 			virtual Obj_bb* getChildren(uint32 vIndex) override { return static_cast<Obj_bb*>(children[vIndex]); } 	//FIXME UNIFY CHILDREN ACCESS FUNCTIONS
// 			virtual uint32  getChildrenCount() override { return children.count(); } 								//FIXME UNIFY CHILDREN ACCESS FUNCTIONS
// 			virtual bool    getChildrenIsValid(uint32 vIndex) override { return children.isValid(vIndex); } 		//FIXME UNIFY CHILDREN ACCESS FUNCTIONS
// 		};















		struct MouseCallbacks_b{
			virtual void onClick(const f32v2 vPos, MouseButton vButton){};
			virtual void onEnter(const f32v2 vPos){};
			virtual void  onExit(const f32v2 vPos){};
			virtual void  onMove(const f32v2 vPos){};
			virtual void  onAxis(const i32  vAxis){};
		};




		// // static constexpr uint32 AnyDim = (uint32)-1;

		// template<uint32 tDim, class tChType> struct obj_b<tChType>{
		// 	static_assert(tDim >= 1 && tDim <= 3, "Invalid tDim value. tDim can only be 1, 2 or 3");
		// };
		template<class tChType = NoChType_t> using obj = obj_b<tChType>;
		// using obj = obj<>;



		template<class tChType> struct obj1 : public obj_b<tChType> {
			float32 pos{ 0 };				//Position of the object. The position is relative to the origin of the object
			float32 yIndex{ 0 };			//Index of the object. Objects with higher yIndex will be rendered on top of others
			float32 scl{ 0 };				//Scale of the object

			////TODO add absolute pixel position and scale
			//Obj2_b* parent{ nullptr };						//Parent of the object
			//lnx::Map<Obj2_b*, uint32> children;				//Children of the object
			//void setChildLimits(const uint32 vChildIndex) final {
			//	children[vChildIndex]->minLim = minLim;
			//	children[vChildIndex]->maxLim = maxLim;
			//}
			//vec2f32 minLim{ 0, 0 };							//The limit of the object render. It depends on the parent of the object and its properties
			//vec2f32 maxLim{ 0, 0 };							//The limit of the object render. It depends on the parent of the object and its properties
		};
		// template<class tChType = NoChType_t> using obj1 = obj_t<1, tChType>;




		template<class tChType> struct obj2 : public obj_b<tChType>, public MouseCallbacks_b {
        	f32v2 pos = { 0, 0 };	                //Position of the object. The position is relative to the origin of the object
        	float32 zIndex = 0;		                //Index of the object. Objects with higher zIndex will be rendered on top of others
        	float32 rot = 0;	                    //Rotation of the object
        	f32v2 scl = { 0, 0 };	                //Scale of the object

        	f32v2 minLim{ 0, 0 };				    //The limit of the object render. It depends on the parent of the object and its properties
        	f32v2 maxLim{ 1, 1 };			        //The limit of the object render. It depends on the parent of the object and its properties
        	_rls(inline) void setMinLim(f32v2 vMinLim)_rls({ minLim = vMinLim; });
        	_rls(inline) void setMaxLim(f32v2 vMaxLim)_rls({ maxLim = vMaxLim; });
        	// _dbg(Border2* debugBorder = nullptr;)   //Debug. Used to draw the object limits

        	limitAlignment limitAlignment_ = limitAlignment::eCenter; 	//The alignment of the object within its limits

	        virtual void setChildLimits(const uint32 vChildIndex) const override;
        	virtual void onSpawn(Window& pWindow) override;

        	obj2<obj_bb>* parent{ nullptr };				//Parent of the object

        	virtual void onLimit() override;
        	virtual void onUpdateg(vk::CommandBuffer pCB) override;
    	};
		// template<class tChType = NoChType_t> using obj2 = obj_t<2, tChType>;

//TODO ADD CHILDREN TYPE


		template<class tChType> struct obj3 : public obj_b<tChType> {
			f32v3 pos{ 0, 0, 0 };			//Position of the object. The position is relative to the origin of the object
			float32 wIndex{ 0 };			//Index of the object. Objects with higher wIndex will be rendered on top of others
			f32v3 rot{ 0, 0, 0 };			//Rotation of the object
			f32v3 scl{ 0, 0, 0 };			//Scale of the object

			////TODO add absolute pixel position and scale
			//Obj3_b* parent{ nullptr };						//Parent of the object
			//lnx::Map<Obj3_b*, uint32> children;				//Children of the object
			//void setChildLimits(const uint32 vChildIndex) const override {
			//	children[vChildIndex]->minLim = minLim;
			//	children[vChildIndex]->maxLim = maxLim;
			//}
			//vec3f32 minLim{ 0, 0, 0 };						//The limit of the object render. It depends on the parent of the object and its properties
			//vec3f32 maxLim{ 1, 1, 1 };						//The limit of the object render. It depends on the parent of the object and its properties
    	};
		// template<class tChType = NoChType_t> using obj3 = obj_t<3, tChType>;
	}
}
