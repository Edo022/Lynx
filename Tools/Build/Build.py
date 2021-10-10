import re, sys, os, subprocess, argparse as ap, shlex, glob
from Paths import *
import Alloy_tmpp, Utils








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
    try:
        with open(file) as f:
            t = list(Utils.clearBuild(f.read()))[1:-1]
    except FileNotFoundError:
        raise Exception(f'Cannot open "{ file }": file not found')



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








# Parse build files ------------------------------------------------------------------------------------------------------------------------#








# Parse argv
p = ap.ArgumentParser(prog = 'Build', add_help = False, usage = 'Build -h\nBuild --version\nBuild -m=<mode> -f=<Path/To/BuildFile> [-v=<verbosity>]')
p.add_argument('-h', '--help',      action = 'store_true', dest = 'h')
p.add_argument('--version',         action = 'store_true', dest = 'version')
p.add_argument('-m', '--mode',      action = 'store',      dest = 'm', choices = ['wd', 'wr', 'ld', 'lr'])
p.add_argument('-f', '--file',      action = 'store',      dest = 'f')
p.add_argument('-v', '--verbosity', action = 'store',      dest = 'v', choices = [0, 1, 2, 3], default = 2, type = int)
args = p.parse_args()


# Help
if args.h:
    print(
        'Usage:'                                                                                                                                            '\n'
        '    Build --help'                                                                                                                                  '\n'
        '    Build --version'                                                                                                                               '\n'
        '    Build -m=<mode> [<options...>] -<selector>: <g++ arguments...> <GLSL files...>'                                                                '\n'
        '    Build -f=<path/to/InputFile>'                                                                                                                  '\n'
        ''                                                                                                                                                  '\n'
        'Options:'                                                                                                                                          '\n'
        '    -h  --help         Display this information. When this option is used, any other option is ignored'                                            '\n'
        '        --version      Display the version of the Lynx Engine. When this option is used, any other option but --help is ignored #TODO'             '\n'
        '    -v  --verbosity    Choose output verbosity. Default: 1. Possible values: 0, 1, 2, 3 #TODO'                                                     '\n'
        '    -b  --no-progress  Hide build progress bar. Default: off #TODO'                                                                                '\n'
        ''                                                                                                                                                  '\n'
        '    -m  --mode         Specify target platform and build configuration. This option is always required. e.g. -m=ld'                                '\n'
        '    -f  --file         Read the Build command from a file. Default:"./engine/Application.lnxbuild.sh" Any other option but -h and -v is ignored'   '\n'
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
        '    Selectors can be repeated multiple times. The active arguments will preserve their order'                                                      '\n'
        ''                                                                                                                                                  '\n'
        'Verbosity:'                                                                                                                                        '\n'
        '    #TODO'                                                                                                                                         '\n'
    )
    sys.exit(0)


# Version
elif args.version:
    print('//TODO implement version option')
    sys.exit(0)


# Check mode
elif args.m is None:
    raise Exception('Build: error: the following arguments are required: -m/--mode')



# Read from file
elif args.f != None:
    try:
        with open(args.f, 'r') as f:
            # Parse arguments
            os.chdir(AtoE)
            eData = parse(EtoA + '/.engine/Engine.lnxbuild.sh', args.m)
            os.chdir(EtoA)
            aData = parse(args.f, args.m)

            # Run build
            Alloy_tmpp.build('g++', f'{ "Linux" if args.m[0] == "l" else "Windows" }/{ "Debug" if args.m[1] == "d" else "Release" }', eData, aData)

    except FileNotFoundError:
        raise Exception(f'Cannot open "{ args.f }": file not found')


