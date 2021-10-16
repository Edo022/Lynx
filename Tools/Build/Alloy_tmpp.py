import os, sys, subprocess, threading, multiprocessing, time, re, pathlib as pl
from Paths import *
import Utils



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


def needsRebuildInit(s, flags):
    # Remove includes
    tmp = f'{ EtoA }/.engine/.tmp/init-{ os.path.basename(s) }'
    with open(s, 'r') as fs, open(tmp, 'w') as fo:
        fo.write(re.sub(r'#.*include.*(?:"|<).*(?:>|").*', r'\n', fs.read()))

    # Get used includes and check if the init macros are in them
    macros = subprocess.run(['g++', '-dU', '-E', *flags, '-xc++', tmp], capture_output = True, text = True).stdout
    return (re.search(r'#define _lnx_init(?:_fun|(?:_var(?:_value|_array)(?:_const)?))_def\(', macros) != None, tmp)



def needsRebuildCPP(o, s, FLG, forced_includes):
    if not os.path.exists(o): return True
    for h in forced_includes + subprocess.run(['g++', '-MM', s] + FLG, capture_output = True, text = True).stdout.strip().replace('\\\n ','').split(' ')[1:]:
        if(needsRebuild(o, h)): return True
    return False











def BuildInit1(i, o, s, flags, tot):
    global EtoA
    global poolMutex
    global avlThrs
    global curThr
    cpp:list = []

    while curThr < i: time.sleep(0.01)
    r = needsRebuildInit(s, flags)
    if r[0]:
        print(f'{ progress(i + 1, tot) } Creating initializer header { o }')
        poolMutex.acquire()
        curThr += 1
        poolMutex.release()
        checkCmd(['Tools/Build/Generators/GenInitializers', r[1], o, str(flags)])
    else:
        print(f'{ progress(i + 1, tot) } Initializer header { o } is not required')
        poolMutex.acquire()
        curThr += 1
        poolMutex.release()

    poolMutex.acquire()
    avlThrs += 1
    poolMutex.release()



def BuildInit(CPP, outputs, flags):
    global poolMutex
    global avlThrs
    global totThrs
    global curThr
    global poolErr

    for i, (o, s) in enumerate(zip(outputs, CPP)):
        t = threading.Thread(target = BuildInit1, args = (i, o, s, flags, len(CPP)), daemon = True)
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






def BuildGSI1(i, ov, oi, og, s, isEngine, tot):
    global EtoA
    global poolMutex
    global avlThrs
    global curThr

    if needsRebuild(ov, s) or needsRebuild(oi, s) or needsRebuild(oi.replace('cpp', 'hpp'), s):
        while curThr < i: time.sleep(0.01)
        print(f'{ progress(i * 3 + 1 + 0, tot * 3) } Generating GLSL source file { og }')
        print(f'{ progress(i * 3 + 1 + 1, tot * 3) } Compiling shader { ov }')
        print(f'{ progress(i * 3 + 1 + 2, tot * 3) } Generating shader interface for { s }')
        poolMutex.acquire()
        curThr += 1
        poolMutex.release()
        checkCmd(['python3', 'Tools/Build/Generators/GenGlsl.py', s, og]) #FIXME use executable
        checkCmd(['glslangValidator', '-V', og, '-o', ov ])
        checkCmd(['Tools/Build/Generators/GenInterfaces', s, oi, EtoA, str(isEngine)])
    else:
        while curThr < i: time.sleep(0.01)
        print(f'{ progress(i + 1, tot) } Target is up to date (Shader { ov })')
        print(f'{ progress(i + 1, tot) } Target is up to date (Interface of shader { s })')
        poolMutex.acquire()
        curThr += 1
        poolMutex.release()

    poolMutex.acquire()
    avlThrs += 1
    poolMutex.release()




def BuildGSI(SPV, GSI, GLS, ILS, isEngine):
    global poolMutex
    global avlThrs
    global totThrs
    global curThr
    global poolErr

    for i, (ov, oi, og, s) in enumerate(zip(SPV, GSI, GLS, ILS)):
        t = threading.Thread(target = BuildGSI1, args = (i, ov, oi, og, s, isEngine, len(GLS)), daemon = True)
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






