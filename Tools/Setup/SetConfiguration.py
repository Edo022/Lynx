import os, sys
import UpdateBuild

#TODO make this script an executable
#TODO remove extra argument

def run(x):
	with open('./.engine/.cf', 'w') as f:
		f.write(x)

	thisDir = os.path.abspath('.')
	UpdateBuild.run(thisDir)

if(len(sys.argv) == 3): run(sys.argv[1])