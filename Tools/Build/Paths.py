import os, sys


# Get paths
if not os.path.exists('./.engine'): raise Exception('.engine directory not found')
with open('./.engine/.setup/.Aabs', 'r') as f: Aabs = f.read()
with open('./.engine/.setup/.Eabs', 'r') as f: Eabs = f.read()
with open('./.engine/.setup/.EtoA', 'r') as f: EtoA = f.read()
with open('./.engine/.setup/.AtoE', 'r') as f: AtoE = f.read()
