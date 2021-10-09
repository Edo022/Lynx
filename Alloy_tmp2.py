import os, sys, subprocess, shlex, threading, multiprocessing, time, re
from pathlib import Path
#TODO check dependencies



with open('./.engine/.EtoA', 'r') as f: EtoA = f.read()
with open('./.engine/.AtoE', 'r') as f: AtoE = f.read()
os.chdir(AtoE)

poolMutex = threading.Lock()
avlThrs = multiprocessing.cpu_count() - 1
totThrs = avlThrs
curThr = 0
poolErr = False

bgreen = '\033[1;32m'
white = '\033[0;37m'
red = '\033[1;31m'




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
    global poolErr
    global avlThrs
    r = subprocess.run(args = args,  text = True, capture_output = True)

    if r.returncode != 0:
        if not poolErr:
            poolErr = True
            print(f'{ red }\n\nCommand "{ " ".join(args) }" failed with exit code { str(r.returncode) }:{ white }')
            print(f'{ red }\nstderr:{ " None" * (not len(r.stderr)) }{ white }\n{ r.stderr }')
            print(f'{ red }\nstdout:{ " None" * (not len(r.stdout)) }{ white }\n{ r.stdout }')
            avlThrs = 999999
            exit(r.returncode)
        else:
            print('Alloy: An error occurred. Thread stopped')
    # else:
        # print(r.stdout) #TODO



def needsRebuild(o, s):
    return (not os.path.exists(o)) or os.path.getmtime(s) > os.path.getmtime(o)


def needsRebuildCPP(o, s, FLG):
    if not os.path.exists(o): return True
    for h in subprocess.run(['g++', '-M', s] + FLG, capture_output = True, text = True).stdout.strip().replace('\\\n ','').split(' ')[1:]:
        if(needsRebuild(o, h)): return True
    return False











def BuildInit(CPP, initOut):
    cpp:list = []
    for s, o in zip(CPP, initOut):
        checkCmd(['python3', 'Tools/Build/GenInitializers.py', s, o])





def BuildGSI1(i, ov, oi, s, isEngine, tot, thrIndex:int):
    global EtoA
    global poolMutex
    global avlThrs
    global curThr

    if needsRebuild(ov, s) or needsRebuild(oi, s) or needsRebuild(oi.replace('cpp', 'hpp'), s):
        while curThr < thrIndex: time.sleep(0.01)
        print(f'{ progress(i + 1, tot) } Compiling shader { ov }')
        print(f'{ progress(i + 1, tot) } Generating shader interface for { s }')
        poolMutex.acquire()
        curThr += 1
        poolMutex.release()
        checkCmd(['glslangValidator', '-V', s, '-o', ov ])
        checkCmd(['python3', 'Tools/Build/GenInterfaces.py', s, EtoA, str(isEngine)])
    else:
        while curThr < thrIndex: time.sleep(0.01)
        print(f'{ progress(i + 1, tot) } Target is up to date (Shader { ov })')
        print(f'{ progress(i + 1, tot) } Target is up to date (Interface of shader { s })')
        poolMutex.acquire()
        curThr += 1
        poolMutex.release()

    poolMutex.acquire()
    avlThrs += 1
    poolMutex.release()




def BuildGSI(SPV, GSI, GLS, isEngine):
    global poolMutex
    global avlThrs
    global totThrs
    global curThr
    global poolErr

    for i, (ov, oi, s) in enumerate(zip(SPV, GSI, GLS)):
        t = threading.Thread(target = BuildGSI1, args = (i, ov, oi, s, isEngine, len(GLS), i), daemon = True)
        t.start()
        poolMutex.acquire()
        avlThrs -= 1
        poolMutex.release()
        while avlThrs == 0: time.sleep(0.01)
        if poolErr:
            sys.exit('Alloy: An error occurred. Build stopped') #TODO add warning output
    while avlThrs < totThrs: time.sleep(0.01)
    if poolErr:
        sys.exit('Alloy: An error occurred. Build stopped') #TODO add warning output
    curThr = 0






