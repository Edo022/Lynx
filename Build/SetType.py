import sys, os


btf = open('./.engine/type', 'w+')
btf.write(sys.argv[1])
btf.close()

thisDir = os.path.abspath('.')
os.chdir(os.path.dirname(os.path.abspath(__file__)) + '/..')
os.system('python3 -m Build.UpdateBuild ' + thisDir)
