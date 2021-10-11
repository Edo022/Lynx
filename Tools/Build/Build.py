import re, sys, os, subprocess, argparse as ap, shlex, glob
from Paths import *
import Alloy_tmpp, Utils
#'Selectors allow you to use a single command to build applications for different platforms and configurations'                                      '\n'
#'    -a:  --always      Always use the arguments, regardless of platform or configuration        e.g. -a: main.cpp"'                                '\n'
#'    -l:  --linux       Only use the arguments when building for Linux                           e.g. -l: -pthread"    e.g. -l: -pthread -Dlinux"'  '\n'
#'    -w:  --windows     Only use the arguments when building for Windows                         e.g. -w: -mthread"    e.g. -w: -mthread -Dwin10"'  '\n'
#'    -d:  --debug       Only use the arguments when building in Debug mode                       e.g. -d: -Og"         e.g. -d: -Og -g3"'           '\n'
#'    -r:  --release     Only use the arguments when building in Release mode                     e.g. -r: -O3"         e.g. -r: -O3 -g0"'           '\n'
#TODO








# Parse build files ------------------------------------------------------------------------------------------------------------------------#








translateSelector : dict = {
    'a' : 'a', 'always'  : 'a',
    'd' : 'd', 'debug'   : 'd',
    'r' : 'r', 'release' : 'r',
    'l' : 'l', 'linux'   : 'l',
    'w' : 'w', 'windows' : 'w'
}

def isActive(selector:str, mode:str):
    if selector in ['ld', 'lr', 'wd', 'wr'] + list(translateSelector.keys()):
        return selector == mode or translateSelector[selector] in ['a', mode[0], mode[1]]
    else:
        if selector in ['dl', 'rl', 'dw', 'rw']:
            raise Exception(f'Unknown selector "{ selector }". Did you mean: "{ selector[1] + selector[0] }"?')
        else:
            raise Exception(f'Unknown selector "{ selector }"')








def parse(file:str, mode:str):
    # Read and clear build file
    with open(file) as f:
        t = list(Utils.clearBuild(f.read()))[1:-1]



    # Parse sections
    sections : dict = {}                # Create parsed sections
    i : int = 0                         # Initialize counter
    while i < len(t):                   # For each section
        section = t[i]                      # Save section name
        sections[section] = []              # Create section list
        i += 1                              # Skip section name
        i += 1                              # Skip section {
        while t[i] != '}':                  # For each selector
            if isActive(t[i], mode):            # If the selector is active
                i += 1                              # Skip selector name
                i += 1                              # Skip selector {
                flag : str = ''                     # Create tmp flag
                while t[i] != '}':                  # For each flag
                    if t[i] == ';':                     # If the flag is complete
                        sections[section] += [flag]         # Add tmp flag to section flags
                        flag = ''                           # Reset tmp flag
                    elif t[i + 1] == '}':               # If the flag is complete bus has no trailing semicolon
                        sections[section] += [flag + t[i]]  # Add tmp flag to section flags
                        flag = ''                           # Reset tmp flag
                    else:                               # If the flag is not complete
                        flag += t[i]                        # Concatenate token to tmp flag
                    i += 1                              # Go to next flag
            else:                               # If it's not active
                while t[i] != '}':                  # For each flag
                    i += 1                              # Skip flag
            i += 1                              # Skip selector }
        i += 1                              # Skip section }




    def expSection(section:list):
        for s in section:
            for r in Utils.expGlob(s):
                yield(r)

    sections['cpp']             = list(expSection(sections['cpp']))
    sections['gls']             = list(expSection(sections['gls']))
    sections['forced_includes'] = list(expSection(sections['forced_includes']))

    #TODO move library paths and libraries to dedicated sections
    return sections








# Parse command line flags -----------------------------------------------------------------------------------------------------------------#








def eclear():
    return not(
            not subprocess.run(['find', '.', '-type', 'f', '-wholename', './Lnx/*.o', '-delete'], cwd = '/.engine/bin').returncode
        and not subprocess.run(['find', '.', '-type', 'f', '-wholename', './Lnx/*.a', '-delete'], cwd = '/.engine/bin').returncode
        and not subprocess.run(['find', '.', '-type', 'f', '-wholename', './*.spv',   '-delete'], cwd = f'{ AtoE }/src/Generated').returncode
        and not subprocess.run(['find', '.', '-type', 'f', '-wholename', './*.cpp',   '-delete'], cwd = f'{ AtoE }/src/Generated').returncode
        and not subprocess.run(['find', '.', '-type', 'f', '-wholename', './*.hpp',   '-delete'], cwd = f'{ AtoE }/src/Generated').returncode
    )


