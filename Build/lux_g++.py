import os
import sys
import re
import BuildOptions
from Platform import Platform as pf
from Type import Type as tp




def parse(p) :
    if(re.match('^\\-(d|r|l|w)\\[.*\\]$', p) != None):
        if(p[1] in [pf, tp]): return ' -' + p[3:-1]
        else: return ''
    elif(re.match('^\\-(((d|r)|(l|w))|((l|w)|(d|r)))\\[.*\\]~', p) != None):
        if(p[1] in [pf, tp] and p[2] in [pf, tp]): return ' -' + p[4:-1]
        else: return ''
    else: return ' ' + p



s = 'g++' + (' -DLUX_DEBUG' if tp == 'd' else '')
for p in sys.argv[1:]:
    s += parse(p)

s += ' -pthread ' + BuildOptions.getAppDeps()
print(s + '\n')
os.system(s)




thisdir = os.path.dirname(os.path.abspath(__file__))
os.system("rm " + thisdir + "/__pycache__ -r")