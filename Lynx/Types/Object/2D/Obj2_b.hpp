#pragma once
#define LNX_H_OBJECT2
#include "Lynx/Types/Object/Obj_b.hpp"
#include "Lynx/Core/Input/MouseInput.hpp"




namespace lnx::obj{
    struct MouseCallbacks_b{
        virtual void onClick(const f32v2 vPos, MouseButton vButton){};
        virtual void onHover(const f32v2 vPos){};
        virtual void onEnter(const f32v2 vPos){};
        virtual void  onExit(const f32v2 vPos){};
        virtual void  onMove(const f32v2 vPos){};
        virtual void  onAxis(const i32  vAxis){};
    };




    //Base class for 2D objects in 2D space
    struct Obj2_b : public Base, public MouseCallbacks_b {

        //TODO add absolute pixel position and scale
        f32v2 pos{ 0, 0 };			//Position of the object. The position is relative to the origin of the object
        float32 zIndex{ 0 };		//Index of the object. Objects with higher zIndex will be rendered on top of others
        float32 rot{ 0 };			//Rotation of the object
        f32v2 scl{ 0, 0 };			//Scale of the object

        //TODO add absolute pixel position and scale
        Obj2_b* parent{ nullptr };						//Parent of the object
        lnx::RaArray<Obj2_b*, uint32> children;			//Children of the object
        virtual bool setChildLimits(const uint32 vChildIndex) const override {
            if(vChildIndex >= children.count()) return false;
            children[vChildIndex]->setMinLim(minLim);
            children[vChildIndex]->setMaxLim(maxLim);
            return true;
        }
        void qHierarchy() {
            for(u32 i = 0; i < children.count(); i++) if(children.isValid(i)) {
                setChildLimits(i);
                //TODO add  recalculateCoords() in all objects
                children[i]->recalculateCoords();
                children[i]->qHierarchy();
            }
            qSelf();
        }

        virtual void onSpawn(Window& pWindow) override;

        limitAlignment limitAlignment_{ limitAlignment::Center }; 	//The alignment of the object within its limits
        f32v2 minLim{ 0, 0 };										//The limit of the object render. It depends on the parent of the object and its properties
        f32v2 maxLim{ 1, 1 };										//The limit of the object render. It depends on the parent of the object and its properties
        _rls(inline) void setMinLim(f32v2 vMinLim)_rls({ minLim = vMinLim; });
        _rls(inline) void setMaxLim(f32v2 vMaxLim)_rls({ maxLim = vMaxLim; });
        _dbg(Border2* debugBorder = nullptr;)					    //Debug. Used to draw the object limits
    };
}