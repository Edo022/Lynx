import os, sys


# Get paths
if not os.path.exists('./.engine'): raise Exception('.engine directory not found')
with open('./.engine/.Aabs', 'r') as f: Aabs = f.read()
with open('./.engine/.Eabs', 'r') as f: Eabs = f.read()
with open('./.engine/.EtoA', 'r') as f: EtoA = f.read()
with open('./.engine/.AtoE', 'r') as f: AtoE = f.read()
