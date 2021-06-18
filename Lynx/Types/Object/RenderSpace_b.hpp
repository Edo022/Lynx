#pragma once
#define LNX_H_RENDER_SPACE
#include "Lynx/Types/Object/Obj_b.hpp"



namespace lnx::obj{
    struct RenderSpace_b : public obj2<obj2<>>{
        void onUpdateg(vk::CommandBuffer pCB) final override {
            obj_bb::onUpdateg(pCB);
        }

        virtual      ram::ptr<char> getShData() override { dbg::printError("Unable to call this base function"); return nullptr; }
        virtual vram::Alloc_b<char> getShVData() override{ dbg::printError("Unable to call this base function"); return vram::Alloc_b<char>(); }
    };
}

