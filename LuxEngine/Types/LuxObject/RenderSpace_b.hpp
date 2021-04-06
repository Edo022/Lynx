#pragma once
#define LUX_H_RENDER_SPACE
#include "LuxEngine/Types/LuxObject/2D/Obj2_b.hpp"



namespace lux::obj{
    struct RenderSpace_b : public Obj2_b{
        void qSelf() final override {}

    };
}

