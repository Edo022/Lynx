import os
import sys

btf = open("Build/Type.py", "w")
btf.write("Type = \"" + sys.argv[1] + "\"")
btf.close()
os.system("python3 Build/UpdateBuild.py")
