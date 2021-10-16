import os, sys, subprocess, threading, multiprocessing, time, re, pathlib as pl
from Paths import *
import Utils



poolMutex = threading.Lock()
avlThrs = multiprocessing.cpu_count() - 1
totThrs = avlThrs
curThr = 0
poolErr = False

bmag   = '\033[1;35m'
bgreen = '\033[1;32m'
white  = '\033[0;37m'
red    = '\033[1;31m'








# Helper function --------------------------------------------------------------------------------------------------------------------------#








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
    t = f'{ EtoA }/.engine/.tmp/init-{ os.path.basename(s) }'
    with open(s, 'r') as fs, open(t, 'w') as fo:
        fo.write(re.sub(r'#.*include.*(?:"|<).*(?:>|").*', r'\n', fs.read()))

    # Get used includes and check if the init macros are in them
    macros = subprocess.run(['g++', '-dU', '-E', *flags, '-xc++', t], capture_output = True, text = True).stdout
    return (re.search(r'#define _lnx_init(?:_fun|(?:_var(?:_value|_array)(?:_const)?))_def\(', macros) != None, t)


def needsRebuildCpp(o, s, FLG, forced_includes):
    if not os.path.exists(o): return True
    for h in forced_includes + subprocess.run(['g++', '-MM', s] + FLG, capture_output = True, text = True).stdout.strip().replace('\\\n ','').split(' ')[1:]:
        if(needsRebuild(o, h)): return True
    return False








# Automated build --------------------------------------------------------------------------------------------------------------------------#








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








# BuildN helper function
def Build1(vCheckFun, vCheckFunArgs:tuple, vTgt:str, vSrc:str, vCmd:list, vCurBuildMsg:str, vCurUpToDateMsg:str, i:int,  tot:int):
    global poolMutex
    global avlThrs, curThr

    r = vCheckFun(*vCheckFunArgs)
    assert isinstance(r, bool), 'vCheckFun must return a boolean value'

    if r:
        while curThr < i: time.sleep(0.01)
        print(f'{ progress(i + 1, tot) } ' + vCurBuildMsg)
        poolMutex.acquire()
        curThr += 1
        poolMutex.release()
        checkCmd(vCmd)
    else:
        while curThr < i: time.sleep(0.01)
        print(f'{ progress(i + 1, tot) } ' + vCurUpToDateMsg)
        poolMutex.acquire()
        curThr += 1
        poolMutex.release()

    poolMutex.acquire()
    avlThrs += 1
    poolMutex.release()




def BuildN(vStartMsg:str, vCheckFun, vCheckFunArgs:tuple, vTgt:list, vSrc:list, vCmd:list, vCurBuildMsg:str, vCurUpToDateMsg:str, vSuccessMsg:str):
    r"""
        Runs vCmd in a background process for each element of vSrc

        vCheckFun:       The function used to check if the target needs to be built. It must return a boolean value
        vCheckFunArgs:   A tuple containing the arguments to pass to vCheckFun
        vStartMsg:       The message to print before the commands are ran.             %s and %t are passed literally
        vSrc:            A list of strings containing the path to each source file.    %s and %t are passed literally
        vTgt:            A list of strings containing the path to each output file.    %s and %t are passed literally
        vCmd:            A list of strings containing the command and arguments to run
        vCurBuildMsg:    The message to print before building each target
        vCurUpToDateMsg: The message to print if the target is up to date and doesn't need to be built
        vSuccessMsg:     The message to print after all commands have exited with a success code. %s and %t are passed literally

        vSrc and vTgt can be relative or absolute paths and must contain the same number of elements
        arguments and strings in the form %s and %t are replaced with the current source file and target file names
            e.g.  ['g++', '-std=c++20', '-g0', '%s', '-o', '%t']
            e.g.  needsRebuild, (foo, '%t', '%s', bar)
            %s and %t can be escaped with a '\' to use the literal '%s' or '%o' as an argument
    """ #FIXME automatize uuid prefix

    global poolMutex, poolErr
    global avlThrs, totThrs, curThr
    assert isinstance(vStartMsg,       str), f'vStartMsg ({ vStartMsg }) must be a string'
    assert callable(vCheckFun),              f'vCheckFun ({ vCheckFun }) must be callable'
    assert isinstance(vCurBuildMsg,    str), f'vCurBuildMsg ({ vCurBuildMsg }) must be a string'
    assert isinstance(vCurUpToDateMsg, str), f'vCurUpToDateMsg ({ vCurUpToDateMsg }) must be a string'
    assert isinstance(vSuccessMsg,     str), f'vSuccessMsg ({ vSuccessMsg }) must be a string'
    assert len(vSrc) == len(vTgt),           f'vSrc and vTgt must contain the same number of elements (len(vSrc) = { str(len(vSrc)) }, len(vTgt) = { str(len(vSrc)) })'


    print(vStartMsg)
    for i, (t, s) in enumerate(zip(vTgt, vSrc)):
        assert isinstance(s, str),                 f'vSrc ({ s }) must contain strings only'
        assert isinstance(t, str),                 f'vTgt ({ t }) must contain strings only'
        assert os.path.exists(s),                  f'vSrc ({ s }) elements must be a valid path to an existent file'
        assert os.path.exists(os.path.dirname(t)), f'vTgt ({ t }) elements must be a valid path'

        t = threading.Thread(
            target = Build1,
            args = (
                vCheckFun,  list((s if a == '%s' else t if a == '%t' else a for a in vCheckFunArgs)),
                vTgt, vSrc, list((s if a == '%s' else t if a == '%t' else a for a in vCmd)),
                vCurBuildMsg.   replace('%s', s).replace('%t', t),
                vCurUpToDateMsg.replace('%s', s).replace('%t', t),
                i, len(vSrc)
            ),
            daemon = True
        )
        t.start()

        poolMutex.acquire()
        avlThrs -= 1
        poolMutex.release()
        while avlThrs == 0: time.sleep(0.01)
        if poolErr:
            sys.exit('Alloy: An error occurred. Build stopped') #TODO add warning output

    while avlThrs < totThrs: time.sleep(0.01)
    if poolErr: sys.exit('Alloy: An error occurred. Build stopped') #TODO add warning output
    else:       print(vSuccessMsg)
    curThr = 0








