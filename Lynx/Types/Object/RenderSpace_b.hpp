#pragma once
#define LNX_H_RENDER_SPACE
#include "Lynx/Types/Object/2D/Obj2_b.hpp"



namespace lnx::obj{
    struct RenderSpace_b : public Obj2_b<2>{
        void qSelf() final override {} //FIXME REMOVE
        void onUpdateg() final override {
            dbg::checkCond(thr::self::thr() != render.parentWindow->t.thr, "This function can only be called by the render thread.");
        }
    };
}

