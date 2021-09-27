import os, sys, subprocess, shlex
from pathlib import Path




with open('./.engine/.etop', 'r') as f: etop = f.read()
with open('./.engine/.ptoe', 'r') as f: ptoe = f.read()
os.chdir(ptoe)









#TODO
def debug():
    print(f'EFLG:{ len(EFLG) }\n{ EFLG }\n')
    print(f'ECPP:{ len(ECPP) }\n{ ECPP }\n')
    print(f'EOBJ:{ len(EOBJ) }\n{ EOBJ }\n')
    print(f'EGLS:{ len(EGLS) }\n{ EGLS }\n')
    print(f'EGSI:{ len(EGSI) }\n{ EGSI }\n')
    print(f'EGSO:{ len(EGSO) }\n{ EGSO }\n')
    print(f'ESPV:{ len(ESPV) }\n{ ESPV }\n')
    print('\n\n\n')
    print(f'AFLG:{ len(AFLG) }\n{ AFLG }\n')
    print(f'ACPP:{ len(ACPP) }\n{ ACPP }\n')
    print(f'AOBJ:{ len(AOBJ) }\n{ AOBJ }\n')
    print(f'AGLS:{ len(AGLS) }\n{ AGLS }\n')
    print(f'AGSI:{ len(AGSI) }\n{ AGSI }\n')
    print(f'AGSO:{ len(AGSO) }\n{ AGSO }\n')
    print(f'ASPV:{ len(ASPV) }\n{ ASPV }\n')
    print('\n\n\n')
    print(f'SFLG:{ len(SFLG) }\n{ SFLG }\n')
    print(f'LINK:{ len(LINK) }\n{ LINK }\n')
    print('\n\n\n')
    print(f'OUTPUT: { OUTPUT }')
    print(f'EXEC:   { EXEC }')
    print(f'EOUT:   { EOUT }')
    print(f'AOUT:   { AOUT }')
    print(f'ELIB:   { ELIB }')
    print(f'ABIN:   { ABIN }')







# ----------------------------------------- BUILD ------------------------------------------- #








def progress(cur:int, tot:int):
    return f"[{ str(cur).rjust(len(str(tot)), '0') }/{ str(tot) } - { '{:3.2f}'.format(cur / tot * 100).rjust(6, '0') }%]"




def checkCmd(args):
    # print(f'> { " ".join(args) }\n')
    r = subprocess.run(args = args,  text = True, capture_output = True)

    if r.returncode != 0:
        print(f'\033[31mCommand "{ " ".join(args) }" failed with exit code { str(r.returncode) }:\033[37m\n')
        print(f'\033[31mstderr:\033[37m\n{ r.stderr }\n\033[31mstdout:\033[37m\n{ r.stdout }')
        exit(r.returncode)




#TODO check dependencies
def BuildGSI(SPV, GSI, GLS, isEngine):
    for i, (ov, oi, s) in enumerate(zip(SPV, GSI, GLS)):
        print(f'{ progress(i + 1, len(GLS)) } Compiling shader ' + s)
        checkCmd(['glslangValidator', '-V', s, '-o', ov ])

        print(f'{ progress(i + 1, len(GLS)) } Generating shader interface for ' + s)
        checkCmd(['python3', 'Tools/Build/GlslToCpp.py', s, etop, str(isEngine)])




#TODO check dependencies
def BuildOBJ(EXEC, FLG, OBJ, CPP):
    for i, (o, s) in enumerate(zip(OBJ, CPP)):
        print(f'{ progress(i + 1, len(CPP)) } Compiling ' + s)
        checkCmd([EXEC] + FLG + ['-c', '-xc++', s, '-o', o ])




def dirs(EOUT:str, AOUT:str):
    # Create directories for generated files
    os.makedirs(exist_ok = True, name = f'{ etop }/.engine/src/Generated')
    os.makedirs(exist_ok = True, name =                f'./src/Generated')
    os.makedirs(exist_ok = True, name = f'{ etop }/.engine/src/Generated/Shaders')
    os.makedirs(exist_ok = True, name =                f'./src/Generated/Shaders')

    # Create output directories
    os.makedirs(exist_ok = True, name = f'{ EOUT }')
    os.makedirs(exist_ok = True, name = f'{ AOUT }')
    os.makedirs(exist_ok = True, name = f'{ EOUT }/Shaders')
    os.makedirs(exist_ok = True, name = f'{ AOUT }/Shaders')








