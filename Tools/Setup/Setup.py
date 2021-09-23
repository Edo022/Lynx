import sys, os, re
import SetPlatform, SetConfiguration
#python3.9 -m py_compile Setup.py && { python3 -m PyInstaller -F --clean ./Setup.py; cp ./dist/Setup ./; rm -r ./dist; rm ./build -r; rm ./Setup.spec; }
#! This script must be compiled in an executable file


#TODO make this script an executable
#TODO generate .engine/Path_config.hpp file
#TODO read paths from config file




# Get relative to cwd and absolute project dir
cwdToP = '.' if len(sys.argv) == 1 else os.path.relpath(sys.argv[1], '.')
cwdToE = os.path.relpath(os.path.dirname(os.readlink('/proc/self/exe')) + '/../..')


# Check the project dir and ask for confirmation
if not os.path.exists(cwdToP):
    print("The specified project path does not exist")
    exit(-2)
else:
    print(
        'The following files will be created or overwritten:'   '\n' +
        cwdToP + '/.vscode/tasks.json'                          '\n' +
        cwdToP + '/.vscode/c_cpp_properties.json'               '\n' +
        cwdToP + '/.engine/.apf'                                '\n' +
        cwdToP + '/.engine/.atp'                                '\n' +
        cwdToP + '/.engine/.cwdToE'                             '\n' +
        '\n'
        'Continue?'
    )
os.path.dirname(os.getcwd())




