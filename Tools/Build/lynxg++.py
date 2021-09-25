import re, sys, os, subprocess, argparse as ap
from argparse import Namespace as ns
import shlex, glob
#python3.9 -m py_compile lynxg++.py && { python3 -m PyInstaller -F --clean ./lynxg++.py; cp ./dist/lynxg++ ./; rm -r ./dist; rm ./build -r; rm ./lynxg++.spec; }

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

    # Help
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


    # Version
    elif args.version:
        print('//TODO implement version option')
        return 0


    # Read from file
    elif args.f != None:
        with open(args.f, 'r') as f:
            try:
                fArgs = shlex.split(f.read(), comments = True)
                return run(fArgs[1:])
            except FileNotFoundError:
                print(f'Cannot open file "{ args.f }"')
                return 1


    # Check mode
    elif args.m is None:
        print('lynxg++: error: the following arguments are required: -m/--mode')
        return 2


    # Ok, start parsing
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
        CPP   : list = []
        GLS   : list = []
        LINK  : list = []

        if args.m[1] == 'd':
            FLAGS += [ '-DLNX_DEBUG -rdynamic' ]


        # Parse -x flags and expand glob paths
        #TODO add -xgls and -x gls for GLSL files with no .comp extension
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



        #FIXME merge split flags like -I ./dir before parsing
        i = 0
        while i < len(cmd):                             # For each element
            if isinstance(cmd[i], list):                    # If the element is a list of file names
                for e in cmd[i]:                                # For each file name
                    if   e[-4:] == '.cpp':  CPP += [e]              # Add to CPP if it's a cpp
                    elif e[-5:] == '.comp': GLS += [e]              # Add to GLS if it's a comp
                    else: LINK += [e]                               # Add to LINK if it's a library or an object file
            else:                                           # If its a flag or has still to be parsed
                if cmd[i] == None:                              # If the element was removed
                    i += 1                                          # Increment counter
                    continue                                        # Skip element
                if cmd[i][0] == '-':                            # If it's a flag
                    if cmd[i] == '-xcpp':                           # If it specifies a language in one argument
                        if cmd[i + 1][0] != '-':                        # If the file name is actually a file name
                            CPP += [expGlobSub(cmd[i + 1])]                 # Expand glob, use first path and reparse the others
                        else:                                           # If it's not
                            FLAGS += [cmd[i]]                               # Add -xcpp to FLAGS and let gcc throw an error
                    elif cmd[i] == '-x' and cmd[i + 1] == 'c++':    # If it specifies a language in two arguments
                        cmd[i + 1] = None                               # Remove the next argument
                        if cmd[i + 2][0] != '-':                        # If the file name is actually a file name
                            CPP += [expGlobSub(cmd[i + 2])]                 # Expand glob, use first path and reparse the others
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



        return ns(**{ #TODO rename
            'mode'  : args.m,
            'FLAGS' : FLAGS,
            'CPP'   : CPP,
            'GLS'   : GLS,
            'LINK'  : LINK
        })







# Get paths
with open('./.engine/.pabs', 'r') as f: pabs = f.read()
with open('./.engine/.eabs', 'r') as f: eabs = f.read()
with open('./.engine/.etop', 'r') as f: etop = f.read()
with open('./.engine/.ptoe', 'r') as f: ptoe = f.read()


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
makeCmd = [ #--debug #--just-print #-p
    'make', '-j11', '-p', '-r', '-C', ptoe, # 11 threads, no builtin rules, run from engine directory #TODO allow the use to choose the number of thread. default based on system max
    '--warn-undefined-variables', '--no-print-directory',
    '_EXEC'   f" = { 'g++' if aRet.mode[0] == 'l' else '//''TODO add windows compiler' }",
    '_OUTPUT' f" = { 'Linux' if aRet.mode[0] == 'l' else 'Windows' }/{ 'Debug' if aRet.mode[1] == 'd' else 'Release' }",
    '_APP'    f' = { etop }',
    '_EFLG'   f' = { " ".join(                                  eRet.FLAGS) } -DenginePath="\\"{ ptoe }\\""',
    '_AFLG'   f' = { " ".join(                                  aRet.FLAGS) } -DenginePath="\\"{ ptoe }\\""', #TODO fix engine path in shipping builds and standalone executables #FIXME replace paths with replative path from engine
    '_ECPP'   f' = { " ".join(                                  eRet.CPP  ) }',
    '_ACPP'   f' = { " ".join(os.path.relpath(s, eabs) for s in aRet.CPP  ) }',
    '_EGLS'   f' = { " ".join(                                  eRet.GLS  ) }',
    '_AGLS'   f' = { " ".join(os.path.relpath(s, eabs) for s in aRet.GLS  ) }',
    '_LINK'   f' = { " ".join(                                  aRet.LINK) }' #FIXME only replace actual paths
]
sys.exit(subprocess.run(makeCmd).returncode)