def BuildOBJ1(EXEC, FLG, i, o, s, tot, thrIndex:int):
    global poolMutex
    global avlThrs
    global curThr

    if needsRebuildCPP(o, s, FLG):
        while curThr < thrIndex: time.sleep(0.01)
        print(f'{ progress(i + 1, tot) } Compiling object file { o }')
        poolMutex.acquire()
        curThr += 1
        poolMutex.release()
        checkCmd([EXEC] + FLG + ['-fdiagnostics-color', '-c', '-xc++', s, '-o', o ])
    else:
        while curThr < thrIndex: time.sleep(0.01)
        print(f'{ progress(i + 1, tot) } Target is up to date (Object file { o })')
        poolMutex.acquire()
        curThr += 1
        poolMutex.release()

    poolMutex.acquire()
    avlThrs += 1
    poolMutex.release()




def BuildOBJ(EXEC, FLG, OBJ, CPP, defineTuUuid = False, tuUuidPrefix = ''):
    global poolMutex
    global avlThrs
    global totThrs
    global curThr
    global poolErr

    for i, (o, s) in enumerate(zip(OBJ, CPP)):
        t = threading.Thread(target = BuildOBJ1, args = ( #FIXME automatize uuid prefix
            EXEC, FLG + [f'-DTU_UUID={ tuUuidPrefix }0x{ hex(i)[2:].zfill(8) }'] if defineTuUuid else FLG, i, o, s, len(CPP), i
        ), daemon = True)
        t.start()
        poolMutex.acquire()
        avlThrs -= 1
        poolMutex.release()
        while avlThrs == 0: time.sleep(0.01)
        if poolErr:
            sys.exit('Alloy: An error occurred. Build stopped') #TODO add warning output
    while avlThrs < totThrs: time.sleep(0.01)
    if poolErr:
        sys.exit('Alloy: An error occurred. Build stopped') #TODO add warning output
    curThr = 0









# ----------------------------------------- SETUP ------------------------------------------- #








def dirs(EOUT:str, AOUT:str):
    # Create directories for generated files
    os.makedirs(exist_ok = True, name = f'{ EtoA }/.engine/src/Generated')
    os.makedirs(exist_ok = True, name =                f'./src/Generated')
    os.makedirs(exist_ok = True, name = f'{ EtoA }/.engine/src/Generated/Shaders')
    os.makedirs(exist_ok = True, name =                f'./src/Generated/Shaders')

    os.makedirs(exist_ok = True, name = f'./src/Generated/.init')
    os.makedirs(exist_ok = True, name = f'./src/Generated/.init/Shaders')


    # Create output directories
    os.makedirs(exist_ok = True, name = f'{ EOUT }')
    os.makedirs(exist_ok = True, name = f'{ EOUT }/Shaders')
    os.chdir(EtoA)
    os.makedirs(exist_ok = True, name = f'{ AOUT }')
    os.makedirs(exist_ok = True, name = f'{ AOUT }/Shaders')
    os.chdir(AtoE)








