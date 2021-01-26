import sys
import os.path
thisdir = os.path.dirname(os.path.abspath(__file__))


btf = open(thisdir + "/Type.py", "w")
btf.write("Type = \"" + sys.argv[1] + "\"")
btf.close()
os.system("python3 "  + thisdir + "/UpdateBuild.py")
