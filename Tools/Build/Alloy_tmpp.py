import os, sys, subprocess, threading, multiprocessing, time, pathlib as pl
from Paths import *
import Utils



# with open('./.engine/.EtoA', 'r') as f: EtoA = f.read()
# with open('./.engine/.AtoE', 'r') as f: AtoE = f.read()
#os.chdir(AtoE)

poolMutex = threading.Lock()
avlThrs = multiprocessing.cpu_count() - 1
totThrs = avlThrs
curThr = 0
poolErr = False

bgreen = '\033[1;32m'
white = '\033[0;37m'
red = '\033[1;31m'








# ----------------------------------------- BUILD ------------------------------------------- #








def progress(cur:int, tot:int):
    return f"[{ str(cur).rjust(len(str(tot)), '0') }/{ str(tot) } - { '{:3.2f}'.format(cur / tot * 100).rjust(6, '0') }%]"




def checkCmd(args):
    global poolErr
    global avlThrs
    r = subprocess.run(args, text = True, capture_output = True)

    if r.returncode != 0:
        if not poolErr:
            poolErr = True
            print(f'{ red }\n\nCommand "{ " ".join(args) }" failed with exit code { str(r.returncode) }:{ white }')
            print(f'{ red }\nstderr:{ " None" * (not len(r.stderr)) }{ white }\n{ r.stderr }')
            print(f'{ red }\nstdout:{ " None" * (not len(r.stdout)) }{ white }\n{ r.stdout }')
            avlThrs = 999999
            sys.exit(r.returncode)
        else:
            print('Alloy: An error occurred. Thread stopped')
    # else:
        # print(r.stdout) #TODO



def needsRebuild(o, s):
    return (not os.path.exists(o)) or os.path.getmtime(s) > os.path.getmtime(o)


def needsRebuildCPP(o, s, FLG):
    if not os.path.exists(o): return True
    for h in subprocess.run(['g++', '-MM', s] + FLG, capture_output = True, text = True).stdout.strip().replace('\\\n ','').split(' ')[1:]:
        if(needsRebuild(o, h)): return True
    return False











def BuildInit(CPP, outputs, flags):
    cpp:list = []
    for s, o in zip(CPP, outputs):
        checkCmd(['Tools/Build/Generators/GenInitializers', s, o, str(flags)])





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
        checkCmd(['python3', 'Tools/Build/Generators/GenInterfaces.py', s, EtoA, str(isEngine)])
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











def eDirs(EOUT:str):
    os.makedirs(exist_ok = True, name = './src/Generated')
    os.makedirs(exist_ok = True, name = './src/Generated/Shaders')
    os.makedirs(exist_ok = True, name = './src/Generated/.init')
    os.makedirs(exist_ok = True, name = './src/Generated/.init/Shaders')
    os.makedirs(exist_ok = True, name = f'{ EOUT }')
    os.makedirs(exist_ok = True, name = f'{ EOUT }/Shaders')


def aDirs(AOUT:str):
    os.makedirs(exist_ok = True, name = './.engine/src/Generated')
    os.makedirs(exist_ok = True, name = './.engine/src/Generated/Shaders')
    os.makedirs(exist_ok = True, name = f'{ AOUT }')
    os.makedirs(exist_ok = True, name = f'{ AOUT }/Shaders')








def eBuild(EXEC:str, EOUT:str, SFLG:list, ELIB:str, eData:dict):
    eData['include_paths']   += ['-I.', f'-I{ AtoE }/src', f'-I./.engine/src']
    eData['compiler_flags']  += [f'-ffile-prefix-map={ os.path.abspath(EtoA) }/={ AtoE }/']
    eData['forced_includes'] += [
        '-include', 'src/Lynx/Core/InitList.hpp',                                               # Include generated engine initializers
        '-include', 'src/Lynx/Core/VkDef.hpp',                                                  # Include forced vulkan macros
        '-include', 'src/Lynx/Lynx_config.hpp',                                                 # Include engine configuration macros
    ]
    EFLG = SFLG + eData['defines'] + eData['forced_includes'] + eData['include_paths'] + eData['compiler_flags']

    EGLS = eData['gls']                                                                         # GLS source files
    ESPV = list((        f'./src/Generated/Shaders/{ pl.Path(s).stem }.spv')     for s in EGLS) # Output .spv files paths
    EGSI = list((        f'./src/Generated/Shaders/{ pl.Path(s).stem }.gsi.cpp') for s in EGLS) # Generated shader interfaces source files
    EGSO = list((f'{ EOUT }/Shaders/{ pl.Path(s).stem }.gsi.o') for s in EGLS)                  # Generated shader interfaces .o files

    ECPP = eData['cpp']                                                                         # C++ source files
    EOBJ = list((f'{ EOUT }'     f'/{ pl.Path(s).stem }.o')     for s in ECPP)                  # Output .o files of the non-generated C++ source files




    # Build libraries
    print(f'Generating engine files')
    BuildGSI(SPV = ESPV, GSI = EGSI, GLS = EGLS, isEngine = True)
    print(f'{ bgreen }Engine files generated successfully\n{ white }')

    initOutputs =\
        list(f'src/Generated/.init'     f'/{ pl.Path(o).stem }.init.hpp' for o in ECPP) +\
        list(f'src/Generated/.init/Shaders/{ pl.Path(o).stem }.init.hpp' for o in EGSI)
    BuildInit(ECPP + EGSI, initOutputs, eData['defines'] + eData['forced_includes'] + eData['include_paths'])



    print(f'Compiling engine source files')
    BuildOBJ(EXEC = EXEC, FLG = EFLG, OBJ = EGSO + EOBJ, CPP = EGSI + ECPP, defineTuUuid = True, tuUuidPrefix = 'e')
    print(f'{ bgreen }Engine source files compiled successfully\n{ white }')

    print(f'Writing Lynx Engine library')
    subprocess.run(['ar', '-rcs', ELIB] + EGSO + EOBJ)
    print(f'{ bgreen }Created "{ ELIB }"\n{ white }')

    os.chdir(EtoA)

    return 0








