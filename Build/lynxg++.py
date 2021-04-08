import re, sys, os

#FIXME fis shipping build
# This script is a G++ wrapper that is called via lynxg++ C++ executable wrapper
# Argv[1] contains all the g++ and lynxg++ parameters, divided by a '\x02' character
# Any character with an escape sequence or '\x02' from the user input is escaped exacly once,
# in order to pass the user input to Python using the $'<string>' format
#
# e.g.
#     lynxg++ -mode=ld -d[ -O0 -a ] -r[-O3] -pthread
# >>
#     python3.9 /lynxg++.py $'-mode=ld\x02-d[\x02-O0 -a\x02]\x02-r[-O3]\x02-pthread'



enginePath:str
with open('./.engine/enginePath', 'r') as f:
    enginePath = f.read()




def compileShader(name:str):
    if os.system( #FIXME USE DIFFERENT OPTIMIZATION LEVELS FOR DEBUG AND RELEASE #https://man.linuxreviews.org/man1/glslc.1.html
        enginePath + '/deps/Linux/Vulkan-1.2.170.0/x86_64/bin/glslc '    +
        enginePath + '/Lynx/shaders/' + name + '.comp -o ' +
        enginePath + '/Lynx/shaders/' + name + '.spv'
    ) != 0: exit()
    if os.system(
        'python3 ' +
        enginePath + '/Lynx/shaders/GlslToCpp.py ' +
        enginePath + '/Lynx/shaders/' + name + '.comp'
    ) != 0: exit()




cmd = []            #Raw lynxg++ command
pf:str              #Target platform. 'l'(Linux) or 'w'(Windows)
tp:str              #Target type.     'd'(Debug),   'r'(Release) or 's'(Shipping)
cd:str = 'u'        #'e'(Engine) or 'a'(User application)
cmd = sys.argv[1].split('\x02')

def gettp() -> str:
    return 'Debug' if tp == 'd' else ('Release' if tp == 'r' else 'Shipping')

def getpf() -> str:
    return 'Linux' if pf == 'l' else 'Windows'




#Find mode
r:re.Match = None; ir:int
for i, o in enumerate(cmd):
    _r = re.match(r'^\-mode=[lw][rds]$', o)
    if _r != None:
        r = _r
        ir = i
    elif o == '--build-engine':
        cd = 'e'
        del cmd[i]


if r != None:
    del cmd[ir]
    pf = r.group(0)[-2]
    tp = r.group(0)[-1]
else:
    print('Error:\n"-mode=(l|w)(r|d|s)" options is required\n')
    exit()




#Pick options based on platform and type
cmdp = []       #Parsed command
i = 0           #Option index
while i < len(cmd):                             #For each command option
    r = re.match(r'^\-([lwrd])\[(.*)\]$', cmd[i])   #
    if r != None:                                   #If -<c>[<option>]
        if r.group(1) in [tp, pf]:                      #If c matches -mode
            cmdp.append(r.group(2))                         #Append <option>
    else:                                           #Else
        r = re.match(r'^\-([lwrd])\[$', cmd[i])         #
        if r != None:                                   #If -<c>[ <options...> ]
            c = r.group(1)                                  #Save c
            i += 1                                          #Skip -<c>[
            while cmd[i] != ']':                            #For each <options...>
                if c in [tp, pf]:                               #If c matches -mode
                    cmdp.append(cmd[i])                             #Append <options...>[i]
                i += 1                                          #Update index
        else:                                           #Else
            cmdp.append(cmd[i])                             #Forward option to g++
    i += 1                                          #Update index




#Construct g++ command
vkdep:str = enginePath + '/deps/' + getpf() + '/Vulkan-1.2.170.0/x86_64/'
gwdep:str = enginePath + '/deps/Shared/GLFW/'
cmdg = (
    'g++ -std=c++2a -pthread' +
    (' -DLNX_DEBUG -rdynamic' if tp == 'd' else '') +       #Activate Lnx debug checks when in debug mode
    ' -DGLM_FORCE_RADIANS -DGLM_FORCE_DEPTH_ZERO_TO_ONE' +  #Define vulkan macros
    ((                                                      #When building user application
        ' -DenginePath="\\"' + enginePath + '\\""'              #Define engine path function #FIXME
        ' ' + enginePath + '/Lynx/getEnginePath.cpp'       #Add engine path definition  #FIXME
        ' ' + enginePath + '/Lynx/Core/Env.cpp'            #Add runtime environment variables
        ' ' + enginePath + '/Build/' + getpf() + '/Lynx' + gettp()
    ) if cd == 'u' else '') +                                   # ^ Add engine binaries
    ' ' + ' '.join(cmdp) +                                  #Copy parsed G++ options
    ' -I' + vkdep + 'include'                               #Add Vulkan include path
    ' -I' + gwdep + 'include'                               #Add GLFW include path
    ' -I' + gwdep + 'deps'                                  #Add GLFW dependencies include path
    ' -I' + enginePath +                                    #Add Lynx include path
    ((                                                      #When building user application
        ' -I' + '.'                                             #Add workspace include path
        ' -L' + vkdep + 'lib'                                   #Add Vulkan library path
        ' -L' + enginePath + '/deps/Shared/GLFWBuild/src'                             #Add GLFW library path #FIXME USE DIFFERENT BINARIES FOR DEBUG AND RELEASE
        ' -ldl -lrt -lXrandr -lXi -lXcursor -lXinerama -lX11'   #Link dependencies
        ' -lvulkan -Bstatic -lglfw3'                            #Link Vulkan dynamically and GLFW statically
    ) if cd == 'u' else '')                                     #
)


#Compile shaders
if cd == 'e':
    #FIXME USE PARAMETERS, NOT HARD CODED PATHS
    print('\n\n' '\033[1m' 'COMPILING SHADERS')
    compileShader('Border2')
    compileShader('Line2')
    compileShader('FloatToIntBuffer')
    print('\033[0m')


#Run G++ command
print('\n\n' '\033[1m' 'COMPILING CPPs')
print(cmdg + '\n' '\033[0m'); os.system(cmdg)