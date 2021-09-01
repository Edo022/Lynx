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
           '{ '                                                                               '\n'
           '    "version": "2.0.0",'                                                          '\n'
           '    "tasks": ['                                                                   '\n'
           '        {'                                                                        '\n'
           '            "type": "shell",'                                                     '\n'
           '            "label": "Linux  |  Debug  |  Build Lynx Engine",'                    '\n' #FIXME build executable from linux script
           '            "command": "' + _epath + '/Tools/Build/lynxg++",'                     '\n'
           '            "args": [ "--mode=ld",'                                               '\n'
           '                "--engine",'                                                      '\n'
           '                "-a:"'                                                            '\n'
           '                    "' + _epath + '/Lynx/Lynx_build.cpp",'                        '\n'
           '                    "' + _epath + '/Lynx/shaders/*.comp",'                        '\n'
           '                    "-pipe",'                                                     '\n'
           '                    "-mavx",'                                                     '\n'
           ''                                                                                 '\n'
           '                "-d:",'                                                           '\n'
           '                    "-p", "-g3", "-ggdb3",'                                       '\n'
           '                    "-rdynamic",'                                                 '\n'
           '                    "-fverbose-asm",'                                             '\n'
           '                    "-DLNX_DEBUG",'                                               '\n'
           '                    "-O0",'                                                       '\n'
           '                    "-fno-elide-constructors",'                                   '\n'
           '                    "-fno-inline-small-functions",'                               '\n'
           '                    "-fno-inline",'                                               '\n'
           ''                                                                                 '\n'
           '                "-r:",'                                                           '\n'
           '                    "-g0", "-Ofast",'                                             '\n'
           '                    "-frename-registers",'                                        '\n'
           '                    "-funroll-loops",'                                            '\n'
           ''                                                                                 '\n'
           '                "-d:",'                                                           '\n'
           '                    "-fsanitize=undefined",'                                      '\n'
           '                    "-fsanitize=alignment",'                                      '\n'
           '                    "-fsanitize=bounds",'                                         '\n'
           '                    "-fsanitize=null",'                                           '\n'
           '                    "-fsanitize=vptr",'                                           '\n'
           '                    "-fsanitize=enum",'                                           '\n'
           '                    "-fsanitize=leak",      //!Not compatible with thread'        '\n'
           '                    "-fsanitize=address",   //!Not compatible with thread'        '\n'
           '                    // "-fsanitize=thread",    //!Not compatible leak or address' '\n'
           ''                                                                                 '\n'
           '                "-a:",'                                                           '\n'
           '                    "-Wall",'                                                     '\n'
           '                    "-Wclobbered",'                                               '\n'
           '                    "-Wcast-function-type",'                                      '\n'
           '                    "-Wdeprecated-copy",'                                         '\n'
           '                    "-Wempty-body",'                                              '\n'
           '                    "-Wignored-qualifiers",'                                      '\n'
           '                    "-Wimplicit-fallthrough=3",'                                  '\n'
           '                    "-Wstring-compare",'                                          '\n'
           '                    "-Wredundant-move",'                                          '\n'
           '                    "-Wtype-limits",'                                             '\n'
           '                    "-Wuninitialized",'                                           '\n'
           '                    "-Wshift-negative-value",'                                    '\n'
           '                    "-Wunused-but-set-parameter",'                                '\n'
           '                    "-Wcast-align",'                                              '\n'
           '                    "-Wcast-qual",'                                               '\n'
           '                    "-Wctor-dtor-privacy",'                                       '\n'
           '                    "-Wdisabled-optimization",'                                   '\n'
           '                    "-Wformat=2",'                                                '\n'
           '                    "-Winit-self",'                                               '\n'
           '                    "-Wlogical-op",'                                              '\n'
           '                    "-Wmissing-include-dirs",'                                    '\n'
           '                    "-Wnoexcept",'                                                '\n'
           '                    "-Woverloaded-virtual",'                                      '\n'
           '                    "-Wredundant-decls",'                                         '\n'
           '                    "-Wshadow",'                                                  '\n'
           '                    "-Wsign-conversion",'                                         '\n'
           '                    "-Wsign-promo",'                                              '\n'
           '                    "-Wstrict-null-sentinel",'                                    '\n'
           '                    "-Wstrict-overflow=5",'                                       '\n'
           '                    "-Wswitch-default",'                                          '\n'
           '                    "-Wundef",'                                                   '\n'
           ''                                                                                 '\n'
           '                    "-c", "-o",'                                                  '\n'
           '                "-d:", "' + _epath + '/Build/Linux/LynxDebug",'                   '\n'
           '                "-r:", "' + _epath + '/Build/Linux/LynxRelease",'                 '\n'
           '            ],'                                                                   '\n'
           '            "problemMatcher": [ "$gcc" ],'                                        '\n'
           '            "options": { "cwd": "${workspaceFolder}" },'                          '\n'
           '            "group": { "kind": "build", "isDefault": true }'                      '\n'
           '        },'                                                                       '\n'
           '        {'                                                                        '\n'
           '            "type": "shell",'                                                     '\n'
           '            "label": "Linux  |  Debug  |  Build Application\",'                   '\n'
           '            "command": "' + _epath + '/Tools/Build/lynxg++",'                     '\n'
           '            "args": [ "--mode=ld",'                                               '\n'
           '                //Your build'                                                     '\n'
           '            ],'                                                                   '\n'
           '            "problemMatcher": [ "$gcc" ],'                                        '\n'
           '            "options": { "cwd": "${workspaceFolder}" },'                          '\n'
           '            "group": { "kind": "build", "isDefault": true }'                      '\n'
           '        },'                                                                       '\n'
           '        {'                                                                        '\n'
           '            "type": "shell",'                                                     '\n'
           '            "label": " > Switch to Windows",'                                     '\n'
           '            "command": "python3",'                                                '\n'
           '            "args": ['                                                            '\n'
           '                "' + _epath + '/Tools/Setup/SetPlatform.py",'                     '\n'
           '                "w",'                                                             '\n'
           '                "ignored_argument" //FIXME'                                       '\n'
           '            ],'                                                                   '\n'
           '            "problemMatcher": [ ],'                                               '\n'
           '            "options": { "cwd": "${workspaceFolder}" },'                          '\n'
           '            "group": { "kind": "build", "isDefault": true }'                      '\n'
           '        },'                                                                       '\n'
           '        {'                                                                        '\n'
           '            "type": "shell",'                                                     '\n'
           '            "label": " > Switch to Release",'                                     '\n'
           '            "command": "python3",'                                                '\n'
           '            "args": ['                                                            '\n'
           '                "' + _epath + '/Tools/Setup/SetType.py",'                         '\n'
           '                "r",'                                                             '\n'
           '                "ignored_argument" //FIXME'                                       '\n'
           '            ],'                                                                   '\n'
           '            "problemMatcher": [ ],'                                               '\n'
           '            "options": { "cwd": "${workspaceFolder}" },'                          '\n'
           '            "group": { "kind": "build", "isDefault": true }'                      '\n'
           '        }'                                                                        '\n'
           '    ]'                                                                            '\n'
           '}'                                                                                '\n'
        )
    # SetPlatform.run('l')
    # SetType.run('d')