# Setup ------------------------------------------------------------------------------------------------------------------------------------#








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








# Build ------------------------------------------------------------------------------------------------------------------------------------#








def eBuild(EXEC:str, EOUT:str, ELIB:str, eData:dict):
    eData['include_paths']   += ['-Isrc']
    eData['compiler_flags']  += [f'-ffile-prefix-map={ os.path.abspath(".") }/={ AtoE }/']
    eData['forced_includes'] += [
        'src/Lynx/Core/InitList.hpp',   # Include generated engine initializers
        'src/Lynx/Core/VkDef.hpp',      # Include forced vulkan macros
        'src/Lynx/Lynx_config.hpp',     # Include engine configuration macros
    ]
    EFLG = eData['defines'] + list(Utils.prefixList('-include', eData['forced_includes'])) + eData['include_paths'] + eData['compiler_flags']

    EILS = eData['ils']                                                                         # GLS source files
    EGLS = list(f'{ EtoA }/.engine/.tmp/glsl-{ os.path.basename(s)     }.comp' for s in EILS)   # GLS source files
    ESPV = list(   f'./src/Generated/Shaders/{ os.path.basename(s)[5:] }.spv'  for s in EGLS)   # Output .spv files paths
    EGSI = list(   f'./src/Generated/Shaders/{ os.path.basename(s)     }.cpp'  for s in EILS)   # Generated shader interfaces source files
    EGSO = list(          f'{ EOUT }/Shaders/{ os.path.basename(s)     }.o'    for s in EGSI)   # Generated shader interfaces .o files

    ECPP = eData['cpp']                                             # C++ source files
    EOBJ = list((f'{ EOUT }/{ pl.Path(s).stem }.o') for s in ECPP)  # Output .o files of the non-generated C++ source files


    if len(EILS) > 0: BuildN(
        'Generating Engine GLSL files',
        needsRebuild, ('%t', '%s'),
        EGLS, EILS,
        ['python3', 'Tools/Build/Generators/GenGlsl.py', '%s', '%t'], #FIXME use executable
        'Generating GLSL source file %t',
        'Target is up to date (GLSL source file %t)',
        f'{ bgreen }Engine GLSL source files generated successfully\n{ white }'
    )

    if len(EILS) > 0: BuildN(
        'Generating Engine shader interfaces',
        needsRebuild, ('%t', '%s'),
        EGSI, EILS,
        ['Tools/Build/Generators/GenInterfaces', '%s', '%t', EtoA, str(True)],
        'Generating shader interface for %s',
        'Target is up to date (Shader interface of %s)',
        f'{ bgreen }Engine shader interfaces generated successfully\n{ white }'
    )

    if len(EGLS) > 0: BuildN(
        'Compiling Engine shaders',
        needsRebuild, ('%t', '%s'),
        ESPV, EGLS,
        ['glslangValidator', '-V', '%s', '-S', 'comp', '-o', '%t' ],
        'Compiling shader %t',
        'Target is up to date (Shader %t)',
        f'{ bgreen }Engine shaders compiled successfully\n{ white }'
    )


    print(f'Generating initializer headers')
    initOutputs =\
        list(f'src/Generated/.init'     f'/{ pl.Path(o).stem }.init.hpp' for o in ECPP) +\
        list(f'src/Generated/.init/Shaders/{ pl.Path(o).stem }.init.hpp' for o in EGSI)
    BuildInit(ECPP + EGSI, initOutputs, ['-include', 'src/Lynx/Lynx_config.hpp', '-include', 'src/Lynx/Core/Init.hpp', *eData['defines'], *eData['include_paths']])
    print(f'{ bgreen }Initializer headers generated successfully\n{ white }')


    BuildN(
        'Compiling Engine object files',
        needsRebuildCpp, ('%t', '%s', EFLG, eData['forced_includes']),
        EGSO + EOBJ, EGSI + ECPP,
        [EXEC, *EFLG, '-fdiagnostics-color', '-c', '-xc++', '%s', '-o', '%t'],
        'Compiling object file %t',
        'Target is up to date (Object file %t)',
        f'{ bgreen }Engine object files compiled successfully\n{ white }'
    )


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

    AILS = aData['ils']                                                                              # ILS source files
    AGLS = list(             f'./.engine/.tmp/glsl-{ os.path.basename(s)     }.comp' for s in AILS)  # Generated GLS source files                #! Can be empty
    ASPV = list(f'./.engine/.src/Generated/Shaders/{ os.path.basename(s)[5:] }.spv'  for s in AGLS)  # Output .spv files paths                   #! Can be empty
    AGSI = list(f'./.engine/.src/Generated/Shaders/{ os.path.basename(s)     }.cpp'  for s in AILS)  # Generated shader interfaces source files  #! Can be empty
    AGSO = list(                f'{ AOUT }/Shaders/{ os.path.basename(s)     }.o'    for s in AGSI)  # Generated shader interfaces .o files

    ACPP = aData['cpp']                                             # C++ source files
    AOBJ = list((f'{ AOUT }/{ pl.Path(s).stem }.o') for s in ACPP)  # Output .o files of the non-generated C++ source files

    LINK = eData['linker_flags'] + aData['linker_flags']    # Linker flags
    ABIN = aData['output'][0]                               # Path to the application executable file




    if len(AILS) > 0: BuildN(
        'Generating Application GLSL files',
        needsRebuild, ('%t', '%s'),
        AGLS, AILS,
        ['python3', 'Tools/Build/Generators/GenGlsl.py', '%s', '%t'], #FIXME use executable
        'Generating GLSL source file %t',
        'Target is up to date (GLSL source file %t)',
        f'{ bgreen }Application GLSL source files generated successfully\n{ white }'
    )

    if len(AILS) > 0: BuildN(
        'Generating Application shader interfaces',
        needsRebuild, ('%t', '%s'),
        AGSI, AILS,
        ['Tools/Build/Generators/GenInterfaces', '%s', '%t', EtoA, str(False)],
        'Generating shader interface for %s',
        'Target is up to date (Shader interface of %s)',
        f'{ bgreen }Application shader interfaces generated successfully\n{ white }'
    )

    if len(AGLS) > 0: BuildN(
        'Compiling Application shaders',
        needsRebuild, ('%t', '%s'),
        ASPV, AGLS,
        ['glslangValidator', '-V', '%s', '-S', 'comp', '-o', '%t' ],
        'Compiling shader %t',
        'Target is up to date (Shader %t)',
        f'{ bgreen }Application shaders compiled successfully\n{ white }'
    )


    BuildN(
        'Compiling Application object files',
        needsRebuildCpp, ('%t', '%s', AFLG, aData['forced_includes']),
        AGSO + AOBJ, AGSI + ACPP,
        [EXEC, *AFLG, '-fdiagnostics-color', '-c', '-xc++', '%s', '-o', '%t'],
        'Compiling object file %t',
        'Target is up to date (Object file of %s)',
        f'{ bgreen }Application object files compiled successfully\n{ white }'
    )



    print(f'Writing application executable file')
    checkCmd([EXEC] + AFLG + AGSO + AOBJ + [f'{ AtoE }/{ ELIB }'] + LINK + ['-o', ABIN])
    if poolErr:
        sys.exit('Alloy: An error occurred. Build stopped') #TODO add warning output
    print(f'{ bgreen }Created "{ ABIN }"\n{ white }')



    print('Build completed successfully')
    return 0








# Main task --------------------------------------------------------------------------------------------------------------------------------#








def build(EXEC, OUTPUT, eData, aData, buildEngine):
    print(f'{ bmag }Working directory: { os.getcwd() }')
    print(f'Using { totThrs } threads{ white }')


    EOUT = f'{ EtoA }/.engine/.bin/Lnx/{ OUTPUT }'       # Path to the engine binary output directory        #! Relative to engine
    AOUT =        f'./.engine/.bin/App/{ OUTPUT }'       # Path to the application binary output directory   #! Relative to application
    ELIB = f'{ EOUT }/libLynxEngine.a'                   # Path to the engine static library                 #! Relative to engine


    e = 0
    if buildEngine:
        print(f'{ bmag }Moving to directory { os.path.abspath(AtoE) }{ white }'); os.chdir(AtoE)
        eDirs(EOUT)
        e = eBuild(EXEC, EOUT, ELIB, eData)
        print(f'{ bmag }Moving to directory { os.path.abspath(EtoA) }{ white }'); os.chdir(EtoA)

    aDirs(AOUT)
    a = aBuild(EXEC, EOUT, AOUT, ELIB, eData, aData)

    return e or a