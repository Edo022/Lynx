import os
import EngineDeps
import ProjectPath
from Platform import Platform as pf
from Type import Type as tp

def getPf():
    return "Linux" if pf == "l" else "Windows"
def getTp():
    return "Debug" if tp == "d" else "Release"




f = open(ProjectPath.ProjectPath + "/.vscode/tasks.json", "w")
f.write(
    #Active build
"""{
    "tasks": [
        {
            "type": "shell",
            "label": "Build LuxEngine  |  """ + getPf() + "  |  " + getTp() + """\",
            "command": "",
            "args": [
                "/usr/bin/g++",
                //Source files
		            "-xc++", "LuxEngine/LuxEngineBuild.cpp.build",
		            "Test.cpp",\n""" +\
                    (EngineDeps.getDebugOptions() if tp == "d" else EngineDeps.getReleaseOptions()) + ",\n" + """
                    "-std=c++2a", "-mavx", "-pipe", "-Wall",""" +\
                    EngineDeps.getEngineDeps() + ',\n' + """
                //Output
					"-o", "./Build/""" + getPf() + '/LuxEngine' + getTp() + """\"
            ],
            "options": { "cwd": "${workspaceFolder}" },
            "problemMatcher": [ "$gcc" ],
            "group": { "kind": "build", "isDefault": true }
        },
        {
            "type": "shell",
            "label": " > Switch to """ + ("Windows" if pf == "l" else "Linux") + """\",
            "command": "python3",
            "args": [
                "./Build/SetPlatform.py",
                \"""" + ("w" if pf == "l" else "l") + """\",
            ],
            "problemMatcher": [ ],
            "group": { "kind": "build", "isDefault": true }
        },
        {
            "type": "shell",
            "label": " > Switch to """ + ("Release" if tp == "d" else "Debug") + """\",
            "command": "python3",
            "args": [
                "./Build/SetType.py",
                \"""" + ("r" if tp == "d" else "d") + """\"
            ],
            "problemMatcher": [ ],
            "group": { "kind": "build", "isDefault": true }
        }
    ]
}\n""")
f.close()


os.system("rm Build/__pycache__ -r")