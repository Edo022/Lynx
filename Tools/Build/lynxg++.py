import re, sys, os, subprocess, argparse as ap
from argparse import Namespace as ns
import shlex, glob
import GlslToCpp
#python3.9 -m py_compile lynxg++.py && python3.9 -m py_compile GlslToCpp.py && { python3 -m PyInstaller -F --clean ./lynxg++.py; cp ./dist/lynxg++ ./; rm -r ./dist; rm ./build -r; rm ./lynxg++.spec; }

#TODO check if the user build actually has arguments


def run(argv:list):
    p = ap.ArgumentParser(prog = 'lynxg++', add_help = False, usage = 'lynxg++ -m=<mode> [<options...>] -<selector>: <g++ arguments...> <GLSL files...>')

    p.add_argument('-h', '--help',      action = 'store_true', dest = 'h')
    p.add_argument('--version',         action = 'store_true', dest = 'version')
    p.add_argument('-m', '--mode',      action = 'store',      dest = 'm', choices = ['wd', 'wr', 'ld', 'lr'])
    p.add_argument('-f', '--file',      action = 'store',      dest = 'f')
    p.add_argument('-v', '--verbosity', action = 'store',      dest = 'v', choices = [0, 1, 2, 3], default = 2, type = int)

    p.add_argument('-a:', '--always:',  action = 'extend', nargs = '+', default = [], dest = 'a')
    p.add_argument('-d:', '--debug:',   action = 'extend', nargs = '+', default = [], dest = 'd')
    p.add_argument('-r:', '--release:', action = 'extend', nargs = '+', default = [], dest = 'r')
    p.add_argument('-w:', '--window:',  action = 'extend', nargs = '+', default = [], dest = 'w')
    p.add_argument('-l:', '--linux:',   action = 'extend', nargs = '+', default = [], dest = 'l')

    p.add_argument('-wd:',              action = 'extend', nargs = '+', default = [], dest = 'wd')
    p.add_argument('-wr:',              action = 'extend', nargs = '+', default = [], dest = 'wr')
    p.add_argument('-ld:',              action = 'extend', nargs = '+', default = [], dest = 'ld')
    p.add_argument('-lr:',              action = 'extend', nargs = '+', default = [], dest = 'lr')



    # Enumerate g++/glslc arguments
    selectors = list(set(p._option_string_actions.keys()) - set(['-m', '--mode', '-v', '--verbosity']))
    for i in range(1, len(argv)):
        if(
            not argv[i] in selectors and
            re.match(r'^((-m)|(--mode))=.*$',      argv[i]) == None and
            re.match(r'^((-v)|(--verbosity))=.*$', argv[i]) == None and
            re.match(r'^((-f)|(--file))=.*$',      argv[i]) == None
        ):
            argv[i] = str(i).zfill(5) + argv[i]


    #FIXME print an error if there are no selectors before the first unknown option

    #TODO add automatic cleanup before compilation
    #TODO replace hard coded paths and use configuration file

    #TODO add verbosity option. default: 1
    #TODO -v=0: only show errors
    #     -v=1: + actual gcc command
    #           + build stage dividers,
    #TODO       + number of compiled shaders
    #TODO       + [total progress bar]
    #TODO       + build outcome
    #     -v=2: + "compiling shader ..." for each shader
    #TODO       + [per-stage progress bar]
    #     -v=3: + actual spirv-val and glslangValidator commands and generated .cpp and .hpp for each shader
    #TODO       + [per-shader & per-file progress bar (multiple bars if multithreaded)]

    #TODO add command line build progress bar in the last line
    #TODO add argument to disable progress bar output


    args = p.parse_args(argv)
    if args.h:
        print(
            'Usage:'                                                                                                                                            '\n'
            '    lynxg++ --help'                                                                                                                                '\n'
            '    lynxg++ --version'                                                                                                                             '\n'
            '    lynxg++ -m=<mode> [<options...>] -<selector>: <g++ arguments...> <GLSL files...>'                                                              '\n'
            '    lynxg++ -f=<path/to/InputFile>'                                                                                                                '\n'
            ''                                                                                                                                                  '\n'
            'Options:'                                                                                                                                          '\n'
            '    -h  --help         Display this information. When this option is used, any other option is ignored'                                            '\n'
            '        --version      Display the version of the Lynx Engine. When this option is used, any other option but --help is ignored #TODO'             '\n'
            '    -v  --verbosity    Choose output verbosity. Default: 1. Possible values: 0, 1, 2, 3 #TODO'                                                     '\n'
            '    -b  --no-progress  Hide build progress bar. Default: off #TODO'                                                                                '\n'
            ''                                                                                                                                                  '\n'
            '    -m  --mode         Specify target platform and build configuration. This option is always required. e.g. -m=ld'                                '\n'
            '    -f  --file         Read the lynxg++ command from a file. Default:"./engine/Build.Application.sh" Any other option but -h and -v is ignored'    '\n'
            '    -p  --pack         Pack all files in a single executable file.       Default: off #TODO'                                                       '\n'
            ''                                                                                                                                                  '\n'
            '    Files with extension .comp are treated as GLSL compute shaders'                                                                                '\n'
            '    By default, the output .spv file has the same name of the .comp and is placed in the same directory'                                           '\n'
            '    A different output file can be specified with the syntax <path/to/inputfile>.comp;<path/to/outputfile>.spv'                                    '\n'
            ''                                                                                                                                                  '\n'
            'Selectors allow you to use a single command to build applications for different platforms and configurations'                                      '\n'
            '    -a:  --always      Always use the arguments, regardless of platform or configuration        e.g. -a: main.cpp"'                                '\n'
            '    -l:  --linux       Only use the arguments when building for Linux                           e.g. -l: -pthread"    e.g. -l: -pthread -Dlinux"'  '\n'
            '    -w:  --windows     Only use the arguments when building for Windows                         e.g. -w: -mthread"    e.g. -w: -mthread -Dwin10"'  '\n'
            '    -d:  --debug       Only use the arguments when building in Debug mode                       e.g. -d: -Og"         e.g. -d: -Og -g3"'           '\n'
            '    -r:  --release     Only use the arguments when building in Release mode                     e.g. -r: -O3"         e.g. -r: -O3 -g0"'           '\n'
            ''                                                                                                                                                  '\n'
            '    Each selector only affects the arguments between itself and the next selector'                                                                 '\n'
            '    Additionally, -ld:, -lr:, -wd: and -wr: selectors can be used to activate arguments based on both the active configuration and target platform''\n'
            '    Any unrecognized argument inside a selector is forwarded to g++'                                                                               '\n'
            '    Selectors can be repeated multiple times. The active arguments will preserve their order'                                                             '\n'
            ''                                                                                                                                                  '\n'
            'Verbosity:'                                                                                                                                        '\n'
            '    #TODO'                                                                                                                                         '\n'
        )
        return 0

    elif args.version:
        print('//TODO implement version option')
        return 0

    elif args.f != None:
        with open(args.f, 'r') as f:
            try:
                fArgs = shlex.split(f.read(), comments = True) #FIXME add glob parsing. glob module
                return run(fArgs[1:])
            except FileNotFoundError:
                print(f'Cannot open file "{ args.f }"')
                return 1


    elif args.m is None:
        print('lynxg++: error: the following arguments are required: -m/--mode')
        return 2

    else:
        # Select active arguments
        cmd:list = args.a
        cmd += args.l if args.m[0] == 'l' else args.w
        cmd += args.d if args.m[1] == 'd' else args.r
        cmd += (
            args.ld if args.m == 'ld' else
            args.lr if args.m == 'lr' else
            args.wd if args.m == 'wd' else
            args.wr
        )


        # Sort and parse arguments
        cmd.sort()
        for i in range(0, len(cmd)):
            cmd[i] = cmd[i][5:]



        # Build GLSLC command
        FLAGS : str  = ''
        SRC   : list = []
        COMP  : list = []

        if args.m[1] == 'd':
            FLAGS += '-DLNX_DEBUG -rdynamic'

        for e in cmd:
            if e[-5:] == '.comp':
                g = glob.glob(e)
                if len(g) > 0: COMP += g
                else:          COMP += [ e ]

            elif e[-4:] == '.cpp' :
                g = glob.glob(e)
                if len(g) > 0: SRC += g
                else:          SRC += [ e ]

            else: FLAGS += ' ' + e

        # Return
        return ns(**{
            'mode'  : args.m,
            'FLAGS' : FLAGS,
            'SRC'   : ' '.join(SRC ),
            'COMP'  : ' '.join(COMP)
        })
        # while i < len(cmd):
        #     r = re.match(r'^(.*)\.comp$', cmd[i])
        #     if r != None:
        #         cmd[i] += ';' + r.group(1) + '.spv'

        #     r = re.match(r'^(.*)\.comp;(.*)\.spv$', cmd[i]) #TODO allow the user to modify interface output files
        #     if r != None:
        #         cmdsh += [[r.group(1), r.group(2)]]
        #         del(cmd[i])
        #     else:
        #         i += 1
        # while i < len(cmd):
        #     r = re.match(r'^(.*)\.comp$', cmd[i])
        #     if r != None:
        #         cmd[i] += ';' + r.group(1) + '.spv'

        #     r = re.match(r'^(.*)\.comp;(.*)\.spv$', cmd[i]) #TODO allow the user to modify interface output files
        #     if r != None:
        #         cmdsh += [[r.group(1), r.group(2)]]
        #         del(cmd[i])
        #     else:
        #         i += 1




        #Build G++ command

        # cmd = ['g++', '-pthread', '-I' + rePath] + cmd                  #Default g++ call, , pthread, include project root
        # cmd += ['-std=c++20', '-m64', '-L/usr/lib64', '-L/lib64']       #Use C++20, build for 64bit environments, prefer 64bit libraries
        # cmd += ['-include', 'Lynx/Core/VkDef.hpp']                      #Include forced vulkan macros
        # cmd += ['-include', 'Lynx/Lynx_config.hpp']                     #Include engine configuration macros
        # if args.m[1] == 'd': cmd += ['-DLNX_DEBUG', '-rdynamic']        #Activate Lynx debug checks when in debug mode
        # cmd += ['-ffile-prefix-map=' + apPath + '=']                    #Fix file prefix

        # if args.e is False:
            # cmd += [  ]    #Define engine path function

