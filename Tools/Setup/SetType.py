import os, sys
import UpdateBuild


def run(x):
	with open('./.engine/type', 'w') as btf:
		btf.write(x)

	thisDir = os.path.abspath('.')
	UpdateBuild.run(thisDir)

if(len(sys.argv) == 2): run(sys.argv[1])