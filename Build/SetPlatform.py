import os
import sys

bpf = open("Build/Platform.py", "w")
bpf.write("Platform = \"" + sys.argv[1] + "\"")
bpf.close()
os.system("python3 Build/UpdateBuild.py")
