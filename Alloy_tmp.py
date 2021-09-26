import os, sys, subprocess, shlex
from pathlib import Path




with open('./.engine/.etop', 'r') as f: etop = f.read()
with open('./.engine/.ptoe', 'r') as f: ptoe = f.read()
os.chdir(ptoe)


EXEC   = sys.argv[1][ len('_EXEC='  ):]
OUTPUT = sys.argv[2][ len('_OUTPUT='):]
APP    = sys.argv[3][ len('_APP='   ):]

LINK = shlex.split(sys.argv[10][len('_LINK='):]) + [    #Likner options
    '-L/usr/lib64', '-L/lib64',                             # Prefer 64bit libraries						#TODO fix windows build
    '-ldl', '-lrt', '-lXrandr', '-lXi',                     # Link dependencies								#TODO fix windows build
    '-lXcursor', '-lXinerama', '-lX11',                     # Link dependencies								#TODO fix windows build
    '-lvulkan', '-Bstatic', '-lglfw',                       # Link Vulkan dynamically and GLFW statically	#TODO fix windows build
]

SFLG = [                                                #Shared default C++ flags
    '-pthread', '-I.',                                      # Default g++ call, pthread, include project root
    '-std=c++20', '-m64',                                   # Use C++20, build for 64bit environments
    '-include', 'Lynx/Core/VkDef.hpp',                      # Include forced vulkan macros
    '-include', 'Lynx/Lynx_config.hpp',                     # Include engine configuration macros
    f'-ffile-prefix-map={ os.path.abspath(etop) }='         # Fix file prefix in debug infos
]




EFLG = SFLG + shlex.split(sys.argv[4][ len('_EFLG='):])             # Default + user defined flags      #! Passed by the wrapper
EGLS =        shlex.split(sys.argv[8][ len('_EGLS='):])             # Engine GLSL source files          #! Passed by the wrapper
ECPP =        shlex.split(sys.argv[6][ len('_ECPP='):])             # Engine C++  source files          #! Passed by the wrapper
EOUT = f'{ ptoe }/.engine/bin/Lnx/{ OUTPUT }'                       # Path to the engine binary output directory
ELIB = f'{ EOUT }/libLynxEngine.a'                                  # Path to the engine static library

ESPV = list((f'./src/Generated/Shaders/{ s }.spv')     for s in EGLS)   # Get output .spv files paths
EGSI = list((f'./src/Generated/Shaders/{ s }.gsi.cpp') for s in EGLS)   # Get generated shader interfaces source files

EGSO = list((f'{ EOUT }/Shaders/{ Path(s).stem }.gsi.o') for s in EGLS) # Get generated shader interfaces .o files
EOBJ = list((f'{ EOUT }'     f'/{ Path(s).stem }.o')     for s in ECPP) # Get output .o files of the non-generated C++ source files




AFLG = SFLG + shlex.split(sys.argv[5][ len('_AFLG='):])             # Default + user defined flags      #! Passed by the wrapper
AGLS =        shlex.split(sys.argv[9][ len('_AGLS='):])             # Application GLSL source files     #! Passed by the wrapper #! Can be empty
ACPP =        shlex.split(sys.argv[7][ len('_ACPP='):])             # Application C++  source files     #! Passed by the wrapper
AOUT = f'{ ptoe }/.engine/bin/App/{ OUTPUT }'                       # Path to the engine application binary output directory
ABIN = f'{ ptoe }/tmp.out'                                          # Path to the application executable file

ASPV = list((f'{ ptoe }/.engine/src/Generated/Shaders/{ s }.spv')     for s in AGLS)    # Get output .spv files paths                   #! Can be empty
AGSI = list((f'{ ptoe }/.engine/src/Generated/Shaders/{ s }.gsi.cpp') for s in AGLS)    # Get generated shader interfaces source files  #! Can be empty