#     rePath + '/Lynx/getEnginePath.cpp',                             #Add engine path definition  #FIXME
#     rePath + '/Lynx/Core/Env.cpp',                                  #Add runtime environment variables
#     './.engine/Build/' + _pf + '/Lynx' + _cf                        #Add engine binaries
# ]

# if not args.e: cmd += [                                         #When building user application
#     '-I' + '.',                                                     #Add workspace include path
#     '-ldl', '-lrt', '-lXrandr', '-lXi', '-lXcursor', '-lXinerama', '-lX11', #Link dependencies
#     '-lvulkan', '-Bstatic', '-lglfw'                                #Link Vulkan dynamically and GLFW statically
# ]




# # Get engine path
# rePath:str = ''
# with open('./.engine/.rePath', 'r') as f:
#     rePath = f.read()




# Get absolute project path
apPath:str = ''
with open('./.engine/.apPath', 'r') as f:
    apPath = f.read()

# Get relative project path
rpPath:str = ''
with open('./.engine/.rpPath', 'r') as f:
    rpPath = f.read()

# Get relative engine path
rePath:str = ''
with open('./.engine/.rePath', 'r') as f:
    rePath = f.read()

# Set complete names for platform and configuration
# _pf:str = 'Linux' if args.m[0] == 'l' else 'Windows'
# _cf:str = 'Debug' if args.m[1] == 'd' else 'Release'


