import sys, os, re, subprocess
#python3.9 -m py_compile Setup.py && { python3 -m PyInstaller -F --clean ./Setup.py; cp ./dist/Setup ./; rm -r ./dist; rm ./build -r; rm ./Setup.spec; }
#! This script must be compiled in an executable file


#TODO make this script an executable
#TODO generate .engine/Path_config.hpp file
#TODO read paths from config file




# Get relative to cwd and absolute application dir
cwdToA = '.' if len(sys.argv) == 1 else os.path.relpath(sys.argv[1], '.')
cwdToE = os.path.relpath(os.path.dirname(os.readlink('/proc/self/exe')) + '/../..')


# Check the application dir and ask for confirmation
if not os.path.exists(cwdToA):
    print("The specified application path does not exist")
    exit(-2)
else:
    print( #TODO add -vsc option
        f'The following files will be created or overwritten:'
        f'\n    { os.path.abspath(cwdToA) }'
        f'\n    ├─ .engine'
        f'\n    │   ├─ Build.Application.sh'
        f'\n    │   ├─ Build.Engine.sh'
        f'\n    │   ├─ Config.hpp'
        f'\n    │   ├─ .Aabs'
        f'\n    │   ├─ .Eabs'
        f'\n    │   ├─ .AtoE'
        f'\n    │   └─ .EtoA'
        f'\n    └─ .vscode'
        f'\n        ├─ tasks.json'
        f'\n        └─ c_cpp_properties.json'
        f'\n\nContinue? (y/n)'
    )
os.path.dirname(os.getcwd())