def build(
    EXEC:str, OUTPUT:str, LINK:list,  # Path to C++ compiler  # Binary output suffix          # Linker options
    EFLG:list, EGLS:list, ECPP:list,  # Engine flags          # Engine GLS source files       # Engine C++ source files
    AFLG:list, AGLS:list, ACPP:list   # Application flags     # Application GLS source files  # Application C++ source files
):
    LINK += [                                           # Default Likner options    #! Relative to application
        '-L/usr/lib64', '-L/lib64',                         # Prefer 64bit libraries                        #TODO fix windows build
        '-ldl', '-lrt', '-lXrandr', '-lXi',                 # Link dependencies                             #TODO fix windows build
        '-lXcursor', '-lXinerama', '-lX11',                 # Link dependencies                             #TODO fix windows build
        '-lvulkan', '-Bstatic', '-lglfw',                   # Link Vulkan dynamically and GLFW statically   #TODO fix windows build
    ]

    SFLG = [                                            # Shared default C++ flags
        '-std=c++20', '-m64', '-pthread',                   # Use C++20, build for 64bit environments, use pthread
        f'-DenginePath="{ AtoE }"',                         # Engine path macro #FIXME
    ]


    EFLG += SFLG + [                                    # Append default flags to user defined flags    #! Passed by the wrapper
        '-include', 'src/Lynx/Core/InitList.hpp',           # Include generated engine initializers     #! Relative to engine
        '-include', 'src/Lynx/Core/VkDef.hpp',              # Include forced vulkan macros              #! Relative to engine
        '-include', 'src/Lynx/Lynx_config.hpp',             # Include engine configuration macros       #! Relative to engine
        '-Isrc',                                            # Include from src directory                #! Relative to engine
        f'-ffile-prefix-map={ os.path.abspath(EtoA) }/={ AtoE }/' # Fix file prefix in debug infos
    ]

    AFLG += SFLG + [                                    # Append default flags to user defined flags    #! Relative to application   #! Passed by the wrapper
        '-include', f'{ AtoE }/src/Lynx/Core/InitList.hpp', # Include generated engine initializers     #!Relative to application
        '-include', f'{ AtoE }/src/Lynx/Core/VkDef.hpp',    # Include forced vulkan macros              #!Relative to application
        '-include', f'{ AtoE }/src/Lynx/Lynx_config.hpp',   # Include engine configuration macros       #!Relative to application
        '-I.', f'-I{ AtoE }/src', f'-I./.engine/src',       # Include from src directories and app root #!Relative to application
        f'-ffile-prefix-map={ os.path.abspath(EtoA) }/='    # Fix file prefix in debug infos
    ]

    EOUT = f'{ EtoA }/.engine/bin/Lnx/{ OUTPUT }'       # Path to the engine binary output directory        #! Relative to engine
    AOUT = f'./.engine/bin/App/{ OUTPUT }'              # Path to the application binary output directory   #! Relative to application
    ELIB = f'{ EOUT }/libLynxEngine.a'                  # Path to the engine static library                 #! Relative to engine
    ABIN = f'./tmp.out'                                 # Path to the application executable file           #! Relative to application


    ESPV = list((        f'./src/Generated/Shaders/{ Path(s).stem }.spv')     for s in EGLS) # Output .spv files paths
    ASPV = list((f'./.engine/src/Generated/Shaders/{ Path(s).stem }.spv')     for s in AGLS) # Output .spv files paths                   #! Relative to application    #! Can be empty
    EGSI = list((        f'./src/Generated/Shaders/{ Path(s).stem }.gsi.cpp') for s in EGLS) # Generated shader interfaces source files
    AGSI = list((f'./.engine/src/Generated/Shaders/{ Path(s).stem }.gsi.cpp') for s in AGLS) # Generated shader interfaces source files  #! Relative to application    #! Can be empty

    EGSO = list((f'{ EOUT }/Shaders/{ Path(s).stem }.gsi.o') for s in EGLS) # Generated shader interfaces .o files
    AGSO = list((f'{ AOUT }/Shaders/{ Path(s).stem }.gsi.o') for s in AGLS) # Generated shader interfaces .o files                       #! Relative to application
    EOBJ = list((f'{ EOUT }'     f'/{ Path(s).stem }.o')     for s in ECPP) # Output .o files of the non-generated C++ source files
    AOBJ = list((f'{ AOUT }'     f'/{ Path(s).stem }.o')     for s in ACPP) # Output .o files of the non-generated C++ source files      #! Relative to application

    # for i in range(0, A)



    # Create missing directories
    print(f'Using { totThrs } threads')
    print(f'Creating output directories')
    dirs(EOUT, AOUT)
    print(f'\n')


    # Build libraries
    print(f'Generating engine files')
    BuildGSI(SPV = ESPV, GSI = EGSI, GLS = EGLS, isEngine = True)
    print(f'{ bgreen }Engine files generated successfully\n{ white }')

    initOut =\
        list(f'src/Generated/.init'     f'/{ Path(o).stem }.init.hpp' for o in ECPP) +\
        list(f'src/Generated/.init/Shaders/{ Path(o).stem }.init.hpp' for o in EGSI)
    BuildInit(ECPP + EGSI, initOut)

    print(f'Compiling engine source files')
    BuildOBJ(EXEC = EXEC, FLG = EFLG, OBJ = EGSO + EOBJ, CPP = EGSI + ECPP, defineTuUuid = True, tuUuidPrefix = 'e')
    print(f'{ bgreen }Engine source files compiled successfully\n{ white }')

    print(f'Writing Lynx Engine library')
    subprocess.run(['ar', '-rcs', ELIB] + EGSO + EOBJ)
    print(f'{ bgreen }Created "{ ELIB }"\n{ white }')




    # Build executable
    ELIB2 = os.path.relpath(ELIB, EtoA)
    os.chdir(EtoA)

    if len(AGLS) > 0:
        print(f'Generating application files')
        BuildGSI(SPV = ASPV, GSI = AGSI, GLS = AGLS, isEngine = False)
        print(f'{ bgreen }Application files generated successfully"\n{ white }')

    if len(ACPP) > 0:
        print(f'Compiling application source files')
        BuildOBJ(EXEC = EXEC, FLG = AFLG, OBJ = AGSO + AOBJ, CPP = AGSI + ACPP, defineTuUuid = True, tuUuidPrefix = 'a')
        print(f'{ bgreen }Application source files compiled successfully\n{ white }')

    print(f'Writing application executable file')
    checkCmd([EXEC] + AFLG + AGSO + AOBJ + [ELIB2] + LINK + ['-o', ABIN])
    if poolErr:
        sys.exit('Alloy: An error occurred. Build stopped') #TODO add warning output
    print(f'{ bgreen }Created "{ ABIN }"\n{ white }')

    os.chdir(AtoE)



    print('Build completed successfully')
    return 0








