import sys, os, re, subprocess


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
        f'\n    │   ├─ Application.lnxbuild.sh'
        f'\n    │   ├─ Engine.lnxbuild.sh'
        f'\n    │   ├─ .setup'
        f'\n    │   │   ├─ .Aabs'
        f'\n    │   │   ├─ .Eabs'
        f'\n    │   │   ├─ .AtoE'
        f'\n    │   │   └─ .EtoA'
        f'\n    │   └─ .editor'
        f'\n    │       └─ .intellisense.hpp'
        f'\n    └─ .vscode'
        f'\n        ├─ tasks.json'
        f'\n        ├─ launch.json'
        f'\n        └─ c_cpp_properties.json'
        f'\n\nContinue? (y/n)'
    )
os.path.dirname(os.getcwd())




if sys.stdin.read(1).lower() == 'y':
    # Create missing directories
    os.makedirs(exist_ok = True, name = './.vscode')
    os.makedirs(exist_ok = True, name = './.engine')
    os.makedirs(exist_ok = True, name = './.engine/.editor')
    os.makedirs(exist_ok = True, name = './.engine/.setup')


    # Write paths
    Aabs = os.path.abspath(cwdToA)
    Eabs = os.path.abspath(cwdToE)
    AtoE = os.path.relpath(Eabs, cwdToA)
    EtoA = os.path.relpath(cwdToA, Eabs)

    os.chdir(cwdToA)
    with open('.engine/.setup/.Aabs', 'w') as f: f.write(Aabs)   # Absolute path of Application dir
    with open('.engine/.setup/.Eabs', 'w') as f: f.write(Eabs)   # Absolute path of Engine SDK
    with open('.engine/.setup/.AtoE', 'w') as f: f.write(AtoE)   # Application to Engine relative path
    with open('.engine/.setup/.EtoA', 'w') as f: f.write(EtoA)   # Engine to Application relative path




    # Write launch configurations
    with open('.vscode/launch.json', 'w') as f:
        f.write(
            f'\n            {{'
            f'\n    "version": "0.2.0",'
            f'\n    "configurations": ['
            f'\n        {{'
            f'\n            "name": "Application (debug)",'
            f'\n            "type": "cppdbg",'
            f'\n            "request": "launch",'
            f'\n            "program": "${{workspaceFolder}}/Application",'
            f'\n            "args": [],'
            f'\n            "stopAtEntry": false,'
            f'\n            "cwd": "${{workspaceFolder}}",'
            f'\n            "environment": [],'
            f'\n            "externalConsole": false,'
            f'\n            "MIMode": "gdb",'
            f'\n            "setupCommands": ['
            f'\n                {{'
            f'\n                    "text": "-enable-pretty-printing",'
            f'\n                    "ignoreFailures": true'
            f'\n                }},'
            f'\n                {{'
            f'\n                    "text": "handle SIGSTOP nostop noprint pass" }},{{      //Suspend thread //TODO'
            f'\n                    "text": "handle SIGCONT nostop noprint pass"          //Resume thread //TODO'
            f'\n                }}'
            f'\n            ]'
            f'\n        }}'
            f'\n    ]'
            f'\n}}'
        )




    # Write C/C++ file
    with open('.vscode/c_cpp_properties.json', 'w') as f:
        f.write(
            f'\n{{'
            f'\n    "configurations": ['
            f'\n        {{'
            f'\n            "name": "Lynx-editor",'
            f'\n            "includePath": ['
            f'\n                "${{workspaceFolder}}/.engine/.src", //Generated source files'
            f'\n                "${{workspaceFolder}}/{ AtoE }/src" //Lynx Engine source files'
            f'\n            ],'
            f'\n            "defines": [],'
            f'\n            "compilerPath": "/usr/bin/g++",'
            f'\n            "cStandard": "${{default}}",'
            f'\n            "cppStandard": "c++20",'
            f'\n            "intelliSenseMode": "linux-gcc-x64",'
            f'\n            "forcedInclude": ['
            f'\n                ".engine/.editor/.intellisense.hpp", //This helps Intellisense to correctly parse your code'
            f'\n                "${{workspaceFolder}}/{ AtoE }/src/Lynx/Core/InitList.hpp" //Generated initializers'
            f'\n            ]'
            f'\n        }}'
            f'\n    ],'
            f'\n    "version": 4'
            f'\n}}'
        )




    # Write vscode tasks
    with open('.engine/Engine.lnxbuild.sh', 'w') as f:
        f.write(
            f'\n##################################################################'
            f'\n# This file contains the command used to build the Lynx Engine'
            f'\n# Bash syntax. Lines are concatenated and comments are ignored'
            f'\n##################################################################'
            f'\n'
            f'\n'
            f'\n'
            f'\n'
            f'\n{{'
            f'\n    cpp {{'
            f'\n        a {{'
            f'\n            src/Lynx/System/SystemInfo.cpp;'
            f'\n            src/Lynx/Tests/StructureInit.cpp;'
            f'\n            src/Lynx/System/System.cpp;'
            f'\n'
            f'\n            src/Lynx/Core/Memory/Ram/Ram.cpp;'
            f'\n            src/Lynx/Core/Memory/Ram/Cell_t.cpp;'
            f'\n            src/Lynx/Core/Memory/VRam/VRam.cpp;'
            f'\n            src/Lynx/Core/Memory/VRam/VCell_t.cpp;'
            f'\n'
            f'\n            src/Lynx/Core/Devices.cpp;'
            f'\n            src/Lynx/Core/Render/GCommands.cpp;'
            f'\n            src/Lynx/Core/Render/Shaders/Shader.cpp;'
            f'\n            src/Lynx/Core/Render/Window/Window.cpp; #FIXME'
            f'\n            src/Lynx/Core/Render/Window/Swapchain.cpp;'
            f'\n'
            f'\n            src/Lynx/Core/Core.cpp;'
            f'\n            src/Lynx/Threads/ThreadPool.cpp;'
            f'\n'
            f'\n            src/Lynx/Core/Render/Buffers.cpp;'
            f'\n            src/Lynx/Core/Render/Render.cpp;'
            f'\n            src/Lynx/Types/Object/Obj_b.cpp;'
            f'\n            src/Lynx/Types/Object/2D/*.cpp;'
            f'\n            src/Lynx/Types/Object/3D/*.cpp;'
            f'\n'
            f'\n            src/Lynx/Core/Input/Input.cpp;'
            f'\n            src/Lynx/Core/Input/InputState.cpp;'
            f'\n'
            f'\n            # App support source files'
            f'\n            src/Lynx/getEnginePath.cpp;'
            f'\n            src/Lynx/Core/Env.cpp;'
            f'\n        }}'
            f'\n    }}'
            f'\n'
            f'\n    gls {{'
            f'\n        a {{ src/Lynx/shaders/*.comp }}'
            f'\n    }}'
            f'\n'
            f'\n    defines         {{}}'
            f'\n    forced_includes {{}}'
            f'\n    include_paths   {{}}'
            f'\n'
            f'\n    compiler_flags {{'
            f'\n        a {{ -mavx }}'
            f'\n'
            f'\n        d {{ # Debug options'
            f'\n            -p; -g3; -ggdb3; -fverbose-asm;                                            # Debug informations'
            f'\n            -O0; -fno-elide-constructors; -fno-inline-small-functions; -fno-inline     # Optimizations'
            f'\n        }}'
            f'\n'
            f'\n        r {{ # Release options'
            f'\n            -g0;                                          # Debug informations'
            f'\n            -Ofast; -frename-registers; -funroll-loops;   # Optimizations'
            f'\n        }}'
            f'\n'
            f'\n        d {{ # Sanitizers'
            f'\n            -fsanitize=undefined,alignment,bounds,null,vptr,enum;'
            f'\n            -fsanitize=leak,address,pointer-compare,pointer-subtract;   #! Not compatible with thread'
            f'\n            #-fsanitize=thread;                                         #! Not compatible leak, address, pointer-compare or pointer-subtract'
            f'\n            -fsanitize-address-use-after-scope; -fstack-protector-strong'
            f'\n        }}'
            f'\n'
            f'\n        a {{ # Warnings'
            f'\n            -Wall;'
            f'\n            -Wempty-body; -Wunused-but-set-parameter; -Wredundant-move; -Wredundant-decls; -Wmissing-include-dirs;      # Redundant code'
            f'\n            -Wimplicit-fallthrough=3; -Wshadow; -Wdisabled-optimization; -Wswitch-default; -Woverloaded-virtual;        # Implicit code'
            f'\n            -Wshift-negative-value; -Wtype-limits; -Wuninitialized; -Wsign-conversion; -Wformat=2; -Wstring-compare;    # Bug prone'
            f'\n            -Wnoexcept; -Wignored-qualifiers; -Wdeprecated-copy;                  # Qualifiers'
            f'\n            -Wcast-align; -Wcast-qual; -Wcast-function-type;                      # Casts'
            f'\n            -Wundef; -Winit-self; -Wstrict-null-sentinel; -Wstrict-overflow=5;    # Runtime errors'
            f'\n            -Wctor-dtor-privacy; -Wlogical-op; -Wsign-promo; -Wclobbered'
            f'\n        }}'
            f'\n    }}'
            f'\n'
            f'\n    #! Used during executable creation'
            f'\n    linker_flags  {{'
            f'\n        a {{ #TODO fix windows build'
            f'\n            -L/usr/lib64; -L/lib64;         # Prefer 64bit libraries'
            f'\n            -ldl; -lrt; -lXrandr; -lXi;     # Link dependencies'
            f'\n            -lXcursor; -lXinerama; -lX11;   # Link dependencies'
            f'\n            -lvulkan; -Bstatic; -lglfw      # Link Vulkan dynamically and GLFW statically'
            f'\n        }}'
            f'\n    }}'
            f'\n'
            f'\n    #! Hard coded'
            f'\n    output{{}}'
            f'\n}}'
        )


    with open('.engine/Application.lnxbuild.sh', 'w') as f:
        f.write(
            f'\n##################################################################'
            f'\n# This file contains the command used to build your application'
            f'\n# Bash syntax. Lines are concatenated and comments are ignored'
            f'\n##################################################################'
            f'\n'
            f'\n'
            f'\n'
            f'\n'
            f'\n{{'
            f'\n    cpp {{ a {{ main.cpp }} }}  # A list of C++ source files'
            f'\n    gls {{}}                  # A list of GLSL source files'
            f'\n'
            f'\n    defines         {{}}      # -D option           # e.g.  -DVAR=1 -D mogu_mogu'       #TODO REMOVE -D
            f'\n    forced_includes {{}}      #                     # e.g.  ./file.hpp idk.hpp'
            f'\n    include_paths   {{}}      # -I option           # e.g.  -I. -Isrc -I path/to/dir'   #TODO REMOVE -I
            f'\n'
            f'\n    # Any other compiler flag'
            f'\n    compiler_flags {{'
            f'\n        d {{ -ggdb3; -g3; -O0 }}'
            f'\n        r {{ -g0; -O3 }}'
            f'\n'
            f'\n        d {{ # Sanitizers'
            f'\n            -fsanitize=undefined,alignment,bounds,null,vptr,enum;'
            f'\n            -fsanitize=leak,address,pointer-compare,pointer-subtract;   #! Not compatible with thread'
            f'\n            #-fsanitize=thread;                                         #! Not compatible leak, address, pointer-compare or pointer-subtract'
            f'\n            -fsanitize-address-use-after-scope; -fstack-protector-strong'
            f'\n        }}'
            f'\n    }}'
            f'\n'
            f'\n    # Any linker flag'
            f'\n    linker_flags{{}}'
            f'\n'
            f'\n    # The output file. It can be a library (.a, .lib, .so, .dll) or an executable file (any other extension, or no extension)'
            f'\n    output {{'
            f'\n        a {{ ./Application }}' #TODO actually read the output path #FIXME allow the user to build libraries
            f'\n    }}'
            f'\n}}'
        )



    subprocess.run([f'{ AtoE }/Tools/Setup/UpdateBuild', 'ld'])
    print('Setup completed successfully') #TODO check if it was actually successful