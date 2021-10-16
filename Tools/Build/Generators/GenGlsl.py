import os, sys
sys.path.append(os.path.dirname(os.path.abspath(__file__)) + '/..')
import Utils



def run(vSrc:str, vOut:str):
    with open(vSrc, 'r') as srcFile, open(vOut, 'w') as outFile:
        outFile.write(srcFile.read())






if len(sys.argv) != 3: raise Exception('GenInterfaces: Wrong number of arguments')
sys.exit(run(sys.argv[1], sys.argv[2]))
