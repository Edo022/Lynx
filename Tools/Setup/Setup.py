import sys, os, re
import SetPlatform, SetConfiguration
#python3.9 -m py_compile Setup.py && { python3 -m PyInstaller -F --clean ./Setup.py; cp ./dist/Setup ./; rm -r ./dist; rm ./build -r; rm ./Setup.spec; }
#! This script must be compiled in an executable file


#TODO make this script an executable
#TODO generate .engine/Path_config.hpp file
#TODO read paths from config file




# Get project dir
rpPath = '.' if len(sys.argv) == 1 else os.path.relpath(sys.argv[1], '.')
apPath = os.path.abspath(rpPath)

rePath = ''
aePath = ''


# Check the project dir and ask for confirmation
if not os.path.exists(apPath):
    print("The specified project path does not exist")
    exit(-2)
else:
    print(
        'The following files will be created or overwritten:'   '\n' +
        apPath + '/.vscode/tasks.json'                          '\n' +
        apPath + '/.vscode/c_cpp_properties.json'               '\n' +
        apPath + '/.engine/.apf'                                '\n' +
        apPath + '/.engine/.atp'                                '\n' +
        apPath + '/.engine/.rePath'                             '\n' +
        '\n'
        'Continue?'
    )
os.path.dirname(os.getcwd())




if sys.stdin.read(1).lower() == 'y':
    aePath = os.path.dirname(os.readlink('/proc/self/exe'))
    rePath = os.path.relpath(aePath + '/../..', apPath)


    # Create missing directories
    if not os.path.exists(apPath + "/.engine"):               os.mkdir(apPath + "/.engine")
    if not os.path.exists(apPath + "/.engine/Build"):         os.mkdir(apPath + "/.engine/Build")
    if not os.path.exists(apPath + "/.engine/Build/Linux"):   os.mkdir(apPath + "/.engine/Build/Linux")
    if not os.path.exists(apPath + "/.engine/Build/Windows"): os.mkdir(apPath + "/.engine/Build/Windows")
    if not os.path.exists(apPath + "/.vscode"):               os.mkdir(apPath + "/.vscode")





    # Write default active platform and configuration
    with open(apPath + "/.engine/.pf", 'w') as f:
        f.write('l')
    with open(apPath + "/.engine/.cf", 'w') as f:
        f.write('d')


    with open(apPath + '/.engine/.rePath', 'w') as f:
        f.write(rePath) # Write relative path to engine SDK

    with open(apPath + '/.engine/.aePath', 'w') as f:
        f.write(aePath) # Write absolute path to engine SDK

    with open(apPath + '/.engine/.rpPath', 'w') as f:
        f.write(rpPath) # Write relative project path

    with open(apPath + '/.engine/.apPath', 'w') as f:
        f.write(apPath) # Write absolute project path




    # Write vscode tasks
    with open(apPath + '/.engine/Build.Engine.sh', 'w') as f:
        f.write(
            f"\n##################################################################"
            f"\n#  This file contains the command used to build the Lynx Engine  #"
            f"\n#  Bash syntax. Lines are concatenated and comments are ignored  #"
            f"\n##################################################################"
            f"\n"
            f"\n{ rePath}/Lynx/Tools/Build/lynxg++"
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
            f"\n    { rePath }/Lynx/shaders/*.comp"
            f"\n"
            f"\n-a: # Use AVX"
            f"\n    -mavx"
            f"\n"
            f"\n-d: # Debug options"
            f"\n    -p -g3 -ggdb3 -DLNX_DEBUG -rdynamic -fverbose-asm                       # Debug informations"
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


    with open(apPath + '/.engine/Build.Application.sh', 'w') as f:
        f.write(
            f"\n##################################################################"
            f"\n#  This file contains the command used to build your application #"
            f"\n#  Bash syntax. Lines are concatenated and comments are ignored  #"
            f"\n##################################################################"
            f"\n"
            f"\n{ rePath}/Lynx/Tools/Build/lynxg++"
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




    with open(apPath + '/.vscode/tasks.json', 'w') as f:
        f.write(
           f'{{'
           f'\n    "version": "2.0.0",'
           f'\n    "tasks": ['
           f'\n        {{'
           f'\n            "type": "shell",'
           f'\n            "label": "Linux  |  Debug  |  Build Lynx Engine",'
           f'\n            "command": "{ rePath }/Tools/Build/lynxg++",'
           f'\n            "args": [ "-f=.engine/Build.Engine.sh" ],'
           f'\n            "problemMatcher": [ "$gcc" ],'
           f'\n            "options": {{ "cwd": "${{workspaceFolder}}" }},'
           f'\n            "group": {{ "kind": "build", "isDefault": true }}'
           f'\n        }},'
           f'\n        {{'
           f'\n            "type": "shell",'
           f'\n            "label": "Linux  |  Debug  |  Build Application\",'
           f'\n            "command": "{ rePath }/Tools/Build/lynxg++",'
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
           f'\n                "{ rePath }/Tools/Setup/SetPlatform.py",'
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
           f'\n                "{ rePath }/Tools/Setup/SetConfiguration.py",'
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
