//############################################################################################
//!  This file is supposed to be used during the compilation of the engine source files
//!  It allows the engine to be initialized before any user defined global variable
//!  Do not compile or include it. It's done automatically
//############################################################################################


#include <mutex>
#include <deque>
#include <atomic>
#include <Lynx/Core/IncludeVulkan.hpp>
#include <GLFW/glfw3.h>
#include "Lynx/Types/Integers/Integers.hpp"





namespace lnx{
    template<class tType, class tIdxt> struct RaArrayC;
    template<class tType, class tIdxt> struct RtArray;
    template<class tType, std::integral tIdxt> struct RaArray;
    class String;
    struct Thread;

    namespace sys{
        struct CpuInfo;
        struct RamInfo;
        struct GpuInfo;
    }

    namespace ram{
        struct Type_t;
        struct Cell_t;
        template<class tType> struct ptr;
    }

    namespace core::dvc{
        struct Device;
    }

    namespace vram{
        struct Type_t2;
        struct Cell_t2;
    }

    namespace shd{
        struct ShaderInterface_b;
        struct InterfaceLayout;
    }

    namespace thr::_pvt{
        struct Func_b;
    }
}








#include "Generated/.init/Env.init.hpp"
#include "Generated/.init/SystemInfo.init.hpp"
#include "Generated/.init/StructureInit.init.hpp"

// #include "Generated/.init/Ram.hpp"
#include "Generated/.init/Cell_t.init.hpp"

#include "Generated/.init/System.init.hpp"

#include "Generated/.init/Devices.init.hpp"
#include "Generated/.init/VRam.init.hpp"
#include "Generated/.init/VCell_t.init.hpp"

#include "Generated/.init/GCommands.init.hpp"
#include "Generated/.init/Shader.init.hpp"

#include "Generated/.init/Shaders/Border2.ilsl.init.hpp"
#include "Generated/.init/Shaders/FloatToIntBuffer.ilsl.init.hpp"
#include "Generated/.init/Shaders/Line2.ilsl.init.hpp"
#include "Generated/.init/Shaders/Volume.ilsl.init.hpp"
//TODO AUTOMATIZE
// #include "Generated/.init/Window.hpp" //FIXME
// #include "Generated/.init/Swapchain.hpp"

#include "Generated/.init/Core.init.hpp"
#include "Generated/.init/ThreadPool.init.hpp"

// #include "Generated/.init/Buffers.hpp"
#include "Generated/.init/Render.init.hpp"

// #include "Generated/.init/Input.hpp"
// #include "Generated/.init/InputState.hpp"