def build(
    EXEC:str, OUTPUT:str, LINK:list,    # Path to C++ compiler      # Binary output suffix              # Linker options
    EFLG:list, EGLS:list, ECPP:list,    # Engine flags              # Engine GLS source files           # Engine C++ source files
    AFLG:list, AGLS:list, ACPP:list     # Application flags         # Application GLS source files      # Application C++ source files
):
    LINK += [                                           # Default Likner options
        '-L/usr/lib64', '-L/lib64',                         # Prefer 64bit libraries                        #TODO fix windows build
        '-ldl', '-lrt', '-lXrandr', '-lXi',                 # Link dependencies                             #TODO fix windows build
        '-lXcursor', '-lXinerama', '-lX11',                 # Link dependencies                             #TODO fix windows build
        '-lvulkan', '-Bstatic', '-lglfw',                   # Link Vulkan dynamically and GLFW statically   #TODO fix windows build
    ]

    SFLG = [                                            # Shared default C++ flags
        '-std=c++20', '-m64', '-pthread',                   # Use C++20, build for 64bit environments, use pthread
        '-I.', '-Isrc', f'-I{ etop }/.engine/src',          # Include from src directories and project root
        '-include', 'Lynx/Core/VkDef.hpp',                  # Include forced vulkan macros
        '-include', 'Lynx/Lynx_config.hpp',                 # Include engine configuration macros
        f'-ffile-prefix-map={ os.path.abspath(etop) }=',    # Fix file prefix in debug infos
        f'-DenginePath="{ ptoe }"',                         # Engine path macro #FIXME
    ]


    EFLG += SFLG                                        # Append default flags to user defined flags    #! Passed by the wrapper
    AFLG += SFLG                                        # Append default flags to user defined flags    #! Passed by the wrapper
    EOUT = f'{ etop }/.engine/bin/Lnx/{ OUTPUT }'       # Path to the engine binary output directory
    AOUT = f'{ etop }/.engine/bin/App/{ OUTPUT }'       # Path to the engine application binary output directory
    ELIB = f'{ EOUT }/libLynxEngine.a'                  # Path to the engine static library
    ABIN = f'{ etop }/tmp.out'                          # Path to the application executable file


    ESPV = list((               f'./src/Generated/Shaders/{ Path(s).stem }.spv')     for s in EGLS)     # Get output .spv files paths
    ASPV = list((f'{ etop }/.engine/src/Generated/Shaders/{ Path(s).stem }.spv')     for s in AGLS)     # Get output .spv files paths                   #! Can be empty
    EGSI = list((               f'./src/Generated/Shaders/{ Path(s).stem }.gsi.cpp') for s in EGLS)     # Get generated shader interfaces source files
    AGSI = list((f'{ etop }/.engine/src/Generated/Shaders/{ Path(s).stem }.gsi.cpp') for s in AGLS)     # Get generated shader interfaces source files  #! Can be empty

    EGSO = list((f'{ EOUT }/Shaders/{ Path(s).stem }.gsi.o') for s in EGLS)     # Get generated shader interfaces .o files
    AGSO = list((f'{ AOUT }/Shaders/{ Path(s).stem }.gsi.o') for s in AGLS)     # Get generated shader interfaces .o files
    EOBJ = list((f'{ EOUT }'     f'/{ Path(s).stem }.o')     for s in ECPP)     # Get output .o files of the non-generated C++ source files
    AOBJ = list((f'{ AOUT }'     f'/{ Path(s).stem }.o')     for s in ACPP)     # Get output .o files of the non-generated C++ source files




    # Create missing directories
    dirs(EOUT = EOUT, AOUT = AOUT)


    # Build libraries
    BuildGSI(SPV = ESPV, GSI = EGSI, GLS = EGLS, isEngine = True)
    BuildOBJ(EXEC = EXEC, FLG = EFLG, OBJ = EGSO + EOBJ, CPP = EGSI + ECPP)
    print(f'Writing Lynx Engine library "{ ELIB }"')
    subprocess.run(['ar', '-rcs', ELIB] + EGSO + EOBJ)


    # Build executable
    BuildGSI(SPV = ASPV, GSI = AGSI, GLS = AGLS, isEngine = False)
    BuildOBJ(EXEC = EXEC, FLG = AFLG, OBJ = AGSO + AOBJ, CPP = AGSI + ACPP)
    print(f'Writing application executable file "{ ABIN }"')
    subprocess.run([EXEC] + AFLG + AGSO + AOBJ + [ELIB] + LINK + ['-o', ABIN])

    return 0








# ----------------------------------------- CLEAR ------------------------------------------------- #








def eclear():
    return not(
            not subprocess.run(['find', '.', '-type', 'f', '-wholename', './Lnx/*.o', '-delete'], cwd = f'{ etop }/.engine/bin').returncode
        and not subprocess.run(['find', '.', '-type', 'f', '-wholename', './Lnx/*.a', '-delete'], cwd = f'{ etop }/.engine/bin').returncode
        and not subprocess.run(['find', '.', '-type', 'f', '-wholename', './*.spv',     '-delete'], cwd = './src/Generated').returncode
        and not subprocess.run(['find', '.', '-type', 'f', '-wholename', './*.cpp',     '-delete'], cwd = './src/Generated').returncode
        and not subprocess.run(['find', '.', '-type', 'f', '-wholename', './*.hpp',     '-delete'], cwd = './src/Generated').returncode
    )


def aclear():
    return not(
            not subprocess.run(['find', '.', '-type', 'f', '-wholename', './App/*.o', '-delete'], cwd = f'{ etop }/.engine/bin').returncode
        and not subprocess.run(['find', '.', '-type', 'f', '-wholename', './*.spv',     '-delete'], cwd = f'{ etop }/.engine/src/Generated').returncode
        and not subprocess.run(['find', '.', '-type', 'f', '-wholename', './*.cpp',     '-delete'], cwd = f'{ etop }/.engine/src/Generated').returncode
        and not subprocess.run(['find', '.', '-type', 'f', '-wholename', './*.hpp',     '-delete'], cwd = f'{ etop }/.engine/src/Generated').returncode
    )


def clear():
    return not(not aclear() and not eclear())




if sys.argv[-1] in ['clear', 'aclear', 'eclear']:
    sys.exit(eval(f'{ sys.argv[-1] }()'))

elif sys.argv[-1] == 'build':
    sys.exit(eval(f'{ sys.argv[-1] }({ ",".join(sys.argv[1:-1]) })'))

else:
    sys.exit(f'Unknown task "{ sys.argv[-1] }"')