//############################################################################################
//!  This file is supposed to be used during the compilation of the engine source files
//!  It allows the engine to be initialized before any user defined global variable
//!  Do not compile or include it. It's done automatically
//############################################################################################




#include "Generated/.init/SystemInfo.hpp"
#include "Generated/.init/StructureInit.hpp"

#include "Generated/.init/Ram.hpp"
#include "Generated/.init/Cell_t.hpp"

#include "Generated/.init/System.hpp"

#include "Generated/.init/Devices.hpp"
#include "Generated/.init/VRam.hpp"
#include "Generated/.init/VCell_t.hpp"

#include "Generated/.init/GCommands.hpp"
#include "Generated/.init/Shader.hpp"
#include "Generated/.init/Window.hpp" //FIXME
#include "Generated/.init/Swapchain.hpp"

#include "Generated/.init/Core.hpp"
#include "Generated/.init/ThreadPool.hpp"

#include "Generated/.init/Buffers.hpp"
#include "Generated/.init/Render.hpp"

#include "Generated/.init/Input.hpp"
#include "Generated/.init/InputState.hpp"
