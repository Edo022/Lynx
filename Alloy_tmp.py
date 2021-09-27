import os, sys, subprocess, shlex
from pathlib import Path




with open('./.engine/.etop', 'r') as f: etop = f.read()
with open('./.engine/.ptoe', 'r') as f: ptoe = f.read()
os.chdir(ptoe)










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








def checkCmd(args):
    print(f'> { " ".join(args) }\n')
    r = subprocess.run(args = args,  text = True, capture_output = True)

    if r.returncode != 0:
        print(f'\033[31mCommand "{ " ".join(args) }" failed with exit code { str(r.returncode) }:\033[37m\n')
        print(f'\033[31mstderr:\033[37m\n{ r.stderr }\n\033[31mstdout:\033[37m\n{ r.stdout }')
        exit(r.returncode)


#TODO check dependencies
def BuildGSI(SPV, GSI, GLS, isEngine):
    for v, i, d in zip(SPV, GSI, GLS):
        print('Compiling shader ' + d)
        checkCmd(['glslangValidator', '-V', d, '-o', v ])
        print('Generating shader interface for ' + d)
        checkCmd(['python3', 'Tools/Build/GlslToCpp.py', d, etop, str(isEngine)])


#TODO check dependencies
def BuildOBJ(EXEC, FLG, OBJ, CPP):
    for o, d in zip(OBJ, CPP):
        print('Compiling ' + d)
        checkCmd([EXEC] + FLG + ['-c', '-xc++', d, '-o', o ])




def dirs(EOUT:str, AOUT:str):
    # Create directories for generated files
    os.makedirs(exist_ok = True, name = f'{ etop }/.engine/src/Generated')
    os.makedirs(exist_ok = True, name = f'./src/Generated')
    os.makedirs(exist_ok = True, name = f'{ etop }/.engine/src/Generated/Shaders')
    os.makedirs(exist_ok = True, name = f'./src/Generated/Shaders')

    # Create output directories
    os.makedirs(exist_ok = True, name = f'{ EOUT }')
    os.makedirs(exist_ok = True, name = f'{ AOUT }')
    os.makedirs(exist_ok = True, name = f'{ EOUT }/Shaders')
    os.makedirs(exist_ok = True, name = f'{ AOUT }/Shaders')




def elib(
    ELIB:str, EXEC:str, EFLG:list,
    EGLS:list, EGSI:list, ECPP:list,
    ESPV:list, EGSO:list, EOBJ:list
):
    BuildGSI(SPV = ESPV, GSI = EGSI, GLS = EGLS, isEngine = True)
    BuildOBJ(EXEC = EXEC, FLG = EFLG, OBJ = EGSO + EOBJ, CPP = EGSI + ECPP)
    print('Writing Lynx Engine library')
    subprocess.run(['ar', '-rcs', ELIB] + EGSO + EOBJ)




