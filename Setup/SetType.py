import os
import UpdateBuild 

def run(x):
	with open('./.engine/type', 'w') as btf:
		btf.write(x)

	thisDir = os.path.abspath('.')
	UpdateBuild.run(thisDir)