if sys.stdin.read(1).lower() == 'y':
    # Create missing directories
    if not os.path.exists(cwdToP + "/.engine"): os.mkdir(cwdToP + "/.engine")
    if not os.path.exists(cwdToP + "/.vscode"): os.mkdir(cwdToP + "/.vscode")


    # Write default active platform and configuration
    with open(cwdToP + "/.engine/.pf", 'w') as f: f.write('l')
    with open(cwdToP + "/.engine/.cf", 'w') as f: f.write('d')

    # Write paths
    pabs = os.path.abspath(cwdToP)
    eabs = os.path.abspath(cwdToE)
    ptoe = os.path.relpath(eabs, cwdToP)
    etop = os.path.relpath(cwdToP, eabs)
    with open(cwdToP + '/.engine/.pabs', 'w') as f: f.write(pabs)   # Absolute path of Project dir
    with open(cwdToP + '/.engine/.eabs', 'w') as f: f.write(eabs)   # Absolute path of Engine SDK
    with open(cwdToP + '/.engine/.ptoe', 'w') as f: f.write(ptoe)   # Project to Engine relative path
    with open(cwdToP + '/.engine/.etop', 'w') as f: f.write(etop)   # Engine to Project relative path




    # Write vscode tasks
    with open(cwdToP + '/.engine/Build.Engine.sh', 'w') as f:
        f.write(
            f"\n##################################################################"
            f"\n"
            f"\n# This file contains the command used to build the Lynx Engine"
            f"\n# Bash syntax. Lines are concatenated and comments are ignored"
            f"\n"
            f"\n##################################################################"
            f"\n"
            f"\n"
            f"\n{ ptoe }/Lynx/Tools/Build/lynxg++"
            f"\n"
            f"\n#--mode=ld #! Build mode and target platform are copied from the application build"
            f"\n-a: # Source files"
            f"\n    Lynx/System/SystemInfo.cpp"
            f"\n    Lynx/Tests/StructureInit.cpp"
            f"\n"
            f"\n    Lynx/Core/Memory/Ram/Ram.cpp"
            f"\n    Lynx/Core/Memory/Ram/Cell_t.cpp"
            f"\n"
            f"\n    Lynx/System/System.cpp"
            f"\n"
            f"\n    Lynx/Core/Devices.cpp"
            f"\n    Lynx/Core/Memory/VRam/VRam.cpp"
            f"\n    Lynx/Core/Memory/VRam/VCell_t.cpp"
            f"\n"
            f"\n    Lynx/Core/Render/GCommands.cpp"
            f"\n    Lynx/Core/Render/Shaders/Shader.cpp"
            f"\n    Lynx/Core/Render/Window/Window.cpp #FIXME"
            f"\n    Lynx/Core/Render/Window/Swapchain.cpp"
            f"\n    Lynx/Core/Core.cpp"
            f"\n"
            f"\n    Lynx/Threads/ThreadPool.cpp"
            f"\n"
            f"\n    Lynx/Types/Object/Obj_b.cpp"
            f"\n    Lynx/Types/Object/2D/Line2.cpp"
            f"\n    Lynx/Types/Object/2D/Border2.cpp"
            f"\n    Lynx/Types/Object/2D/RenderSpace2.cpp"
            f"\n    Lynx/Types/Object/2D/RenderSpace3.cpp"
            f"\n"
            f"\n    Lynx/Types/Object/3D/Volume.cpp"
            f"\n"
            f"\n    Lynx/Core/Render/Buffers.cpp"
            f"\n    Lynx/Core/Render/Render.cpp"
            f"\n"
            f"\n    Lynx/Core/Input/Input.cpp"
            f"\n    Lynx/Core/Input/InputState.cpp"
            f"\n"
            f"\n-a: # App support source files"
            f"\n    Lynx/getEnginePath.cpp"
            f"\n    Lynx/Core/Env.cpp"
            f"\n"
            f"\n-a: # Shader source files"
            f"\n    Lynx/shaders/*.comp"
            f"\n"
            f"\n-a: # Use AVX"
            f"\n    -mavx"
            f"\n"
            f"\n-d: # Debug options"
            f"\n    -p -g3 -ggdb3 -fverbose-asm                                             # Debug informations"
            f"\n    -O0 -fno-elide-constructors -fno-inline-small-functions -fno-inline     # Optimizations"
            f"\n"
            f"\n-r: # Release options"
            f"\n    -Ofast -frename-registers -funroll-loops                                # Optimizations"
            f"\n    -g0                                                                     # Debug informations"
            f"\n"
            f"\n-d: # Sanitizers"
            f"\n    -fsanitize=undefined"
            f"\n    -fsanitize=alignment"
            f"\n    -fsanitize=bounds"
            f"\n    -fsanitize=null"
            f"\n    -fsanitize=vptr"
            f"\n    -fsanitize=enum"
            f"\n    -fsanitize=leak      #! Not compatible with thread"
            f"\n    -fsanitize=address   #! Not compatible with thread"
            f"\n    #-fsanitize=thread   #! Not compatible leak or address"
            f"\n"
            f"\n-a: # Warnings"
            f"\n    -Wall"
            f"\n    -Wclobbered"
            f"\n    -Wcast-function-type"
            f"\n    -Wdeprecated-copy"
            f"\n    -Wempty-body"
            f"\n    -Wignored-qualifiers"
            f"\n    -Wimplicit-fallthrough=3"
            f"\n    -Wstring-compare"
            f"\n    -Wredundant-move"
            f"\n    -Wtype-limits"
            f"\n    -Wuninitialized"
            f"\n    -Wshift-negative-value"
            f"\n    -Wunused-but-set-parameter"
            f"\n    -Wcast-align"
            f"\n    -Wcast-qual"
            f"\n    -Wctor-dtor-privacy"
            f"\n    -Wdisabled-optimization"
            f"\n    -Wformat=2"
            f"\n    -Winit-self"
            f"\n    -Wlogical-op"
            f"\n    -Wmissing-include-dirs"
            f"\n    -Wnoexcept"
            f"\n    -Woverloaded-virtual"
            f"\n    -Wredundant-decls"
            f"\n    -Wshadow"
            f"\n    -Wsign-conversion"
            f"\n    -Wsign-promo"
            f"\n    -Wstrict-null-sentinel"
            f"\n    -Wstrict-overflow=5"
            f"\n    -Wswitch-default"
            f"\n    -Wundef"
            f"\n"
            f"\n    #! Hard coded -c"
            f"\n    #! Hard coded output paths"
        )


    with open(cwdToP + '/.engine/Build.Application.sh', 'w') as f:
        f.write(
            f"\n##################################################################"
            f"\n"
            f"\n# This file contains the command used to build your application   "
            f"\n# Bash syntax. Lines are concatenated and comments are ignored    "
            f"\n"
            f"\n##################################################################"
            f"\n"
            f"\n"
            f"\n{ ptoe }/Lynx/Tools/Build/lynxg++"
            f"\n"
            f"\n"
            f"--mode=ld #! Changed by vscode"
            f"\n-d:"
            f"\n    -ggdb3 -g3"
            f"\n    -O0"
            f"\n-r:"
            f"\n    -g0"
            f"\n    -O3"
            f"\n-a:"
            f"\n    main.cpp" #TODO add example file
            f"\n    -o LynxEngineTest"
        )




    with open(cwdToP + '/.vscode/tasks.json', 'w') as f:
        f.write(
           f'{{'
           f'\n    "version": "2.0.0",'
           f'\n    "tasks": ['
           f'\n        {{'
           f'\n            "type": "shell",'
           f'\n            "label": "Linux  |  Debug  |  Build Lynx Engine",'
           f'\n            "command": "{ ptoe }/Tools/Build/lynxg++",'
           f'\n            "args": [ "-f=.engine/Build.Engine.sh" ],'
           f'\n            "problemMatcher": [ "$gcc" ],'
           f'\n            "options": {{ "cwd": "${{workspaceFolder}}" }},'
           f'\n            "group": {{ "kind": "build", "isDefault": true }}'
           f'\n        }},'
           f'\n        {{'
           f'\n            "type": "shell",'
           f'\n            "label": "Linux  |  Debug  |  Build Application\",'
           f'\n            "command": "{ ptoe }/Tools/Build/lynxg++",'
           f'\n            "args": [ "-f=.engine/Build.Application.sh" ],'
           f'\n            "problemMatcher": [ "$gcc" ],'
           f'\n            "options": {{ "cwd": "${{workspaceFolder}}" }},'
           f'\n            "group": {{ "kind": "build", "isDefault": true }}'
           f'\n        }},'
           f'\n        {{'
           f'\n            "type": "shell",'
           f'\n            "label": " > Switch to Windows",'
           f'\n            "command": "python3",'
           f'\n            "args": ['
           f'\n                "{ ptoe }/Tools/Setup/SetPlatform.py",'
           f'\n                "w",'
           f'\n                "ignored_argument" //FIXME'
           f'\n            ],'
           f'\n            "problemMatcher": [ ],'
           f'\n            "options": {{ "cwd": "${{workspaceFolder}}" }},'
           f'\n            "group": {{ "kind": "build", "isDefault": true }}'
           f'\n        }},'
           f'\n        {{'
           f'\n            "type": "shell",'
           f'\n            "label": " > Switch to Release",'
           f'\n            "command": "python3",'
           f'\n            "args": ['
           f'\n                "{ ptoe }/Tools/Setup/SetConfiguration.py",'
           f'\n                "r",'
           f'\n                "ignored_argument" //FIXME'
           f'\n            ],'
           f'\n            "problemMatcher": [ ],'
           f'\n            "options": {{ "cwd": "${{workspaceFolder}}" }},'
           f'\n            "group": {{ "kind": "build", "isDefault": true }}'
           f'\n        }}'
           f'\n    ]'
           f'\n}}'
        )
    # SetPlatform.run('l')
    # SetConfiguration.run('d')
