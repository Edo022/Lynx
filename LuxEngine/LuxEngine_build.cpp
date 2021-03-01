//Link order
//Including cpps like this also improves g++ compilation time
//Do NOT #include this file


#ifndef __INTELLISENSE__
    #include "LuxEngine/Tests/StructureInit.cpp"
    #include "LuxEngine/Core/Memory/Ram/Ram.cpp"
    #include "LuxEngine/Core/Memory/Ram/Cell_t.cpp"
    #include "LuxEngine/Core/Devices.cpp"
    #include "LuxEngine/Core/Core.cpp"

    #include "LuxEngine/Core/Memory/VRam/VRam.cpp"
    #include "LuxEngine/Core/Memory/VRam/VCell_t.cpp"

    #include "LuxEngine/System/System.cpp"
    #include "LuxEngine/Threads/ThreadPool.cpp"
        //#include "LuxEngine/Math/Trigonometry/GoniometricFunctions.cpp"

    #include "LuxEngine/Types/LuxObject/LuxObject.cpp"
    #include "LuxEngine/Types/LuxObject/2D/2DBorder.cpp"
    #include "LuxEngine/Types/LuxObject/2D/2DLines.cpp"
    #include "LuxEngine/Types/LuxObject/2D/2DRenderSpace.cpp"




    #include "LuxEngine/Core/Render/Buffers.cpp"
    //#include "LuxEngine/Core/Render/Compute.cpp"

    #include "LuxEngine/Core/Render/Shaders/Data.cpp"
    #include "LuxEngine/Core/Render/Shaders/Shader.cpp"

    #include "LuxEngine/Core/Render/GCommands.cpp"
    #include "LuxEngine/Core/Render/GOutput.cpp"
    #include "LuxEngine/Core/Render/GSwapchain.cpp"
    #include "LuxEngine/Core/Render/Render.cpp"
    #include "LuxEngine/Core/Render/Window.cpp"

    #include "LuxEngine/Core/Input/Input.cpp"
    #include "LuxEngine/Core/Input/InputState.cpp"
#endif