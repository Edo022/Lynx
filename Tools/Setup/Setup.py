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


    # Write relative path to engine SDK
    with open(apPath + '/.engine/.rePath', 'w') as f:
        f.write(rePath)


    # Write absolute project path
    with open(apPath + '/.engine/.apPath', 'w') as f:
        f.write(apPath)




    # Write vscode tasks
    with open(apPath + '/.vscode/tasks.json', 'w') as f:
        f.write(
           f'{{'
           f'\n    "version": "2.0.0",'
           f'\n    "tasks": ['
           f'\n        {{'
           f'\n            "type": "shell",'
           f'\n            "label": "Linux  |  Debug  |  Build Lynx Engine",'
           f'\n            "command": "{ rePath }/Tools/Build/lynxg++",'
           f'\n            "args": [ "--mode=ld",'
           f'\n                "--engine",'
           f'\n                "-a:",'
           f'\n                    "{ rePath }/Lynx/Lynx_build.cpp",'
           f'\n                    "{ rePath }/Lynx/shaders/*.comp",'
           f'\n                    "-pipe",'
           f'\n                    "-mavx",'
           f'\n'
           f'\n                "-d:",'
           f'\n                    "-p", "-g3", "-ggdb3",'
           f'\n                    "-rdynamic",'
           f'\n                    "-fverbose-asm",'
           f'\n                    "-DLNX_DEBUG",'
           f'\n                    "-O0",'
           f'\n                    "-fno-elide-constructors",'
           f'\n                    "-fno-inline-small-functions",'
           f'\n                    "-fno-inline",'
           f'\n'
           f'\n                "-r:",'
           f'\n                    "-g0", "-Ofast",'
           f'\n                    "-frename-registers",'
           f'\n                    "-funroll-loops",'
           f'\n'
           f'\n                "-d:",'
           f'\n                    "-fsanitize=undefined",'
           f'\n                    "-fsanitize=alignment",'
           f'\n                    "-fsanitize=bounds",'
           f'\n                    "-fsanitize=null",'
           f'\n                    "-fsanitize=vptr",'
           f'\n                    "-fsanitize=enum",'
           f'\n                    "-fsanitize=leak",      //!Not compatible with thread'
           f'\n                    "-fsanitize=address",   //!Not compatible with thread'
           f'\n                    // "-fsanitize=thread",    //!Not compatible leak or address'
           f'\n'
           f'\n                "-a:",'
           f'\n                    "-Wall",'
           f'\n                    "-Wclobbered",'
           f'\n                    "-Wcast-function-type",'
           f'\n                    "-Wdeprecated-copy",'
           f'\n                    "-Wempty-body",'
           f'\n                    "-Wignored-qualifiers",'
           f'\n                    "-Wimplicit-fallthrough=3",'
           f'\n                    "-Wstring-compare",'
           f'\n                    "-Wredundant-move",'
           f'\n                    "-Wtype-limits",'
           f'\n                    "-Wuninitialized",'
           f'\n                    "-Wshift-negative-value",'
           f'\n                    "-Wunused-but-set-parameter",'
           f'\n                    "-Wcast-align",'
           f'\n                    "-Wcast-qual",'
           f'\n                    "-Wctor-dtor-privacy",'
           f'\n                    "-Wdisabled-optimization",'
           f'\n                    "-Wformat=2",'
           f'\n                    "-Winit-self",'
           f'\n                    "-Wlogical-op",'
           f'\n                    "-Wmissing-include-dirs",'
           f'\n                    "-Wnoexcept",'
           f'\n                    "-Woverloaded-virtual",'
           f'\n                    "-Wredundant-decls",'
           f'\n                    "-Wshadow",'
           f'\n                    "-Wsign-conversion",'
           f'\n                    "-Wsign-promo",'
           f'\n                    "-Wstrict-null-sentinel",'
           f'\n                    "-Wstrict-overflow=5",'
           f'\n                    "-Wswitch-default",'
           f'\n                    "-Wundef",'
           f'\n'
           f'\n                    "-c", "-o",'
           f'\n                "-ld:", "./.engine/Build/Linux/LynxDebug",'
           f'\n                "-lr:", "./.engine/Build/Linux/LynxRelease",'
           f'\n                "-wd:", "./.engine/Build/Windows/LynxDebug",'
           f'\n                "-wr:", "./.engine/Build/Windows/LynxRelease",'
           f'\n            ],'
           f'\n            "problemMatcher": [ "$gcc" ],'
           f'\n            "options": {{ "cwd": "${{workspaceFolder}}" }},'
           f'\n            "group": {{ "kind": "build", "isDefault": true }}'
           f'\n        }},'
           f'\n        {{'
           f'\n            "type": "shell",'
           f'\n            "label": "Linux  |  Debug  |  Build Application\",'
           f'\n            "command": "{ rePath }/Tools/Build/lynxg++",'
           f'\n            "args": [ "--mode=ld",'
           f'\n                //Your build'
           f'\n            ],'
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