if sys.stdin.read(1).lower() == 'y':
    # Create missing directories
    if not os.path.exists(cwdToA + "/.engine"): os.mkdir(cwdToA + "/.engine")
    if not os.path.exists(cwdToA + "/.vscode"): os.mkdir(cwdToA + "/.vscode")


    # Write paths
    Aabs = os.path.abspath(cwdToA)
    Eabs = os.path.abspath(cwdToE)
    AtoE = os.path.relpath(Eabs, cwdToA)
    EtoA = os.path.relpath(cwdToA, Eabs)

    os.chdir(cwdToA)
    with open('.engine/.Aabs', 'w') as f: f.write(Aabs)   # Absolute path of Application dir
    with open('.engine/.Eabs', 'w') as f: f.write(Eabs)   # Absolute path of Engine SDK
    with open('.engine/.AtoE', 'w') as f: f.write(AtoE)   # Application to Engine relative path
    with open('.engine/.EtoA', 'w') as f: f.write(EtoA)   # Engine to Application relative path




    # Write vscode tasks
    with open('.engine/Build.Engine.sh', 'w') as f:
        f.write(
            f'\n##################################################################'
            f'\n# This file contains the command used to build the Lynx Engine'
            f'\n# Bash syntax. Lines are concatenated and comments are ignored'
            f'\n##################################################################'
            f'\n'
            f'\n'
            f'\n#--mode=ld #! Build mode and target platform are copied from the application build'
            f'\n'
            f'\nbuild_options {{}}'
            f'\nsource_files {{'
            f'\n    -a:'
            f'\n        src/Lynx/System/SystemInfo.cpp'
            f'\n        src/Lynx/Tests/StructureInit.cpp'
            f'\n        src/Lynx/System/System.cpp'
            f'\n'
            f'\n        src/Lynx/Core/Memory/Ram/Ram.cpp'
            f'\n        src/Lynx/Core/Memory/Ram/Cell_t.cpp'
            f'\n        src/Lynx/Core/Memory/VRam/VRam.cpp'
            f'\n        src/Lynx/Core/Memory/VRam/VCell_t.cpp'
            f'\n'
            f'\n        src/Lynx/Core/Devices.cpp'
            f'\n        src/Lynx/Core/Render/GCommands.cpp'
            f'\n        src/Lynx/Core/Render/Shaders/Shader.cpp'
            f'\n        src/Lynx/Core/Render/Window/Window.cpp #FIXME'
            f'\n        src/Lynx/Core/Render/Window/Swapchain.cpp'
            f'\n'
            f'\n        src/Lynx/Core/Core.cpp'
            f'\n        src/Lynx/Threads/ThreadPool.cpp'
            f'\n'
            f'\n        src/Lynx/Core/Render/Buffers.cpp'
            f'\n        src/Lynx/Core/Render/Render.cpp'
            f'\n        src/Lynx/Types/Object/Obj_b.cpp'
            f'\n        src/Lynx/Types/Object/2D/*.cpp'
            f'\n        src/Lynx/Types/Object/3D/*.cpp'
            f'\n'
            f'\n        src/Lynx/Core/Input/Input.cpp'
            f'\n        src/Lynx/Core/Input/InputState.cpp'
            f'\n'
            f'\n        # App support source files'
            f'\n        src/Lynx/getEnginePath.cpp'
            f'\n        src/Lynx/Core/Env.cpp'
            f'\n'
            f'\n        # Shader source files'
            f'\n        src/Lynx/shaders/*.comp'
            f'\n}}'
            f'\n'
            f'\ngcc_flags {{'
            f'\n    -a: -mavx'
            f'\n'
            f'\n    -d: # Debug options'
            f'\n        -p -g3 -ggdb3 -fverbose-asm                                             # Debug informations'
            f'\n        -O0 -fno-elide-constructors -fno-inline-small-functions -fno-inline     # Optimizations'
            f'\n'
            f'\n    -r: # Release options'
            f'\n        -Ofast -frename-registers -funroll-loops                                # Optimizations'
            f'\n        -g0                                                                     # Debug informations'
            f'\n'
            f'\n    -d: # Sanitizers'
            f'\n        -fsanitize=undefinedm, alignmentm, boundsm, nullm, vptrm, enum'
            f'\n        -fsanitize=leak,address     #! Not compatible with thread'
            f'\n        #-fsanitize=thread          #! Not compatible leak or address'
            f'\n'
            f'\n    -a: # Warnings'
            f'\n        -Wall'
            f'\n        -Wempty-body -Wunused-but-set-parameter -Wredundant-move -Wredundant-decls -Wmissing-include-dirs       # Redundant code'
            f'\n        -Wimplicit-fallthrough=3 -Wshadow -Wdisabled-optimization -Wswitch-default -Woverloaded-virtual         # Implicit code'
            f'\n        -Wshift-negative-value -Wtype-limits -Wuninitialized -Wsign-conversion -Wformat=2 -Wstring-compare      # Bug prone'
            f'\n        -Wnoexcept -Wignored-qualifiers -Wdeprecated-copy                   # Qualifiers'
            f'\n        -Wcast-align -Wcast-qual -Wcast-function-type                       # Casts'
            f'\n        -Wundef -Winit-self -Wstrict-null-sentinel -Wstrict-overflow=5      # Runtime errors'
            f'\n        -Wctor-dtor-privacy -Wlogical-op -Wsign-promo -Wclobbered'
            f'\n}}'
            f'\n'
            f'\ninclude_paths {{}}'
            f'\nforced_includes {{}}'
            f'\nlinker_options {{}} #! Used during executable creation'
        )


    with open('.engine/Build.Application.sh', 'w') as f:
        f.write(
            f'\n##################################################################'
            f'\n# This file contains the command used to build your application'
            f'\n# Bash syntax. Lines are concatenated and comments are ignored'
            f'\n##################################################################'
            f'\n'
            f'\n'
            f'\n'
            f'\n'
            f'\nbuild_options {{'
            f'    --mode=ld #! Changed by vscode' #FIXME actually change the build mode
            f'\n}}'
            f'\n'
            f'\nsource_files {{'
            f'\n    -a: main.cpp #TODO add example file'
            f'\n}}'
            f'\n'
            f'\ngcc_flags{{'
            f'\n    -d: -ggdb3 -g3 -O0'
            f'\n    -r: -g0 -O3'
            f'\n'
            f'\n    -d: # Sanitizers'
            f'\n        -fsanitize=undefined,alignment,bounds,null,vptr,enum'
            f'\n        -fsanitize=leak,address     #! Not compatible with thread'
            f'\n        #-fsanitize=thread          #! Not compatible leak or address'
            f'\n}}'
            f'\n'
            f'\output {{'
            f'\n    ./LynxEngineTest' #TODO actually read the output path #FIXME allow the user to build libraries
            f'\n}}'
            f'\n'
            f'\ninclude_paths {{}}      # e.g.  -I. -Isrc -I path/to/dir'
            f'\nforced_includes {{}}    # e.g.  -include ./file.hpp -include idk.hpp'
            f'\nlinker_options {{}}     # e.g.  -L path/to/libs -lm -lvulkan'
        )



    subprocess.run(['python3', f'{ AtoE }/Tools/Setup/UpdateBuild.py', 'ld'])
    print('Setup completed successfully')
