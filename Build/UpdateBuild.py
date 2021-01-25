import os
import EngineDeps
import ProjectPath
import Platform as pf
import Type as tp





f = open(ProjectPath.ProjectPath + "/.vscode/tasks.json", "w")
f.write(
    #Active build
"""{
    "tasks": [
        {
            "type": "shell",
            "label": "Build LuxEngine  |  """ +  ("Linux" if pf.Platform == "l" else "Windows") + "  |  " + ("Debug" if tp.Type == "d" else "Release") + """\",
            "command": "",
            "args": [
                "/usr/bin/g++",\n""" + \
                EngineDeps.getEngineDeps() + """
            ],
            "options": { "cwd": "${workspaceFolder}" },
            "problemMatcher": [ "$gcc" ],
            "group": { "kind": "build", "isDefault": true }
        },
        {
            "type": "shell",
            "label": " > Switch to """ + ("Windows" if pf.Platform == "l" else "Linux") + """\",
            "command": "python3",
            "args": [
                "./Build/SetPlatform.py",
                \"""" + ("w" if pf.Platform == "l" else "l") + """\",
            ],
            "problemMatcher": [ ],
            "group": { "kind": "build", "isDefault": true }
        },
        {
            "type": "shell",
            "label": " > Switch to """ + ("Release" if tp.Type == "d" else "Debug") + """\",
            "command": "python3",
            "args": [
                "./Build/SetType.py",
                \"""" + ("r" if tp.Type == "d" else "d") + """\"
            ],
            "problemMatcher": [ ],
            "group": { "kind": "build", "isDefault": true }
        }
    ]
}\n""")
f.close()


os.system("rm Build/__pycache__ -r")