def aclear():
    return not(
            not subprocess.run(['find', '.', '-type', 'f', '-wholename', './App/*.o', '-delete'], cwd = './.engine/bin').returncode
        and not subprocess.run(['find', '.', '-type', 'f', '-wholename', './*.spv',   '-delete'], cwd = './.engine/src/Generated').returncode
        and not subprocess.run(['find', '.', '-type', 'f', '-wholename', './*.cpp',   '-delete'], cwd = './.engine/src/Generated').returncode
        and not subprocess.run(['find', '.', '-type', 'f', '-wholename', './*.hpp',   '-delete'], cwd = './.engine/src/Generated').returncode
    )


def clear():
    return not(not aclear() and not eclear())








# Clear tasks ------------------------------------------------------------------------------------------------------------------------------#








# Parse argv
p = ap.ArgumentParser(prog = 'Build', add_help = False, usage = '-m=<mode> -f=<PathTo/BuildFile> [--verbosity=<verbosity>]')
p.add_argument('-h',  '--help',      action = 'store_true', dest = 'h')
p.add_argument('-v',  '--version',   action = 'store_true', dest = 'v')
p.add_argument('-c',  '--clear',     action = 'store_true', dest = 'c')
p.add_argument('-ca', '--clear-all', action = 'store_true', dest = 'ca')
p.add_argument(       '--verbosity', action = 'store',      dest = 'vv', choices = [0, 1, 2, 3], default = 2, type = int)
p.add_argument('-b',  '--build',     action = 'store',      dest = 'b',  default = None)
p.add_argument('-ba', '--build-all', action = 'store',      dest = 'ba', default = None)
p.add_argument('-m',  '--mode',      action = 'store',      dest = 'm',  choices = ['wd', 'wr', 'ld', 'lr'])
p.add_argument('-p',  '--pack',      action = 'store_true', dest = 'p')
args = p.parse_args()




# No arguments
if args.b == None and args.ba == None and not(args.h or args.v or args.c or args.ca):
    print(
        '\nI don\'t know what you want me to do u.u'
        '\nUse Build -h for usage syntax and a list of options'
    )
    sys.exit(0)




# Help
if args.h:
    print(
        '\nUsage:'
        '\n    Build -h'
        '\n    Build -v'
        '\n    Build -c'
        '\n    Build -ca'
        '\n    Build -m=<mode> -b=<PathTo/BuildFile> [-v=<verbosity>]'
        '\n    Build -m=<mode> -ba=<PathTo/BuildFile> [-v=<verbosity>]'
        '\n'
        '\nActions:'
        '\n    -h   --help         Display this information'
        '\n    -v   --version      Display the version of the Lynx Engine'
        '\n    -c   --clear        Delete any file generated by the build tasks. This action can be specified together with -b or -ba to clean the files before the build starts'
        '\n    -ca  --clear-all    Same as -c, but also delete the files of the engine. After this action is used, a complete rebuild (-ba) is required in order to use the engine'
        '\n         --verbosity    Choose output verbosity. Default: 1. Possible values: 0, 1, 2, 3. Ignored when none of -c, -ca, -b and -ba is used'
        '\n'
        '\nBuild options:'
        '\n    -b   --build        Read the Build flags from a file and build the application. Requires -m. Default: "./.engine/Application.lnxbuild.sh"'
        '\n    -ba  --build-all    Same as -b, but also build the engine. Requires -m. The engine build file path is hard coded ("./.engine/Engine.lnxbuild.sh")'
        '\n    -m   --mode         Specify target platform and build configuration      Ignored when both -b and -ba are not used'
        '\n    -p   --pack         Pack all files in a single executable file.          Ignored when both -b and -ba are not used'
        '\n'
        '\nVerbosity:'
        '\n    #''TODO'
    )


# Version
if args.v:
    print('//''TODO implement version option')


# Clear
if args.ca:
    sys.exit(aclear())
elif args.c:
    sys.exit(clear())




# Read from file
if args.b != None or args.ba != None:
    # Check mode
    if args.m is None:
        raise Exception('Build: error: the following arguments are required: -m/--mode')


    # Parse engine arguments
    os.chdir(AtoE)
    eData = parse(EtoA + '/.engine/Engine.lnxbuild.sh', args.m)
    os.chdir(EtoA)

    # Parse application arguments
    f = args.ba if args.ba != None else args.b
    parse(f, args.m)

    # Run build
    sys.exit(Alloy_tmpp.build(
        'g++',
        f'{ "Linux" if args.m[0] == "l" else "Windows" }/{ "Debug" if args.m[1] == "d" else "Release" }',
        eData, aData, args.ba != None
    ))
