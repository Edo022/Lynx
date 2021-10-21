import os, subprocess, re, io, token, tokenize, glob, math



# Color codes
nBlk = '\033[0;30m';    bBlk= '\033[1;30m';    uBlk= '"\033[4;30m'
nRed = '\033[0;31m';    bRed= '\033[1;31m';    uRed= '"\033[4;31m'
nGrn = '\033[0;32m';    bGrn= '\033[1;32m';    uGrn= '"\033[4;32m'
nYel = '\033[0;33m';    bYel= '\033[1;33m';    uYel= '"\033[4;33m'
nBlu = '\033[0;34m';    bBlu= '\033[1;34m';    uBlu= '"\033[4;34m'
nMag = '\033[0;35m';    bMag= '\033[1;35m';    uMag= '"\033[4;35m'
nCyn = '\033[0;36m';    bCyn= '\033[1;36m';    uCyn= '"\033[4;36m'
nWht = '\033[0;37m';    bWht= '\033[1;37m';    uWht= '"\033[4;37m'




# Returns the value of vVal rounded by excess to a multiple of vMul
def roundUp(vVal:int, vMul:int):
    r = vVal % vMul
    if r == 0: return vVal
    else:      return vVal + vMul - r




# Makes the first letter of vStr uppercase and returns the string
# This is the equivalent of vStr[0].upper() + vStr[1:]
def capitalize1(vStr:str):
    if len(vStr) > 1: return vStr[0].upper() + vStr[1:]
    else:             return vStr.upper()




# Indents each line of vStr by vIndent tabs and replaces every 4 leading spaces with a single tab
# Extra leading spaces are kept after the tabs
# Returns the resulting string
def fixTabs(vStr:str, vIndent:int = 0):
    ls = vStr.split('\n')
    for i, l in enumerate(ls):
        wsLen:int = 0
        while True:
            r = re.match(r'^((?:(?: {,3})\t)|(?: {4}))', l)
            if r == None: break
            l = l[len(r.group(1)):]
            wsLen += 1
        ls[i] = '\t' * (wsLen + vIndent * (len(l) > 0)) + l

    return '\n'.join(ls)




# Adds an element with the vPrefix type and value before each element of vList
def prefixList(vPrefix, vList:list):
    for e in vList:
        yield(vPrefix)
        yield(e)




# Adds an element with the vPostfix type and value after each element of vList
def postfixList(vPostfix, vList:list):
    for e in vList:
        yield(e)
        yield(vPrefix)




# Reads a glob path and returns a list containing the expanded paths
# If the path is not a glob, the return list contains the original path
def expGlob(vPath:str, vStart:str = '.'):
    cwd = os.getcwd()       # Save cwd
    os.chdir(vStart)         # Cd to vStart
    g = glob.glob(vPath)     # Get glob results
    if len(g) > 0:          # If it was a glob
        return g                # Return the list of paths
    else:                   # If it was not
        return [vPath]           # Return a list containing the original path only
    os.chdir(cwd)           # Cd to caller cwd





#TODO REMOVE
# Reads a GLS source file
# Removes any comment and useless withespace near operators and replaces any combination of withespace characters with a single space
# Newlines characters after preprocessor directives are preserved
# Returns the result as a string. The input code must contain no comments
def clearGls(vCode:str):
    return (
        re.sub(r'([-+&|])''\x07',  r'\g<1> \g<1>',                  # Prevent ++ - like  operators from being merged #! GLSL has no pointers. * * is a syntax error
        re.sub(r' ?([()\[\]{}+*-\/.!<>=&^|?:%,;])(?: )?', r'\g<1>', # Remove spaces near opeartors
        re.sub(r'([-+&|]) \1',     r'\g<1>''\x07',                  # Prevent ++ - like  operators from being merged
        re.sub('\x07',             r'\n',                           # Preserve newlines after preprocessor directives
        re.sub(r'[ \t\v\r]+',      r' ',                            # Trim whitespace
        re.sub(r'\n',              r' ',                            # Remove normal newlines
        re.sub(r'(#.*?)\n',        r'\g<1>''\x07',                  # Preserve newlines after preprocessor directives
    vCode))))))))






# Reads a Lynx build file
# Removes any useless withespace near curly braces and replaces any combination of withespace characters with a single space
# Returns the result as a string. The input code must contain no comments
def tokenizeBuildFile(vCode:str):
    for t in tokenize.generate_tokens(io.StringIO(vCode).readline):
        if t.type not in [token.INDENT, token.DEDENT, token.NL, token.ENDMARKER, token.NEWLINE, token.COMMENT]:
            yield(t.string)



#TODO REMOVE
# Reads a GLS file and returns the preprocessed output as a string
# Comments are removed
def preprocessGls(vFile:str):
    return subprocess.run(
        ['glslangValidator', vFile, '-S', 'comp', '-E'],
        capture_output = True, text = True
    ).stdout






# Reads a C++ file and returns the preprocessed output as a string
# Comments are removed
def preprocessCpp(vFile:str, vFlags:list):
    return subprocess.run(
        ['g++', *vFlags, '-E', '-xc++', vFile],
        capture_output = True, text = True
    ).stdout


#TODO REMOVE
# Reads a C file and returns the preprocessed output as a string
# Comments are removed
def preprocessC(vFile:str, vFlags:list):
    return subprocess.run(
        ['g++', *vFlags, '-E', '-xc', vFile],
        capture_output = True, text = True
    ).stdout
