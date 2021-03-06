import os, sys
import UpdateBuild


def run(x):
	with open('./.engine/platform', 'w') as bpf:
		bpf.write(x)

	thisDir = os.path.abspath('.')
	UpdateBuild.run(thisDir)

if(len(sys.argv) == 2): run(sys.argv[1])