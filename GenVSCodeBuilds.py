import sys
import os.path
thisdir = os.path.dirname(os.path.abspath(__file__))

if len(sys.argv) != 2:
    exit(-1)

pdir = sys.argv[1]
if not os.path.exists(pdir):
    print("Unknown path")
    exit(-2)
if not os.path.exists(pdir + "/.vscode"):
    print("Unable to find the .vscode folder in the provided path")
    exit(-3)




pf = open(thisdir + "/Build/ProjectPath.py", "w")
pf.write("ProjectPath = \"" + os.path.relpath(thisdir, pdir) + "\"")
pf.close()

os.system("python3 " + thisdir + "/Build/SetPlatform.py l")
os.system("python3 " + thisdir + "/Build/SetType.py d")
