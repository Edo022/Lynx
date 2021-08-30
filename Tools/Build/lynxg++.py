import re, sys, os, subprocess
import GlslToCpp
import os, shlex
import argparse as ap







p = ap.ArgumentParser(prog = 'lynxg++', add_help = False, usage = 'lynxg++ -mode=<mode> -<selector>: <g++options...> <GLSLfiles...>')

p.add_argument('-h', '--help',   action = 'store_true', dest = 'h')
p.add_argument('-m', '--mode',   action = 'store',      dest = 'm', choices = ['wd', 'wr', 'ld', 'lr'], )
p.add_argument('-e', '--engine', action = 'store_true', dest = 'e')

p.add_argument('-a:', '--always:',  action = 'extend', nargs = '+', default = [''], dest = 'a')
p.add_argument('-d:', '--debug:',   action = 'extend', nargs = '+', default = [''], dest = 'd')
p.add_argument('-r:', '--release:', action = 'extend', nargs = '+', default = [''], dest = 'r')
p.add_argument('-w:', '--window:',  action = 'extend', nargs = '+', default = [''], dest = 'w')
p.add_argument('-l:', '--linux:',   action = 'extend', nargs = '+', default = [''], dest = 'l')

p.add_argument('-wd:',             action = 'extend', nargs = '+', default = [''], dest = 'wd')
p.add_argument('-wr:',             action = 'extend', nargs = '+', default = [''], dest = 'wr')
p.add_argument('-ld:',             action = 'extend', nargs = '+', default = [''], dest = 'ld')
p.add_argument('-lr:',             action = 'extend', nargs = '+', default = [''], dest = 'lr')

for i in range(1, len(sys.argv)):
    selectors = list(set(p._option_string_actions.keys()) - set(['-m', '--mode']))
    if not sys.argv[i] in selectors and re.match(r'^((-m)|(--mode))=.*$', sys.argv[i]) == None:
        sys.argv[i] = str(i).zfill(5) + sys.argv[i]




args = p.parse_args()
if args.h:
    print(
        'Usage:'                                                                                                                                        '\n'
        '    lynxg++ -h'                                                                                                                                '\n'
        '    lynxg++ -mode=<mode> -<selector>: <g++options...> <GLSLfiles...>'                                                                          '\n'
        ''                                                                                                                                              '\n'
        'Options:'                                                                                                                                      '\n'
        '    -h  --help         Display this information'                                                                                               '\n'
        '    -m  --mode         Specify target platform and build type. This option is always required  e.g. -m=ld'                                     '\n'
        '                            l = linux, w = windows, d = debug, r = release'                                                                    '\n'
        '    -e  --engine       Build the engine instead of the user application (off by default)'                                                      '\n'
        ''                                                                                                                                              '\n'
        'Selectors allow you to use a single command to build applications for different platforms or build types'                                      '\n'
        '    -a  --always       Always use the options, regardless of platform or type                  e.g. -a"main.cpp"'                              '\n'
        '    -l  --linux        Only use the options when building for Linux                            e.g. -l"-pthread"    e.g. -l"-pthread -Dlinux"' '\n'
        '    -w  --windows      Only use the options when building for Windows                          e.g. -w"-mthread"    e.g. -w"-mthread -Dwin10"' '\n'
        '    -d  --debug        Only use the options when building in Debug mode                        e.g. -d"-Og"         e.g. -d"-Og -g3"'          '\n'
        '    -r  --release      Only use the options when building in Release mode                      e.g. -r"-O3"         e.g. -r"-O3 -g0"'          '\n'
        ''                                                                                                                                              '\n'
        '    Files with extension .comp are treated as GLSL compute shaders'                                                                            '\n'
        '        By default, the .spv has the same name of the .comp and is placed in the same directory'                                               '\n'
        '        A different output file can be specified with the syntax:'                                                                             '\n'
        '        <path/to/inputfile>.comp;<path/to/outputfile>.spv;'                                                                                    '\n'
        '    Any unrecognized option inside a selector is forwarded to g++'                                                                             '\n'
    )
    exit(0)

elif args.m is None:
    print('lynxg++: error: the following arguments are required: -m/--mode')
    exit(1)




#Get engine path
_epath:str
with open('./.engine/enginePath', 'r') as f:
    _epath = f.read()




#Select active arguments
cmd = []
cmd += shlex.split(' '.join(args.a))
cmd += shlex.split(' '.join(args.l if args.m[0] == 'l' else args.w))
cmd += shlex.split(' '.join(args.d if args.m[1] == 'd' else args.r))
cmd += shlex.split(' '.join(
    args.ld if args.m == 'ld' else
    args.lr if args.m == 'lr' else
    args.wd if args.m == 'wd' else
    args.wr
))


