# from Lynx.Setup.BuildOptions import enginePath
import sys, os
import SetPlatform, SetType
thisdir = os.path.dirname(os.path.abspath(__file__))


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

    enginePath = os.path.relpath(thisdir + '/..', pdir)
    with open('./.engine/enginePath', 'w') as pf:
        pf.write(enginePath)




    with open(pdir + '/.vscode/tasks.json', 'w') as uf:
        uf.write(
           '{ '                                                                                                             '\n'
           '    "version": "2.0.0",'                                                                                        '\n'
           '    "tasks": ['                                                                                                 '\n'
           '        {'                                                                                                      '\n'
           '            "type": "shell",'                                                                                   '\n'
           '            "label": "Linux  |  Debug  |  Build Lynx",'                                                    '\n'
           '            "command": "' + enginePath + '/Build/lynxg++",'                                                      '\n'
           '            "args": [ "-mode=ld",'                                                                              '\n'
           '                "Lynx/Lynx/Lynx_build.cpp",'                                                     '\n'
           '                "-pipe", "-mavx", "--build-engine",'                                                            '\n'
           ''                                                                                                               '\n'
           '                "-d[",'                                                                                         '\n'
           '                    "-p", "-g3", "-ggdb3", "-rdynamic", "-fverbose-asm", "-DLUX_DEBUG",'                        '\n'
           '                    "-Og", "-fno-elide-constructors", "-fno-inline-small-functions", "-fno-inline",'            '\n'
           '                "]",'                                                                                           '\n'
           '                "-r[", "-g0", "-Ofast", "-frename-registers", "-funroll-loops", "]",'                           '\n'
           ''                                                                                                               '\n'
           '                "-Wall",'                                                                                       '\n'
           '                "-Wclobbered", "-Wcast-function-type", "-Wdeprecated-copy", "-Wempty-body", "-Wignored-qualifiers", "-Wimplicit-fallthrough=3",'    '\n'
           '                "-Wstring-compare", "-Wredundant-move", "-Wtype-limits", "-Wuninitialized", "-Wshift-negative-value", "-Wunused-but-set-parameter",''\n'
           '                "-Wcast-align", "-Wcast-qual", "-Wctor-dtor-privacy", "-Wdisabled-optimization", "-Wformat=2", "-Winit-self", "-Wlogical-op", "-Wmissing-include-dirs",' '\n'
           '                "-Wnoexcept", "-Woverloaded-virtual", "-Wredundant-decls", "-Wshadow", "-Wsign-conversion", "-Wsign-promo",'                        '\n'
           '                "-Wstrict-null-sentinel", "-Wstrict-overflow=5", "-Wswitch-default", "-Wundef",'                '\n'
           ''                                                                                                               '\n'
           '                "-c", "-o",'                                                                                    '\n'
           '                "-d[Lynx/Build/Linux/LynxDebug]",'                                                    '\n'
           '                "-r[Lynx/Build/Linux/LynxRelease]",'                                                  '\n'
           '            ],'                                                                                                 '\n'
           '            "problemMatcher": [ "$gcc" ],'                                                                      '\n'
           '            "options": { "cwd": "${workspaceFolder}" },'                                                        '\n'
           '            "group": { "kind": "build", "isDefault": true }'                                                    '\n'
           '        },'                                                                                                     '\n'
           '        {'                                                                                                      '\n'
           '            "type": "shell",'                                                                                   '\n'
           '            "label": "Linux  |  Debug  |  Build Application\",'                                                 '\n'
           '            "command": "' + enginePath + '/Build/lynxg++",'                                                      '\n'
           '            "args": [ "-mode=ld",'                                                                              '\n'
           '                //Your build'                                                                                   '\n'
           '            ],'                                                                                                 '\n'
           '            "problemMatcher": [ "$gcc" ],'                                                                      '\n'
           '            "options": { "cwd": "${workspaceFolder}" },'                                                        '\n'
           '            "group": { "kind": "build", "isDefault": true }'                                                    '\n'
           '        },'                                                                                                     '\n'
           '        {'                                                                                                      '\n'
           '            "type": "shell",'                                                                                   '\n'
           '            "label": " > Switch to Windows",'                                                                   '\n'
           '            "command": "python3",'                                                                              '\n'
           '            "args": ['                                                                                          '\n'
           '                "' + enginePath + '/Setup/SetPlatform.py",'                                                     '\n'
           '                "w",'                                                                                           '\n'
           '            ],'                                                                                                 '\n'
           '            "problemMatcher": [ ],'                                                                             '\n'
           '            "options": { "cwd": "${workspaceFolder}" },'                                                        '\n'
           '            "group": { "kind": "build", "isDefault": true }'                                                    '\n'
           '        },'                                                                                                     '\n'
           '        {'                                                                                                      '\n'
           '            "type": "shell",'                                                                                   '\n'
           '            "label": " > Switch to Release",'                                                                   '\n'
           '            "command": "python3",'                                                                              '\n'
           '            "args": ['                                                                                          '\n'
           '                "' + enginePath + '/Setup/SetType.py",'                                                         '\n'
           '                "r"'                                                                                            '\n'
           '            ],'                                                                                                 '\n'
           '            "problemMatcher": [ ],'                                                                             '\n'
           '            "options": { "cwd": "${workspaceFolder}" },'                                                        '\n'
           '            "group": { "kind": "build", "isDefault": true }'                                                    '\n'
           '        }'                                                                                                      '\n'
           '    ]'                                                                                                          '\n'
           '}'                                                                                                              '\n'
        )
    # SetPlatform.run('l')
    # SetType.run('d')
