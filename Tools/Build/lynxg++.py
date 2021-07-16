import re, sys, os, subprocess

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
r:re.Match = None
i:int = 0
while i < len(cmd):
    _r = re.match(r'^\-mode=[lw][rds]$', cmd[i])
    if _r != None:
        r = _r
        del cmd[i]
    elif cmd[i] == '--build-engine':
        cd = 'e'
        del cmd[i]
    else:
        i += 1

if r != None:
    pf = r.group(0)[-2]
    tp = r.group(0)[-1]
else:
    print('Error:\n"-mode=(l|w)(r|d|s)" options is required\n')
    exit()




#Pick options based on platform and type
cmdp = []                                       #Parsed command
i = 0                                           #Option index
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




#Build GLSLC command
cmdsh = []
i = 0
while i < len(cmdp):
    r = re.match(r'^([^\-](.*))\.comp$', cmdp[i])
    if r != None:
        cmdp[i] = r.group(1) + '.comp;' + r.group(1) + '.spv'

    r = re.match(r'^([^\-](.*))\.comp;(.*)\.spv$', cmdp[i])
    if r != None:
        iname = r.group(1)
        oname = r.group(3)
        cmdsh += [
            [
                enginePath + '/Deps/Linux/Vulkan-1.2.170.0/x86_64/bin/glslangValidator',
                '-V', iname + '.comp', '-o', oname + '.spv'
            ],[
                enginePath + '/Deps/Linux/Vulkan-1.2.170.0/x86_64/bin/spirv-val',
                oname + '.spv'
            ],[
                'python3',
                enginePath + '/Lynx/shaders/GlslToCpp.py',
                iname + '.comp',
            ]
        ]
        del(cmdp[i])
    else:
        i += 1




#Build G++ command
vkdep:str = enginePath + '/Deps/' + getpf() + '/Vulkan-1.2.170.0/x86_64'
gwdep:str = enginePath + '/Deps/Shared/GLFW'

cmdg = ['g++', '-std=c++20', '-pthread']                        #Base options
cmdg += ['-include', 'Lynx/Core/VkDef.hpp']                     #Include forced vulkan macros
cmdg += ['-include', 'Lynx/Lynx_config.hpp']                    #Include engine configuration macros
if tp == 'd': cmdg += ['-DLNX_DEBUG', '-rdynamic']              #Activate Lynx debug checks when in debug mode

if cd == 'u': cmdg += [                                         #When building user application
    '-DenginePath="' + enginePath + '"',                        #Define engine path function #FIXME
    enginePath + '/Lynx/getEnginePath.cpp',                         #Add engine path definition  #FIXME
    enginePath + '/Lynx/Core/Env.cpp',                              #Add runtime environment variables
    enginePath + '/Build/' + getpf() + '/Lynx' + gettp()            #Add engine binaries
]

cmdg += cmdp + [                                                #Copy parsed G++ options
    '-I' + vkdep + '/include',                                      #Add Vulkan include path
    '-I' + gwdep + '/include',                                      #Add GLFW include path
    '-I' + gwdep + '/deps',                                         #Add GLFW dependencies include path
    '-I' + enginePath                                               #Add Lynx include path
]

if cd == 'u': cmdg += [                                         #When building user application
    '-I' + '.',                                                     #Add workspace include path
    '-L' + vkdep + '/lib',                                          #Add Vulkan library path
    '-L' + enginePath + '/Deps/Shared/GLFWBuild/src',               #Add GLFW library path #FIXME USE DIFFERENT BINARIES FOR DEBUG AND RELEASE
    '-ldl', '-lrt', '-lXrandr', '-lXi', '-lXcursor', '-lXinerama', '-lX11', #Link dependencies
    '-lvulkan', '-Bstatic', '-lglfw3'                               #Link Vulkan dynamically and GLFW statically
]







#Run GLSLC commands
if len(cmdsh) > 0:
    print('\n' + ('-' * os.get_terminal_size().columns))
    print('\n\n' '\033[1m' 'COMPILING SHADERS')
    for sh in cmdsh:
        print('\033[1m' + (' '.join(sh)) + '\033[0m')
    for sh in cmdsh:
        subprocess.run(sh)
    print('\n')


#Run G++ command
if len(cmdg) > 1:
    print('\n' + ('-' * os.get_terminal_size().columns))
    print('\n\n' '\033[1m' 'COMPILING TRANSLATION UNITS')
    print(' '.join(cmdg) + '\n' '\033[0m')
    subprocess.run(cmdg)