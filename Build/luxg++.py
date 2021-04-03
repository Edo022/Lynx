import re, sys, os

# This script is a G++ wrapper that is called via luxg++ C++ executable wrapper
# Argv[1] contains all the g++ and luxg++ parameters, divided by a '\x02' character
# Any character with an escape sequence or '\x02' from the user input is escaped exacly once,
# in order to pass the user input to Python using the $'<string>' format
#
# Options:
#     -l[<option(s)>]               Activate option(s) only when building for Linux              e.g. -l[-pthread]       e.g. -l['-pthread -Dlinux']
#     -w[<option(s)>]               Activate option(s) only when building for Windows            e.g. -w[-mthread]       e.g. -w['-mthread -Dwin10']
#     -d[<option(s)>]               Activate option(s) only when building in Debug   mode        e.g. -d[-Og]            e.g. -d['-Og -g3']
#     -r[<option(s)>]               Activate option(s) only when building in Release mode        e.g. -r[-O3]            e.g. -r['-O3 -g0']
#     --build-engine                Used to compile the engine and prevent the script from linking its binaries
#     -mode=<platform><type>        Specify target platform and type
#         -mode=ld                      Debug    mode for Linux
#         -mode=lr                      Release  mode for Linux
#         -mode=ls                      Shipping mode for Linux
#         -mode=wd                      Debug    mode for Windows
#         -mode=wr                      Release  mode for Windows
#         -mode=ws                      Shipping mode for Windows
#    <path>.comp                    Compile a glsl shader into an .spv and generate a C++ interface     e.g. shaders/cube.comp
#
#    Anything else is forwarded to G++
#
#
# e.g.
#     ./LuxEngine/Build/luxg++ -mode=ld -d['-O0 -a'] -r[-O3] -pthread
# or
#     python3.9 ./LuxEngine/Build/luxg++.py $'-mode=ld\x02-d[\'-O0 -a\']\x02-r[-O3]\x02-pthread'



enginePath:str
with open('./.engine/enginePath', 'r') as f:
    enginePath = f.read()




def compileShader(name:str):
    if os.system( #FIXME USE DIFFERENT OPTIMIZATION LEVELS FOR DEBUG AND RELEASE #https://man.linuxreviews.org/man1/glslc.1.html
        enginePath + '/deps/Linux/Vulkan-1.2.170.0/x86_64/bin/glslc '    +
        enginePath + '/LuxEngine/Contents/shaders/' + name + '.comp -o ' +
        enginePath + '/LuxEngine/Contents/shaders/' + name + '.spv'
    ) != 0: exit()
    if os.system(
        'python3 ' +
        enginePath + '/LuxEngine/Contents/shaders/GlslToCpp.py ' +
        enginePath + '/LuxEngine/Contents/shaders/' + name + '.comp'
    ) != 0: exit()




cmd = []            #Raw luxg++ command
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
    print('Error:\n"-mode=[l|w][r|d|s]" options is required\n')
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




#Construct g++ command
vkdep:str = enginePath + '/deps/' + getpf() + '/Vulkan-1.2.170.0/x86_64/'
gwdep:str = enginePath + '/deps/Shared/glfw-3.3.3/'
cmdg = (
    'g++' +
    (' -DLUX_DEBUG -rdynamic' if tp == 'd' else '') +       #Activate Lux debug checks when in debug mode
    ' -DGLM_FORCE_RADIANS -DGLM_FORCE_DEPTH_ZERO_TO_ONE' +  #Define vulkan macros
    ((                                                      #When building user application
        ' -DenginePath="\\"' + enginePath + '\\""'              #Define engine path function #FIXME
        ' ' + enginePath + '/LuxEngine/getEnginePath.cpp'       #Add engine path definition  #FIXME
        ' ' + enginePath + '/LuxEngine/Core/Env.cpp'            #Add runtime environment variables
        ' ' + enginePath + '/Build/' + getpf() + '/LuxEngine' + gettp()
    ) if cd == 'u' else '') +                                   # ^ Add engine binaries
    ' ' + ' '.join(cmdp) +                                  #Copy parsed G++ options
    ' -I' + vkdep + 'include'                               #Add Vulkan include path
    ' -I' + gwdep + 'include'                               #Add GLFW include path
    ' -I' + gwdep + 'deps'                                  #Add GLFW dependencies include path
    ' -I' + enginePath +                                    #Add LuxEngine include path
    ((                                                      #When building user application
        ' -I' + '.'                                             #Add workspace include path
        ' -L' + vkdep + 'lib'                                   #Add Vulkan library path
        ' -L' + gwdep + 'build/src'                             #Add GLFW library path #FIXME USE DIFFERENT BINARIES FOR DEBUG AND RELEASE
        '-ldl -lrt -lXrandr -lXi -lXcursor -lXinerama -lX11'    #Link dependencies
        ' -lvulkan -Bstatic -lglfw3'                            #Link Vulkan dynamically and GLFW statically
    ) if cd == 'u' else '')                                     #
)


#Compile shaders
if cd == 'e':
    #FIXME USE PARAMETERS, NOT HARD CODED PATHS
    print('\n\n' '\033[1m' 'COMPILING SHADERS')
    compileShader('Border2D')
    compileShader('Line2D')
    compileShader('FloatToIntBuffer')
    print('\033[0m')


#Run G++ command
print('\n\n' '\033[1m' 'COMPILING CPPs')
print(cmdg + '\n' '\033[0m'); os.system(cmdg)