def abin(
    ELIB:str, ABIN:str, EXEC:str, AFLG:list,
    AGLS:list, AGSI:list, ACPP:list,
    ASPV:list, AGSO:list, AOBJ:list
):
    BuildGSI(SPV = ASPV, GSI = AGSI, GLS = AGLS, isEngine = False)
    BuildOBJ(EXEC = EXEC, FLG = AFLG, OBJ = AGSO + AOBJ, CPP = AGSI + ACPP)
    print('Writing executable file')
    subprocess.run([EXEC] + AFLG + AGSO + AOBJ + [ELIB] + LINK + ['-o', ABIN])




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
        f'-DenginePath=\\"\\\\\\"{ ptoe }\\\\\\"\\"',       # Engine path macro #FIXME
    ]


    EFLG += SFLG                                        # Append default flags to user defined flags    #! Passed by the wrapper
    AFLG += SFLG                                        # Append default flags to user defined flags    #! Passed by the wrapper
    EOUT = f'{ ptoe }/.engine/bin/Lnx/{ OUTPUT }'       # Path to the engine binary output directory
    AOUT = f'{ ptoe }/.engine/bin/App/{ OUTPUT }'       # Path to the engine application binary output directory
    ELIB = f'{ EOUT }/libLynxEngine.a'                  # Path to the engine static library
    ABIN = f'{ ptoe }/tmp.out'                          # Path to the application executable file


    ESPV = list((               f'./src/Generated/Shaders/{ Path(s).stem }.spv')     for s in EGLS)     # Get output .spv files paths
    ASPV = list((f'{ ptoe }/.engine/src/Generated/Shaders/{ Path(s).stem }.spv')     for s in AGLS)     # Get output .spv files paths                   #! Can be empty
    EGSI = list((               f'./src/Generated/Shaders/{ Path(s).stem }.gsi.cpp') for s in EGLS)     # Get generated shader interfaces source files
    AGSI = list((f'{ ptoe }/.engine/src/Generated/Shaders/{ Path(s).stem }.gsi.cpp') for s in AGLS)     # Get generated shader interfaces source files  #! Can be empty

    EGSO = list((f'{ EOUT }/Shaders/{ Path(s).stem }.gsi.o') for s in EGLS)     # Get generated shader interfaces .o files
    AGSO = list((f'{ AOUT }/Shaders/{ Path(s).stem }.gsi.o') for s in AGLS)     # Get generated shader interfaces .o files
    EOBJ = list((f'{ EOUT }'     f'/{ Path(s).stem }.o')     for s in ECPP)     # Get output .o files of the non-generated C++ source files
    AOBJ = list((f'{ AOUT }'     f'/{ Path(s).stem }.o')     for s in ACPP)     # Get output .o files of the non-generated C++ source files


    dirs(EOUT = EOUT, AOUT = AOUT)
    elib(ELIB = ELIB,              EXEC = EXEC, EFLG = EFLG, EGLS = EGLS, EGSI = EGSI, ECPP = ECPP, ESPV = ESPV, EGSO = EGSO, EOBJ = EOBJ)
    abin(ELIB = ELIB, ABIN = ABIN, EXEC = EXEC, AFLG = AFLG, AGLS = AGLS, AGSI = AGSI, ACPP = ACPP, ASPV = ASPV, AGSO = AGSO, AOBJ = AOBJ)








# ----------------------------------------- CLEAN ------------------------------------------------- #








def eclean():
    subprocess.run(['find', '.', '-type', 'f', '-wholename', './*.o',   '-delete'], cwd = f'{ etop }/.engine/bin')
    subprocess.run(['find', '.', '-type', 'f', '-wholename', './*.a',   '-delete'], cwd = f'{ etop }/.engine/bin')
    subprocess.run(['find', '.', '-type', 'f', '-wholename', './*.spv', '-delete'], cwd = './src/Generated')
    subprocess.run(['find', '.', '-type', 'f', '-wholename', './*.cpp', '-delete'], cwd = './src/Generated')
    subprocess.run(['find', '.', '-type', 'f', '-wholename', './*.hpp', '-delete'], cwd = './src/Generated')

def aclean():
    subprocess.run(['find', '.', '-type', 'f', '-wholename', './*.o',   '-delete'], cwd = f'{ etop }/.engine/bin')
    subprocess.run(['find', '.', '-type', 'f', '-wholename', './*.spv', '-delete'], cwd = f'{ etop }/.engine/src/Generated')
    subprocess.run(['find', '.', '-type', 'f', '-wholename', './*.cpp', '-delete'], cwd = f'{ etop }/.engine/src/Generated')
    subprocess.run(['find', '.', '-type', 'f', '-wholename', './*.hpp', '-delete'], cwd = f'{ etop }/.engine/src/Generated')

def clean():
    aclean()
    eclean()



eval(f'build({ ",".join(sys.argv[1:-1]) })')
# print(f'build({ ",".join(sys.argv[1:-1]) })')