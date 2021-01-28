import sys
import os
thisdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(thisdir + '/..')


btf = open(thisdir + "/BuildOptions/Data/Type.py", "w")
btf.write("Type = \"" + sys.argv[1] + "\"")
btf.close()

os.system("python3 -m Build.UpdateBuild")