def BuildOBJ1(EXEC, FLG, i, o, s, forced_includes, tot):
    global poolMutex
    global avlThrs
    global curThr

    if needsRebuildCPP(o, s, FLG, forced_includes):
        while curThr < i: time.sleep(0.01)
        print(f'{ progress(i + 1, tot) } Compiling object file { o }')
        poolMutex.acquire()
        curThr += 1
        poolMutex.release()
        checkCmd([EXEC] + FLG + ['-fdiagnostics-color', '-c', '-xc++', s, '-o', o ])
    else:
        while curThr < i: time.sleep(0.01)
        print(f'{ progress(i + 1, tot) } Target is up to date (Object file { o })')
        poolMutex.acquire()
        curThr += 1
        poolMutex.release()

    poolMutex.acquire()
    avlThrs += 1
    poolMutex.release()




def BuildOBJ(EXEC, FLG, OBJ, CPP, forced_includes, defineTuUuid = False, tuUuidPrefix = ''):
    global poolMutex
    global avlThrs
    global totThrs
    global curThr
    global poolErr

    for i, (o, s) in enumerate(zip(OBJ, CPP)):
        t = threading.Thread(target = BuildOBJ1, args = ( #FIXME automatize uuid prefix
            EXEC, FLG + [f'-DTU_UUID={ tuUuidPrefix }0x{ hex(i)[2:].zfill(8) }'] if defineTuUuid else FLG, i, o, s, forced_includes, len(CPP)
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
    os.makedirs(exist_ok = True, name = f'{ EtoA }/.engine/.tmp')
    os.makedirs(exist_ok = True, name = f'{ EtoA }/.engine/.tmp/Shaders')
    os.makedirs(exist_ok = True, name = f'./src/Generated')
    os.makedirs(exist_ok = True, name = f'./src/Generated/Shaders')
    os.makedirs(exist_ok = True, name = f'./src/Generated/.init')
    os.makedirs(exist_ok = True, name = f'./src/Generated/.init/Shaders')
    os.makedirs(exist_ok = True, name = f'{ EOUT }')
    os.makedirs(exist_ok = True, name = f'{ EOUT }/Shaders')


def aDirs(AOUT:str):
    os.makedirs(exist_ok = True, name = f'./.engine/.src/Generated')
    os.makedirs(exist_ok = True, name = f'./.engine/.src/Generated/Shaders')
    os.makedirs(exist_ok = True, name = f'{ AOUT }')
    os.makedirs(exist_ok = True, name = f'{ AOUT }/Shaders')








def eBuild(EXEC:str, EOUT:str, ELIB:str, eData:dict):
    eData['include_paths']   += ['-Isrc']
    eData['compiler_flags']  += [f'-ffile-prefix-map={ os.path.abspath(".") }/={ AtoE }/']
    eData['forced_includes'] += [
        'src/Lynx/Core/InitList.hpp',   # Include generated engine initializers
        'src/Lynx/Core/VkDef.hpp',      # Include forced vulkan macros
        'src/Lynx/Lynx_config.hpp',     # Include engine configuration macros
    ]
    EFLG = eData['defines'] + list(Utils.prefixList('-include', eData['forced_includes'])) + eData['include_paths'] + eData['compiler_flags']

    EILS = eData['ils']                                                                        # GLS source files
    EGLS = list(f'{ EtoA }/.engine/.tmp/glsl-{ os.path.basename(s) }.comp'    for s in EILS)   # GLS source files
    ESPV = list(   f'./src/Generated/Shaders/{ os.path.basename(s) }.spv'     for s in EGLS)   # Output .spv files paths
    EGSI = list(   f'./src/Generated/Shaders/{ os.path.basename(s) }.cpp' for s in EILS)   # Generated shader interfaces source files
    EGSO = list(          f'{ EOUT }/Shaders/{ os.path.basename(s) }.o'       for s in EGSI)   # Generated shader interfaces .o files

    ECPP = eData['cpp']                                             # C++ source files
    EOBJ = list((f'{ EOUT }/{ pl.Path(s).stem }.o') for s in ECPP)  # Output .o files of the non-generated C++ source files




    # Build libraries
    print(f'Generating engine files')
    BuildGSI(SPV = ESPV, GSI = EGSI, GLS = EGLS, ILS = EILS, isEngine = True)
    print(f'{ bgreen }Engine files generated successfully\n{ white }')


    print(f'Generating initializer headers')
    initOutputs =\
        list(f'src/Generated/.init'     f'/{ pl.Path(o).stem }.init.hpp' for o in ECPP) +\
        list(f'src/Generated/.init/Shaders/{ pl.Path(o).stem }.init.hpp' for o in EGSI)
    BuildInit(ECPP + EGSI, initOutputs, ['-include', 'src/Lynx/Lynx_config.hpp', '-include', 'src/Lynx/Core/Init.hpp', *eData['defines'], *eData['include_paths']])
    print(f'{ bgreen }Initializer headers generated successfully\n{ white }')


    print(f'Compiling engine source files')
    BuildOBJ(EXEC = EXEC, FLG = EFLG, OBJ = EGSO + EOBJ, CPP = EGSI + ECPP, forced_includes = eData['forced_includes'], defineTuUuid = True, tuUuidPrefix = 'e')
    print(f'{ bgreen }Engine source files compiled successfully\n{ white }')


    print(f'Writing Lynx Engine library')
    subprocess.run(['ar', '-rcs', ELIB] + EGSO + EOBJ)
    print(f'{ bgreen }Created "{ ELIB }"\n{ white }')


    return 0








def aBuild(EXEC:str, EOUT:str, AOUT:str, ELIB:str, eData:dict, aData:dict):
    # Parse arguments
    aData['include_paths']   += [f'-I{ AtoE }/src', f'-I.engine/.src']
    aData['compiler_flags']  += [f'-ffile-prefix-map={ os.path.abspath(EtoA) }/=']
    aData['forced_includes'] += [
        f'{ AtoE }/src/Lynx/Core/InitList.hpp',     # Include generated engine initializers
        f'{ AtoE }/src/Lynx/Core/VkDef.hpp',        # Include forced vulkan macros
        f'{ AtoE }/src/Lynx/Lynx_config.hpp',       # Include engine configuration macros
    ]
    AFLG = aData['defines'] + list(Utils.prefixList('-include', aData['forced_includes'])) + aData['include_paths'] + aData['compiler_flags']

    AILS = aData['ils']                                                                             # ILS source files
    AGLS = list(             f'./.engine/.tmp/glsl-{ os.path.basename(s) }.comp'    for s in AILS)  # Generated GLS source files                #! Can be empty
    ASPV = list(f'./.engine/.src/Generated/Shaders/{ os.path.basename(s) }.spv'     for s in AGLS)  # Output .spv files paths                   #! Can be empty
    AGSI = list(f'./.engine/.src/Generated/Shaders/{ os.path.basename(s) }.cpp' for s in AILS)  # Generated shader interfaces source files  #! Can be empty
    AGSO = list(                f'{ AOUT }/Shaders/{ os.path.basename(s) }.o'       for s in AGSI)  # Generated shader interfaces .o files

    ACPP = aData['cpp']                                             # C++ source files
    AOBJ = list((f'{ AOUT }/{ pl.Path(s).stem }.o') for s in ACPP)  # Output .o files of the non-generated C++ source files

    LINK = eData['linker_flags'] + aData['linker_flags']    # Linker flags
    ABIN = aData['output'][0]                               # Path to the application executable file



    # Build executable

    if len(AILS) > 0:
        print(f'Generating application files')
        BuildGSI(SPV = ASPV, GSI = AGSI, GLS = AGLS, ILS = AILS, isEngine = False)
        print(f'{ bgreen }Application files generated successfully"\n{ white }')


    if len(ACPP) > 0:
        print(f'Compiling application source files')
        BuildOBJ(EXEC = EXEC, FLG = AFLG, OBJ = AGSO + AOBJ, CPP = AGSI + ACPP, forced_includes = aData['forced_includes'], defineTuUuid = True, tuUuidPrefix = 'a')
        print(f'{ bgreen }Application source files compiled successfully\n{ white }')


    print(f'Writing application executable file')
    checkCmd([EXEC] + AFLG + AGSO + AOBJ + [f'{ AtoE }/{ ELIB }'] + LINK + ['-o', ABIN])
    if poolErr:
        sys.exit('Alloy: An error occurred. Build stopped') #TODO add warning output
    print(f'{ bgreen }Created "{ ABIN }"\n{ white }')



    print('Build completed successfully')
    return 0








def build(EXEC, OUTPUT, eData, aData, buildEngine):
    print(f'Using { totThrs } threads')


    EOUT = f'{ EtoA }/.engine/.bin/Lnx/{ OUTPUT }'       # Path to the engine binary output directory        #! Relative to engine
    AOUT =        f'./.engine/.bin/App/{ OUTPUT }'       # Path to the application binary output directory   #! Relative to application
    ELIB = f'{ EOUT }/libLynxEngine.a'                   # Path to the engine static library                 #! Relative to engine


    e = 0
    if buildEngine:
        os.chdir(AtoE)
        eDirs(EOUT)
        e = eBuild(EXEC, EOUT, ELIB, eData)
        os.chdir(EtoA)

    aDirs(AOUT)
    a = aBuild(EXEC, EOUT, AOUT, ELIB, eData, aData)

    return e or a