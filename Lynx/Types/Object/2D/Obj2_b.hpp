#pragma once
#define LNX_H_OBJECT2
#include "Lynx/Types/Object/Obj_b.hpp"
#include "Lynx/Core/Input/MouseInput.hpp"



//TODO Opaque and Structural objects
//TODO add absolute pixel position and scale
namespace lnx::obj{
    struct MouseCallbacks_b{
        virtual void onClick(const f32v2 vPos, MouseButton vButton){};
        virtual void onEnter(const f32v2 vPos){};
        virtual void  onExit(const f32v2 vPos){};
        virtual void  onMove(const f32v2 vPos){};
        virtual void  onAxis(const i32  vAxis){};
    };




    //Base class for 2D objects in 2D space
    struct Obj2_bb : public MouseCallbacks_b, virtual public Obj_bb {
        f32v2 pos = { 0, 0 };	                //Position of the object. The position is relative to the origin of the object
        float32 zIndex = 0;		                //Index of the object. Objects with higher zIndex will be rendered on top of others
        float32 rot = 0;	                    //Rotation of the object
        f32v2 scl = { 0, 0 };	                //Scale of the object

        f32v2 minLim{ 0, 0 };				    //The limit of the object render. It depends on the parent of the object and its properties
        f32v2 maxLim{ 1, 1 };			        //The limit of the object render. It depends on the parent of the object and its properties
        _rls(inline) void setMinLim(f32v2 vMinLim)_rls({ minLim = vMinLim; });
        _rls(inline) void setMaxLim(f32v2 vMaxLim)_rls({ maxLim = vMaxLim; });
        _dbg(Border2* debugBorder = nullptr;)   //Debug. Used to draw the object limits

        Obj2_bb* parent{ nullptr };				//Parent of the object
        // virtual void qHierarchy(){};
        virtual void onLimit() override {
            Obj_bb::onLimit();
            parent->setChildLimits(common.childIndex);
        } //FIXME idk. This doesnt seem right
    };



    /**
     * @brief Obj2_b trampoline
     */
    template<class chType> struct Obj2_bt : public Obj2_bb, public Obj_bt<chType> {
        limitAlignment limitAlignment_ = limitAlignment::eCenter; 	//The alignment of the object within its limits

        virtual void setChildLimits(const uint32 vChildIndex) const override;
        // virtual void qHierarchy() override;
        virtual void onSpawn(Window& pWindow) override;
    };


    /**
     * @brief Base class of 2D objects
     * @tparam chType Dimensions of the child objects. Can be 1, 2 or 3
     */
    template<uint32 chType> struct Obj2_b {};

    // template<> struct Obj2_b<1> : public Obj_bt<Obj1_bb> {};
    template<> struct Obj2_b<2> : public Obj2_bt<Obj2_bb> {};
    // template<> struct Obj2_b<3> : public Obj_bt<Obj3_bb> {};
}