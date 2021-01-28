import sys
import os
thisdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(thisdir + '/..')


bpf = open(thisdir + "/BuildOptions/Data/Platform.py", "w")
bpf.write("Platform = \"" + sys.argv[1] + "\"")
bpf.close()

os.system("python3 -m Build.UpdateBuild")
