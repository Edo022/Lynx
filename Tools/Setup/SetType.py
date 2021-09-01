import os, sys
import UpdateBuild

#TODO make this script an executable

def run(x):
	with open('./.engine/type', 'w') as btf:
		btf.write(x)

	thisDir = os.path.abspath('.')
	UpdateBuild.run(thisDir)

if(len(sys.argv) == 3): run(sys.argv[1])