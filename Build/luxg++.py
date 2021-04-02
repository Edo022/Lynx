import re, sys, os, shlex







enginePath:str
with open('./.engine/enginePath', 'r') as f:
    enginePath = f.read()




def compileShader(name:str):
    os.system(
        enginePath + '/deps/Linux/Vulkan-1.2.170.0/x86_64/bin/glslc '    +
        enginePath + '/LuxEngine/Contents/shaders/' + name + '.comp -o ' +
        enginePath + '/LuxEngine/Contents/shaders/' + name + '.spv'
    )
    os.system(
        'python3 ' +
        enginePath + '/LuxEngine/Contents/shaders/GlslToCpp.py ' +
        enginePath + '/LuxEngine/Contents/shaders/' + name + '.comp'
    )






cmd = [] #Raw luxg++ command
ccmd:str    #g++ command
pf:str      #Target platform. 'l'(Linux) or 'w'(Windows)
tp:str      #Target type.     'd'(Debug),   'r'(Release) or 's'(Shipping)
cmd = shlex.split(sys.argv[1])



#Find mode
r:re.Match = None; ir:int
for i, o in enumerate(cmd):
    _r = re.match(r'^\-mode=[lw][rds]$', o)
    if _r != None:
        r = _r
        ir = i

if r != None:
    del cmd[ir]
    pf = r.group(0)[-2]
    tp = r.group(0)[-1]
else:
    print('Error:\n"-mode[l|w][r|d|s]" options is required\n')
    exit()




#Pick options based on platform and type
cmdp = []    #Parsed command
for o in cmd:
    r = re.match(r'^\-([lwrd])\[(.*)\]$', o)
    if r != None:
        if r.group(1) in [tp, pf]:
            cmdp.append(r.group(2))
    else:
        cmdp.append(o)



# compileShader('Border2D')
# compileShader('Line2D')
# compileShader('FloatToIntBuffer')
print(' '.join(cmdp))

cmd:str = (
    'g++'
    ' -p'
    ' -pthread'
)




#python3.9 ./LuxEngine/Build/luxg++.py $'-mode=ld -d[\'-O0 -a\'] -r[-O3] -pthread'