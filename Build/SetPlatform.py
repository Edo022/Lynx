import sys
import os.path
thisdir = os.path.dirname(os.path.abspath(__file__))


bpf = open(thisdir + "/Platform.py", "w")
bpf.write("Platform = \"" + sys.argv[1] + "\"")
bpf.close()
os.system("python3 " + thisdir + "/UpdateBuild.py")
