#pragma once
#define LNX_H_RENDER_SPACE
#include "Lynx/Types/Object/2D/Obj2_b.hpp"



namespace lnx::obj{
    struct RenderSpace_b : public Obj2_b<Obj2_bb>{
        void qSelf() final override {}

    };
}

