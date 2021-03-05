import os
import Build.UpdateBuild as UpdateBuild #FIXME fix import when calling the script from vscode

def run(x):
	with open('./.engine/platform', 'w') as bpf:
		bpf.write(x)

	thisDir = os.path.abspath('.')
	UpdateBuild.run(thisDir)