# Parse application arguments
aRet = run(sys.argv[1:])
if isinstance(aRet, int):
    sys.exit(aRet)


# Parse engine arguments
with open('.engine/Build.Engine.sh') as f:
    eRet = run([ f"--mode={ aRet.mode }" ] + shlex.split(f.read(), comments = True)[1:]) #FIXME add glob parsing. glob module
    if isinstance(eRet, int):
        sys.exit(eRet)


# Run build
makeCmd = [ #FIXME escape strings or pass a list
    'make', '-C', rePath, #! Run from user application, cd into engine repo
    f"CPP    = { 'g++' if aRet.mode[0] == 'l' else '//TODO add windows compiler' }",
    f"OUTPUT = { 'Linux' if aRet.mode[0] == 'l' else 'Windows' }/{ 'Debug' if aRet.mode[1] == 'd' else 'Release' }",
    f'APP    = { rpPath }',
    f'EFLAGS = { eRet.FLAGS }',
    f'AFLAGS = { aRet.FLAGS } -DenginePath="{ apPath }',
    f'ESRC   = { eRet.SRC   }',
    f'ASRC   = { aRet.SRC   }',
    f'ECOMP  = { eRet.COMP  }',
    f'ACOMP  = { aRet.COMP  }'
]
print('Running:\n' + (' '.join(makeCmd)) + '\n\n')
sys.exit(subprocess.run(makeCmd).returncode)



#TODO escape ""s




# def runCmd(args, v:int):
#     output = ''
#     try:
#         output = subprocess.check_output(args, universal_newlines=True)
#         if v == 3:
#             print('\033[35m' + (' '.join(args)) + '\033[0m')
#     except subprocess.CalledProcessError as e:
#         print(
#             'lynxg++: fatal error: the task \033[1m"' + ' '.join(args) +
#             '"\033[0m exited with return code ' + str(e.returncode) +
#             ':\n\033[31m' + e.stdout + '\033[0m'
#         )
#         sys.exit(1)




# tsize = os.get_terminal_size().columns
# def center(stage):
#     h = '─' * (int(tsize / 2) - int(len(stage) / 2) - 3 - 1)
#     return f"\n\n\033[1m├{ h }[ { stage } ]{ h }┤\033[0m\n\n"




#Run GLSLC commands
# if len(cmdsh) > 0:
#     if args.v >= 1:
#         print(center('Compiling shaders'))

#     for i, files in enumerate(cmdsh):
#         if args.v >= 2:
#             if args.v == 3: print('')
#             s = 'Compiling shader "' + files[0] + '"...'
#             print(s + (' ' * (tsize - len(s) - 4 - 1 - 4)) + str(i + 1).rjust(4) + '/' + str(len(cmdsh)).ljust(4))
#         # if args.v >= 1:
#             # print(str(i + 1) + '/' + str(len(cmdsh)), end = '\r')

#         runCmd(['glslangValidator', '-V', files[0] + '.comp', '-o', files[1] + '.spv'], args.v)
#         runCmd(['spirv-val', files[0] + '.spv'], args.v)
#         if args.v == 3:
#             print('\033[35mGenerating interface files...\033[0m')
#         r = GlslToCpp.parseShader(files[0] + '.comp', _pf, rePath, args.e)
#         if r != 0: sys.exit(r)
#     print('\n')




# #Run G++ command
# if len(cmd) > 1:
#     if args.v >= 1:
#         print(center('Compiling translation units'))
#         print('\n')
#         print('\033[35m' + (' '.join(cmd)) + '\n\033[0m')
#     subprocess.run(cmd)