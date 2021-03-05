import os
import Build.UpdateBuild as UpdateBuild

def run(x):
	with open('./.engine/platform', 'w') as bpf:
		bpf.write(x)

	thisDir = os.path.abspath('.')
	UpdateBuild.run(thisDir)