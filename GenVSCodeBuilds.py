import sys
import os.path


if len(sys.argv) != 2:
    exit(-1)

cwd = sys.argv[1]
if not os.path.exists(cwd):
    print("Unknown path")
    exit(-2)
if not os.path.exists(cwd + "/.vscode"):
    print("Unable to find the .vscode folder in the provided path")
    exit(-3)




pf = open("Build/ProjectPath.py", "w")
pf.write("ProjectPath = \"" + cwd + "\"")
pf.close()

os.system("python3 Build/SetPlatform.py l")
os.system("python3 Build/SetType.py d")
