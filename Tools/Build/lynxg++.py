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
        FLAGS : list = []
        SRC   : list = []
        COMP  : list = []
        LINK  : list = []

        if args.m[1] == 'd':
            FLAGS += [ '-DLNX_DEBUG -rdynamic' ]



        # Parse -x flags and expand glob paths
        #TODO add -x glsl

        def expGlob(s:str):     # Expand glob paths
            os.chdir(ptoe)
            g = glob.glob(s)        # Get glob result #FIXME use engine or app path
            os.chdir(etop)
            if len(g) > 0:          # If it was a glob
                return g                # Return the list of paths
            else:                   # If it was not
                return [s]              # Return a list containing the original path only


        def expGlobSub(s:str):  # Expand glob paths, return the first path and assign the others to s. Set s to None if there was only one path
            g = expglob(s)          # Expand glob
            if len(g > 1):          # If there is more than 1 path
                s = g[1:]               # Overwrite the old file name element with the remaining paths
            else:                   # Else
                s = None                # Remove the file name argument
            return g[0]             # Return first path



        i = 0
        # for i, e in enumerate(cmd):                 # For each element
        while i < len(cmd):                             # For each element
            if isinstance(cmd[i], list):                    # If the element is a list of file names
                for e in cmd[i]:                                # For each file name
                    if   e[-4:] == '.cpp':  SRC += [e]              # Add to SRC if it's a cpp
                    elif e[-5:] == '.comp': COMP += [e]             # Add to COMP if it's a comp
                    else: LINK += [e]                               # Add to LINK if it's a library or an object file
            else:                                           # If its a flag or has still to be parsed
                if cmd[i] == None:                              # If the element was removed
                    i += 1                                          # Increment counter
                    continue                                        # Skip element
                if cmd[i][0] == '-':                            # If it's a flag
                    if cmd[i] == '-xcpp':                           # If it specifies a language in one argument
                        if cmd[i + 1][0] != '-':                        # If the file name is actually a file name
                            SRC += [expGlobSub(cmd[i + 1])]                 # Expand glob, use first path and reparse the others
                        else:                                           # If it's not
                            FLAGS += [cmd[i]]                               # Add -xcpp to FLAGS and let gcc throw an error
                    elif cmd[i] == '-x' and cmd[i + 1] == 'c++':    # If it specifies a language in two arguments
                        cmd[i + 1] = None                               # Remove the next argument
                        if cmd[i + 2][0] != '-':                        # If the file name is actually a file name
                            SRC += [expGlobSub(cmd[i + 2])]                 # Expand glob, use first path and reparse the others
                        else:                                           # If it's not
                            FLAGS += [cmd[i], cmd[i + 1]]                   # Add -x cpp to FLAGS and let gcc throw an error
                    elif cmd[i] == '-l':                            # If it's a library name in 2 arguments
                        if cmd[i + 1][0] != '-':                        # If the library name is actually a library name
                            LINK += [expGlobSub(cmd[i + 1])]                # Expand glob, use first path and reparse the others
                        else:                                           # If it's not
                            FLAGS += [cmd[i]]                               # Add -l to LINK and let gcc throw an error
                    elif cmd[i][:1] == '-l':                        # If it's a library name in 1 argument
                        LINK += [cmd[i]]                                # Add the argument to LINK
                    else:                                           # If it's something else
                        FLAGS += [cmd[i]]                               # Add to FLAGS
                else:                                           # If it's a file path
                    cmd[i] = expGlob(cmd[i])                        # Expand glob
                    i -= 1                                          # Parse element again
            i += 1                                          # Increment counter
        #sorry



        # # Sort options
        # for e in cmd:
        #     if e[-5:] == '.comp':
        #         g = glob.glob(e)
        #         if len(g) > 0: COMP += g
        #         else:          COMP += [ e ]

        #     elif e[-4:] == '.cpp' :
        #         g = glob.glob(e)
        #         if len(g) > 0: SRC += g
        #         else:          SRC += [ e ]

        #     else: FLAGS += [ e ]

        # Return
        return ns(**{ #TODO rename
            'mode'  : args.m,
            'FLAGS' : FLAGS,
            'SRC'   : SRC,
            'COMP'  : COMP,
            'LINK'  : LINK
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

        # cmd = ['g++', '-pthread', '-I' + ptoe] + cmd                  #Default g++ call, , pthread, include project root
        # cmd += ['-std=c++20', '-m64', '-L/usr/lib64', '-L/lib64']       #Use C++20, build for 64bit environments, prefer 64bit libraries
        # cmd += ['-include', 'Lynx/Core/VkDef.hpp']                      #Include forced vulkan macros
        # cmd += ['-include', 'Lynx/Lynx_config.hpp']                     #Include engine configuration macros
        # if args.m[1] == 'd': cmd += ['-DLNX_DEBUG', '-rdynamic']        #Activate Lynx debug checks when in debug mode
        # cmd += ['-ffile-prefix-map=' + pabs + '=']                    #Fix file prefix

        # if args.e is False:
            # cmd += [  ]    #Define engine path function

#     ptoe + '/Lynx/getEnginePath.cpp',                             #Add engine path definition  #FIXME
#     ptoe + '/Lynx/Core/Env.cpp',                                  #Add runtime environment variables
#     './.engine/Build/' + _pf + '/Lynx' + _cf                        #Add engine binaries
# ]

# if not args.e: cmd += [                                         #When building user application
#     '-I' + '.',                                                     #Add workspace include path
#     '-ldl', '-lrt', '-lXrandr', '-lXi', '-lXcursor', '-lXinerama', '-lX11', #Link dependencies
#     '-lvulkan', '-Bstatic', '-lglfw'                                #Link Vulkan dynamically and GLFW statically
# ]




# # Get engine path
# ptoe:str = ''
# with open('./.engine/.ptoe', 'r') as f:
#     ptoe = f.read()




# Get absolute project path
pabs:str = ''
with open('./.engine/.pabs', 'r') as f:
    pabs = f.read()

# Get relative project path
etop:str = ''
with open('./.engine/.etop', 'r') as f:
    etop = f.read()

# Get relative engine path
ptoe:str = ''
with open('./.engine/.ptoe', 'r') as f:
    ptoe = f.read()

# Set complete names for platform and configuration
# _pf:str = 'Linux' if args.m[0] == 'l' else 'Windows'
# _cf:str = 'Debug' if args.m[1] == 'd' else 'Release'


# Parse application arguments
aRet = run(sys.argv[1:])
if isinstance(aRet, int):
    sys.exit(aRet)

# Parse engine arguments
with open('.engine/Build.Engine.sh') as f:
    eRet = run([ f"--mode={ aRet.mode }" ] + shlex.split(f.read(), comments = True)[1:])
    if isinstance(eRet, int):
        sys.exit(eRet)


# Run build
makeCmd = [
    'make', '-j8', '-C', ptoe, #! Run from user application, cd into engine repo
    'CPP'    f" = { 'g++' if aRet.mode[0] == 'l' else '//''TODO add windows compiler' }",
    'OUTPUT' f" = { 'Linux' if aRet.mode[0] == 'l' else 'Windows' }/{ 'Debug' if aRet.mode[1] == 'd' else 'Release' }",
    'APP'    f' = { etop }',
    'EFLAGS' f' = { " ".join(eRet.FLAGS) }',
    'AFLAGS' f' = { " ".join(aRet.FLAGS) } -DenginePath="{ ptoe }"', #TODO fix engine path in shipping builds and standalone executables
    'ESRC'   f' = { " ".join(eRet.SRC) }',
    'ASRC'   f' = { " ".join((etop + "/" + s) for s in aRet.SRC) }',
    'ECOMP'  f' = { " ".join(eRet.COMP) }',
    'ACOMP'  f' = { " ".join((etop + "/" + s) for s in aRet.COMP) }',
    'LINK'   f' = { " ".join((etop + "/" + s) for s in aRet.COMP) }'
]

print(f"""Using: [
    { makeCmd[-7] },\n
    { makeCmd[-6] },\n
    { makeCmd[-5] },\n
    { makeCmd[-4] },\n
    { makeCmd[-3] },\n
    { makeCmd[-2] },\n
    { makeCmd[-1] }
]""")

sys.exit(subprocess.run(makeCmd).returncode)

#TODO cleanup
#FIXME add engine init functions
#FIXME add engine init pointers



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
#         r = GlslToCpp.parseShader(files[0] + '.comp', _pf, ptoe, args.e)
#         if r != 0: sys.exit(r)
#     print('\n')




# #Run G++ command
# if len(cmd) > 1:
#     if args.v >= 1:
#         print(center('Compiling translation units'))
#         print('\n')
#         print('\033[35m' + (' '.join(cmd)) + '\n\033[0m')
#     subprocess.run(cmd)