# from LuxEngine.Setup.BuildOptions import enginePath
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
           '            "label": "Linux  |  Debug  |  Build LuxEngine",'                                                    '\n'
           '            "command": "' + enginePath + '/Build/luxg++",'                                                      '\n'
           '            "args": [ "mode=ld",'                                                                               '\n'
           '                "LuxEngine/LuxEngine/LuxEngine_build.cpp",'                                                     '\n'
           '                "-pipe", "-std=c++2a", "-mavx", "-pthread", "--build-engine",'                                  '\n'
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
           '                "-d[LuxEngine/Build/Linux/LuxEngineDebug]",'                                                    '\n'
           '                "-r[LuxEngine/Build/Linux/LuxEngineRelease]",'                                                  '\n'
           '            ],'                                                                                                 '\n'
           '            "problemMatcher": [ "$gcc" ],'                                                                      '\n'
           '            "options": { "cwd": "${workspaceFolder}" },'                                                        '\n'
           '            "group": { "kind": "build", "isDefault": true }'                                                    '\n'
           '        },'                                                                                                     '\n'
           '        {'                                                                                                      '\n'
           '            "type": "shell",'                                                                                   '\n'
           '            "label": "Linux  |  Debug  |  Build Application\",'                                                 '\n'
           '            "command": "' + enginePath + '/Build/luxg++",'                                                      '\n'
           '            "args": ['                                                                                          '\n'
           '                "-std=c++2a", "-d[ggdb3]", "-d[g3]", "-d[O0]", "-r[g0]", "-r[O3]", "main.cpp", "-o", "out",'    '\n'
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