def aBuild(EXEC:str, EOUT:str, AOUT:str, SFLG:list, ELIB:str, eData:dict, aData:dict):
    # Parse arguments
    aData['include_paths']   += ['-Isrc']
    aData['compiler_flags']  += [f'-ffile-prefix-map={ os.path.abspath(EtoA) }/=']
    aData['forced_includes'] += [
        '-include', f'{ AtoE }/src/Lynx/Core/InitList.hpp',                                     # Include generated engine initializers
        '-include', f'{ AtoE }/src/Lynx/Core/VkDef.hpp',                                        # Include forced vulkan macros
        '-include', f'{ AtoE }/src/Lynx/Lynx_config.hpp',                                       # Include engine configuration macros
    ]
    AFLG = SFLG + aData['defines'] + aData['forced_includes'] + aData['include_paths'] + aData['compiler_flags']

    AGLS = aData['gls']                                                                         # GLS source files
    ASPV = list((f'./.engine/src/Generated/Shaders/{ pl.Path(s).stem }.spv')     for s in AGLS) # Output .spv files paths                  #! Can be empty
    AGSI = list((f'./.engine/src/Generated/Shaders/{ pl.Path(s).stem }.gsi.cpp') for s in AGLS) # Generated shader interfaces source files #! Can be empty
    AGSO = list((f'{ AOUT }/Shaders/{ pl.Path(s).stem }.gsi.o') for s in AGLS)                  # Generated shader interfaces .o files

    ACPP = aData['cpp']                                                                         # C++ source files
    AOBJ = list((f'{ AOUT }'     f'/{ pl.Path(s).stem }.o')     for s in ACPP)                  # Output .o files of the non-generated C++ source files

    LINK = eData['linker_flags'] + aData['linker_flags']                                        # Linker flags
    ABIN = aData['output'][0]                                                                   # Path to the application executable file



    # Build executable

    if len(AGLS) > 0:
        print(f'Generating application files')
        BuildGSI(SPV = ASPV, GSI = AGSI, GLS = AGLS, isEngine = False)
        print(f'{ bgreen }Application files generated successfully"\n{ white }')

    if len(ACPP) > 0:
        print(f'Compiling application source files')
        BuildOBJ(EXEC = EXEC, FLG = AFLG, OBJ = AGSO + AOBJ, CPP = AGSI + ACPP, defineTuUuid = True, tuUuidPrefix = 'a')
        print(f'{ bgreen }Application source files compiled successfully\n{ white }')

    print(f'Writing application executable file')
    checkCmd([EXEC] + AFLG + AGSO + AOBJ + [f'{ AtoE }/{ ELIB }'] + LINK + ['-o', ABIN])
    if poolErr:
        sys.exit('Alloy: An error occurred. Build stopped') #TODO add warning output
    print(f'{ bgreen }Created "{ ABIN }"\n{ white }')

    os.chdir(AtoE)



    print('Build completed successfully')
    return 0








def build(EXEC, OUTPUT, eData, aData, buildEngine):
    print(f'Using { totThrs } threads')


    SFLG = [                                            # Shared default C++ flags
        '-std=c++20', '-m64', '-pthread',               # Use C++20, build for 64bit environments, use pthread
        f'-DenginePath="{ AtoE }"',                     # Engine path macro #FIXME
    ]
    EOUT = f'{ EtoA }/.engine/bin/Lnx/{ OUTPUT }'       # Path to the engine binary output directory        #! Relative to engine
    AOUT =        f'./.engine/bin/App/{ OUTPUT }'       # Path to the application binary output directory   #! Relative to application
    ELIB = f'{ EOUT }/libLynxEngine.a'                  # Path to the engine static library                 #! Relative to engine


    if buildEngine:
        os.chdir(AtoE)
        eDirs(EOUT)
        e = eBuild(EXEC, EOUT, SFLG, ELIB, eData)
        os.chdir(EtoA)

    aDirs(AOUT)
    a = aBuild(EXEC, EOUT, AOUT, SFLG, ELIB, eData, aData)

    return 0 if a == 0 and b == 0 else 1