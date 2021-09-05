# from Lynx.Setup.BuildOptions import enginePath
import sys, os
import SetPlatform, SetType
thisdir = os.path.dirname(os.path.abspath(__file__))

#TODO make this script an executable
#TODO generate .engine/Path_config.hpp file
#TODO read paths from config file


if len(sys.argv) == 1:
    pdir = os.getcwd()
else:
    pdir = sys.argv[1]

if not os.path.exists(pdir):
    print("The specified path does not exist")
    exit(-2)




print('Those files will be created or overwritten:\n' +\
    os.path.abspath(pdir) + '/.vscode/tasks.json\n' +\
    os.path.abspath(pdir) + '/.vscode/c_cpp_properties.json\n' +\
    'Continue?'
)




if sys.stdin.read(1).lower() == 'y':
    if not os.path.exists(pdir + "/.engine"):
        os.mkdir(pdir + "/.engine")
    if not os.path.exists(pdir + "/.vscode"):
        os.mkdir(pdir + "/.vscode")


    with open(pdir + "/.engine/platform", 'w') as f:
        f.write('l')
    with open(pdir + "/.engine/type", 'w') as f:
        f.write('d')


    _epath = os.path.relpath(thisdir + '/../..', pdir)
    with open('./.engine/enginePath', 'w') as pf:
        pf.write(_epath)




    with open(pdir + '/.vscode/tasks.json', 'w') as uf:
        uf.write(
           f'{{ '                                                                              '\n'
           f'    "version": "2.0.0",'                                                          '\n'
           f'    "tasks": ['                                                                   '\n'
           f'        {{'                                                                       '\n'
           f'            "type": "shell",'                                                     '\n'
           f'            "label": "Linux  |  Debug  |  Build Lynx Engine",'                    '\n' #FIXME build executable from linux script
           f'            "command": "{ _epath }/Tools/Build/lynxg++",'                         '\n'
           f'            "args": [ "--mode=ld",'                                               '\n'
           f'                "--engine",'                                                      '\n'
           f'                "-a:"'                                                            '\n'
           f'                    "{ _epath }/Lynx/Lynx_build.cpp",'                            '\n'
           f'                    "{ _epath }/Lynx/shaders/*.comp",'                            '\n'
           f'                    "-pipe",'                                                     '\n'
           f'                    "-mavx",'                                                     '\n'
           f''                                                                                 '\n'
           f'                "-d:",'                                                           '\n'
           f'                    "-p", "-g3", "-ggdb3",'                                       '\n'
           f'                    "-rdynamic",'                                                 '\n'
           f'                    "-fverbose-asm",'                                             '\n'
           f'                    "-DLNX_DEBUG",'                                               '\n'
           f'                    "-O0",'                                                       '\n'
           f'                    "-fno-elide-constructors",'                                   '\n'
           f'                    "-fno-inline-small-functions",'                               '\n'
           f'                    "-fno-inline",'                                               '\n'
           f''                                                                                 '\n'
           f'                "-r:",'                                                           '\n'
           f'                    "-g0", "-Ofast",'                                             '\n'
           f'                    "-frename-registers",'                                        '\n'
           f'                    "-funroll-loops",'                                            '\n'
           f''                                                                                 '\n'
           f'                "-d:",'                                                           '\n'
           f'                    "-fsanitize=undefined",'                                      '\n'
           f'                    "-fsanitize=alignment",'                                      '\n'
           f'                    "-fsanitize=bounds",'                                         '\n'
           f'                    "-fsanitize=null",'                                           '\n'
           f'                    "-fsanitize=vptr",'                                           '\n'
           f'                    "-fsanitize=enum",'                                           '\n'
           f'                    "-fsanitize=leak",      //!Not compatible with thread'        '\n'
           f'                    "-fsanitize=address",   //!Not compatible with thread'        '\n'
           f'                    // "-fsanitize=thread",    //!Not compatible leak or address' '\n'
           f''                                                                                 '\n'
           f'                "-a:",'                                                           '\n'
           f'                    "-Wall",'                                                     '\n'
           f'                    "-Wclobbered",'                                               '\n'
           f'                    "-Wcast-function-type",'                                      '\n'
           f'                    "-Wdeprecated-copy",'                                         '\n'
           f'                    "-Wempty-body",'                                              '\n'
           f'                    "-Wignored-qualifiers",'                                      '\n'
           f'                    "-Wimplicit-fallthrough=3",'                                  '\n'
           f'                    "-Wstring-compare",'                                          '\n'
           f'                    "-Wredundant-move",'                                          '\n'
           f'                    "-Wtype-limits",'                                             '\n'
           f'                    "-Wuninitialized",'                                           '\n'
           f'                    "-Wshift-negative-value",'                                    '\n'
           f'                    "-Wunused-but-set-parameter",'                                '\n'
           f'                    "-Wcast-align",'                                              '\n'
           f'                    "-Wcast-qual",'                                               '\n'
           f'                    "-Wctor-dtor-privacy",'                                       '\n'
           f'                    "-Wdisabled-optimization",'                                   '\n'
           f'                    "-Wformat=2",'                                                '\n'
           f'                    "-Winit-self",'                                               '\n'
           f'                    "-Wlogical-op",'                                              '\n'
           f'                    "-Wmissing-include-dirs",'                                    '\n'
           f'                    "-Wnoexcept",'                                                '\n'
           f'                    "-Woverloaded-virtual",'                                      '\n'
           f'                    "-Wredundant-decls",'                                         '\n'
           f'                    "-Wshadow",'                                                  '\n'
           f'                    "-Wsign-conversion",'                                         '\n'
           f'                    "-Wsign-promo",'                                              '\n'
           f'                    "-Wstrict-null-sentinel",'                                    '\n'
           f'                    "-Wstrict-overflow=5",'                                       '\n'
           f'                    "-Wswitch-default",'                                          '\n'
           f'                    "-Wundef",'                                                   '\n'
           f''                                                                                 '\n'
           f'                    "-c", "-o",'                                                  '\n'
           f'                "-d:", "{ _epath }/Build/Linux/LynxDebug",'                       '\n'
           f'                "-r:", "{ _epath }/Build/Linux/LynxRelease",'                     '\n'
           f'            ],'                                                                   '\n'
           f'            "problemMatcher": [ "$gcc" ],'                                        '\n'
           f'            "options": {{ "cwd": "${{workspaceFolder}}" }},'                      '\n'
           f'            "group": {{ "kind": "build", "isDefault": true }}'                    '\n'
           f'        }},'                                                                      '\n'
           f'        {{'                                                                       '\n'
           f'            "type": "shell",'                                                     '\n'
           f'            "label": "Linux  |  Debug  |  Build Application\",'                   '\n'
           f'            "command": "{ _epath }/Tools/Build/lynxg++",'                         '\n'
           f'            "args": [ "--mode=ld",'                                               '\n'
           f'                //Your build'                                                     '\n'
           f'            ],'                                                                   '\n'
           f'            "problemMatcher": [ "$gcc" ],'                                        '\n'
           f'            "options": {{ "cwd": "${{workspaceFolder}}" }},'                      '\n'
           f'            "group": {{ "kind": "build", "isDefault": true }}'                    '\n'
           f'        }},'                                                                      '\n'
           f'        {{'                                                                       '\n'
           f'            "type": "shell",'                                                     '\n'
           f'            "label": " > Switch to Windows",'                                     '\n'
           f'            "command": "python3",'                                                '\n'
           f'            "args": ['                                                            '\n'
           f'                "{ _epath }/Tools/Setup/SetPlatform.py",'                         '\n'
           f'                "w",'                                                             '\n'
           f'                "ignored_argument" //FIXME'                                       '\n'
           f'            ],'                                                                   '\n'
           f'            "problemMatcher": [ ],'                                               '\n'
           f'            "options": {{ "cwd": "${{workspaceFolder}}" }},'                      '\n'
           f'            "group": {{ "kind": "build", "isDefault": true }}'                    '\n'
           f'        }},'                                                                      '\n'
           f'        {{'                                                                       '\n'
           f'            "type": "shell",'                                                     '\n'
           f'            "label": " > Switch to Release",'                                     '\n'
           f'            "command": "python3",'                                                '\n'
           f'            "args": ['                                                            '\n'
           f'                "{ _epath }/Tools/Setup/SetType.py",'                             '\n'
           f'                "r",'                                                             '\n'
           f'                "ignored_argument" //FIXME'                                       '\n'
           f'            ],'                                                                   '\n'
           f'            "problemMatcher": [ ],'                                               '\n'
           f'            "options": {{ "cwd": "${{workspaceFolder}}" }},'                      '\n'
           f'            "group": {{ "kind": "build", "isDefault": true }}'                    '\n'
           f'        }}'                                                                       '\n'
           f'    ]'                                                                            '\n'
           f'}}'                                                                               '\n'
        )
    # SetPlatform.run('l')
    # SetType.run('d')
