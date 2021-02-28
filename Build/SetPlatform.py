import sys, os


bpf = open('./.engine/platform', 'w+')
bpf.write(sys.argv[1])
bpf.close()

thisDir = os.path.abspath('.')
os.chdir(os.path.dirname(os.path.abspath(__file__)) + '/..')
os.system('python3 -m Build.UpdateBuild ' + thisDir)
