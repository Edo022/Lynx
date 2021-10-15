import os, subprocess, re, io, token, tokenize, glob








def capitalize1(s:str):
    if len(s) > 1:
        return s[0].upper() + s[1:]
    else:
        return s.upper()




def fixTabs(s:str, indent:int = 0):
    ls = s.split('\n')
    for i, l in enumerate(ls):
        wsLen:int = 0
        while True:
            r = re.match(r'^((?:(?: {,3})\t)|(?: {4}))', l)
            if r == None: break
            l = l[len(r.group(1)):]
            wsLen += 1
        ls[i] = '\t' * (wsLen + indent * (len(l) > 0)) + l

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






# Reads a GLS source file
# Removes any useless withespace near operators and replaces any combination of withespace characters with a single space
# Newlines characters after preprocessor directives are preserved
# Returns the result as a string. The input code must contain no comments
def clearGls(vCode:str):
    return (
        re.sub(r'([-+])''\x07',  r'\g<1> \g<1>',                    # Prevent - - and + + from being merged #! GLSL has no pointers. * * is a syntax error
        re.sub(r' ?([()\[\]{}+*-\/.!<>=&^|?:%,;])(?: )?', r'\g<1>', # Remove spaces near opeartors
        re.sub(r'([-+]) \1',     r'\g<1>''\x07',                    # Prevent - - and + + from being merged
        re.sub(r'\\n',           r'\n',                             # Remove newlines
        re.sub(r'\n',            r'',                               # Remove newlines
        re.sub(r'(#.*?)\n',      r'\g<1>\\n',                       # Remove newlines
        re.sub(r'( |\t|\v|\r)+', r' ',                              # Trim whitespace
    vCode))))))))




# Reads a GLS source file
# Removes any useless withespace near curly braces and replaces any combination of withespace characters with a single space
# Returns the result as a string. The input code must contain no comments
def clearBuild(vCode:str):
    for t in tokenize.generate_tokens(io.StringIO(vCode).readline):
        if t.type not in [token.INDENT, token.DEDENT, token.NL, token.ENDMARKER, token.NEWLINE, token.COMMENT]:
            yield(t.string)




# Reads a GLS file and returns the preprocessed output as a string
# Comments are removed
def preprocessGls(vFile:str):
    return subprocess.run(
        ['glslangValidator', vFile, '-E'],
        capture_output = True, text = True
    ).stdout






# Reads a C++ file and returns the preprocessed output as a string
# Comments are removed
def preprocessCpp(file:str, flags:list):
    return subprocess.run(
        ['g++', *flags, '-E', '-xc++', file],
        capture_output = True, text = True
    ).stdout