# ----------------------------------------- CLEAR ------------------------------------------------- #








def eclear():
    return not(
            not subprocess.run(['find', '.', '-type', 'f', '-wholename', './Lnx/*.o', '-delete'], cwd = f'{ EtoA }/.engine/bin').returncode
        and not subprocess.run(['find', '.', '-type', 'f', '-wholename', './Lnx/*.a', '-delete'], cwd = f'{ EtoA }/.engine/bin').returncode
        and not subprocess.run(['find', '.', '-type', 'f', '-wholename', './*.spv',     '-delete'], cwd = './src/Generated').returncode
        and not subprocess.run(['find', '.', '-type', 'f', '-wholename', './*.cpp',     '-delete'], cwd = './src/Generated').returncode
        and not subprocess.run(['find', '.', '-type', 'f', '-wholename', './*.hpp',     '-delete'], cwd = './src/Generated').returncode
    )


def aclear():
    return not(
            not subprocess.run(['find', '.', '-type', 'f', '-wholename', './App/*.o', '-delete'], cwd = f'{ EtoA }/.engine/bin').returncode
        and not subprocess.run(['find', '.', '-type', 'f', '-wholename', './*.spv',     '-delete'], cwd = f'{ EtoA }/.engine/src/Generated').returncode
        and not subprocess.run(['find', '.', '-type', 'f', '-wholename', './*.cpp',     '-delete'], cwd = f'{ EtoA }/.engine/src/Generated').returncode
        and not subprocess.run(['find', '.', '-type', 'f', '-wholename', './*.hpp',     '-delete'], cwd = f'{ EtoA }/.engine/src/Generated').returncode
    )


def clear():
    return not(not aclear() and not eclear())




if sys.argv[-1] in ['clear', 'aclear', 'eclear']:
    sys.exit(eval(f'{ sys.argv[-1] }()'))

elif sys.argv[-1] == 'build':
    sys.exit(eval(f'{ sys.argv[-1] }({ ",".join(sys.argv[1:-1]) })'))

else:
    sys.exit(f'Unknown task "{ sys.argv[-1] }"')