AGSO = list((f'{ AOUT }/Shaders/{ Path(s).stem }.gsi.o') for s in AGLS)                 # Get generated shader interfaces .o files
AOBJ = list((f'{ AOUT }'     f'/{ Path(s).stem }.o')     for s in ACPP)                 # Get output .o files of the non-generated C++ source files









def debug():
    print(f'EFLG:{ len(EFLG) }\n{ EFLG }')
    print(f'ECPP:{ len(ECPP) }\n{ ECPP }')
    print(f'EOBJ:{ len(EOBJ) }\n{ EOBJ }')
    print(f'EGLS:{ len(EGLS) }\n{ EGLS }')
    print(f'EGSI:{ len(EGSI) }\n{ EGSI }')
    print(f'EGSO:{ len(EGSO) }\n{ EGSO }')
    print(f'ESPV:{ len(ESPV) }\n{ ESPV }')
    print('\n\n\n')
    print(f'AFLG:{ len(AFLG) }\n{ AFLG }')
    print(f'ACPP:{ len(ACPP) }\n{ ACPP }')
    print(f'AOBJ:{ len(AOBJ) }\n{ AOBJ }')
    print(f'AGLS:{ len(AGLS) }\n{ AGLS }')
    print(f'AGSI:{ len(AGSI) }\n{ AGSI }')
    print(f'AGSO:{ len(AGSO) }\n{ AGSO }')
    print(f'ASPV:{ len(ASPV) }\n{ ASPV }')
    print('\n\n\n')
    print(f'SFLG:{ len(SFLG) }\n{ SFLG }')
    print(f'LINK:{ len(LINK) }\n{ LINK }')








# ----------------------------------------- BUILD ------------------------------------------- #








#TODO check dependencies
def BuildGSI(SPV, GSI, GLS, isEngine):
    for v, i, d in zip(SPV, GSI, GLS):
        print('Compiling shader ' + d)
        subprocess.run(['glslangValidator', '-V', d, '-o', v ])
        print('Generating shader interface for ' + d)
        subprocess.run(['python3', 'Tools/Build/GlslToCpp.py', d, APP, isEngine ])



#TODO check dependencies
def BuildOBJ(EXEC, FLG, OBJ, CPP):
    for o, d in zip(OBJ, CPP):
        print('Compiling ' + d)
        subprocess.run([EXEC, FLG, '-c', '-xc++', d, '-o', o ])




def edirs():
    # Create directories for generated files
    os.makedirs(exist_ok = True, name = f'$(APP)/.engine/src/Generated')
    os.makedirs(exist_ok = True, name = f'./src/Generated')
    os.makedirs(exist_ok = True, name = f'$(APP)/.engine/src/Generated/Shaders')
    os.makedirs(exist_ok = True, name = f'./src/Generated/Shaders')

    # Create output directories
    os.makedirs(exist_ok = True, name = f'$(EOUT)')
    os.makedirs(exist_ok = True, name = f'$(AOUT)')
    os.makedirs(exist_ok = True, name = f'$(EOUT)/Shaders')
    os.makedirs(exist_ok = True, name = f'$(AOUT)/Shaders')


def elib():
    BuildGSI(SPV = ESPV, GSI = EGSI, GLS = EGLS)
    BuildOBJ(EXEC = EXEC, FLG = EFLG, OBJ = EGSO + EOBJ, CPP = EGSI + ECPP)
    print('Writing Lynx Engine library')
    subprocess.run(['ar', '-rcs', ELIB] + EGSO + EOBJ)


def abin():
    BuildGSI(SPV = ASPV, GSI = AGSI, GLS = AGLS)
    BuildOBJ(EXEC = EXEC, FLG = AFLG, OBJ = AGSO + AOBJ, CPP = AGSI + ACPP)
    print('Writing executable file')
    subprocess.run([EXEC] + AFLG + AGSO + AOBJ + [ELIB] + LINK + ['-o', ABIN])


def build():
    dirs()
    elib()
    abin()







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



eval(sys.argv[11] + '()')