#Sort and parse arguments
cmd.sort()
for i in range(0, len(cmd)):
    cmd[i] = cmd[i][5:]


#Set complete names for platform and type
_ptfm = 'Linux' if args.m[0] == 'l' else 'Windows'
_type = 'Debug' if args.m[1] == 'd' else 'Release'




#Build GLSLC command
cmdsh = []
i = 0
for i, arg in enumerate(cmd):
    r = re.match(r'^(.*)\.comp$', arg)
    if r != None:
        arg += ';' + r.group(1) + '.spv'

    r = re.match(r'^(.*)\.comp;(.*)\.spv$', arg)
    if r != None:
        cmdsh += [[r.group(1), r.group(2)]]
        del(cmd[i])




#Build G++ command
vkdep:str = _epath + '/Deps/' + _ptfm + '/Vulkan-1.2.170.0/x86_64'
gwdep:str = _epath + '/Deps/Shared/GLFW'

cmd = ['g++', '-std=c++20', '-pthread'] + cmd                   #Default g++ call, C++20, pthread
cmd += ['-include', 'Lynx/Core/VkDef.hpp']                      #Include forced vulkan macros
cmd += ['-include', 'Lynx/Lynx_config.hpp']                     #Include engine configuration macros
if args.m[1] == 'd': cmd += ['-DLNX_DEBUG', '-rdynamic']        #Activate Lynx debug checks when in debug mode

# cmdg += ['-ffile-prefix-map="' + os.getcwd() + '"="./"']
# #FIXME ^ this doesn't work

if args.e is False: cmd += [                                    #When building user application
    '-DenginePath="' + _epath + '"',                            #Define engine path function #FIXME
    _epath + '/Lynx/getEnginePath.cpp',                         #Add engine path definition  #FIXME
    _epath + '/Lynx/Core/Env.cpp',                              #Add runtime environment variables
    _epath + '/Build/' + _ptfm + '/Lynx' + _type                #Add engine binaries
]

cmd += [                                                        #Copy parsed G++ options
    '-I' + vkdep + '/include',                                      #Add Vulkan include path
    '-I' + gwdep + '/include',                                      #Add GLFW include path
    '-I' + gwdep + '/deps',                                         #Add GLFW dependencies include path
    '-I' + _epath                                               #Add Lynx include path
]

if not args.e: cmd += [                                    #When building user application
    '-I' + '.',                                                     #Add workspace include path
    '-L' + vkdep + '/lib',                                          #Add Vulkan library path
    '-L' + _epath + '/Deps/Shared/GLFWBuild/src',               #Add GLFW library path #FIXME USE DIFFERENT BINARIES FOR DEBUG AND RELEASE
    '-ldl', '-lrt', '-lXrandr', '-lXi', '-lXcursor', '-lXinerama', '-lX11', #Link dependencies
    '-lvulkan', '-Bstatic', '-lglfw3'                               #Link Vulkan dynamically and GLFW statically
]








def runCmd(args):
    try:
        subprocess.check_output(args, universal_newlines=True)
        print('\033[1m' + (' '.join(args)) + '\033[0m')
    except subprocess.CalledProcessError as e:
        print(
            'The task \033[1m"' + ' '.join(args) +
            '"\033[0m exited with return code ' + str(e.returncode) +
            ':\n\033[31m' + str(e.output) + '\033[0m'
        )
        exit(1)



#Run GLSLC commands
if len(cmdsh) > 0:
    print('\n' + ('-' * os.get_terminal_size().columns))
    print('\n\n' '\033[1m' 'COMPILING SHADERS\033[0m')

    print(cmdsh)
    for files in cmdsh:
        print(files)
        print('\n')
        runCmd([_epath + '/Deps/Linux/Vulkan-1.2.170.0/x86_64/bin/spirv-val', files[0] + '.spv'])
        runCmd([_epath + '/Deps/Linux/Vulkan-1.2.170.0/x86_64/bin/glslangValidator', '-V', files[0] + '.comp', '-o', files[1] + '.spv'])
        r = GlslToCpp.run(files[0] + '.comp', _ptfm)
        if r != 0: exit(r)
    print('\n')


#Run G++ command
if len(cmd) > 1:
    print('\n' + ('-' * os.get_terminal_size().columns))
    print('\n\n' '\033[1m' 'COMPILING TRANSLATION UNITS\033[0m')
    print(' '.join(cmd) + '\n' '\033[0m')
    subprocess.run(cmd)