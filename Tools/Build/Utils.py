import os, subprocess, re, io, token, tokenize, glob



# Reads a glob path and returns a list containing the expanded paths
# If the path is not a glob, the return list contains the original path
def expGlob(path:str, start:str = '.'):
    cwd = os.getcwd()       # Save cwd
    os.chdir(start)         # Cd to start
    g = glob.glob(path)     # Get glob results
    if len(g) > 0:          # If it was a glob
        return g                # Return the list of paths
    else:                   # If it was not
        return [path]           # Return a list containing the original path only
    os.chdir(cwd)           # Cd to caller cwd






# Reads a GLS source file
# Removes any useless withespace near operators and replaces any combination of withespace characters with a single space
# Newlines characters after preprocessor directives are preserved
# Returns the result as a string. The input code must contain no comments
def clearGls(code:str):
    return (
        re.sub(r'([-+])''\x07',  r'\g<1> \g<1>',                    # Prevent - - and + + from being merged #! GLSL has no pointers. * * is a syntax error
        re.sub(r' ?([()\[\]{}+*-\/.!<>=&^|?:%,;])(?: )?', r'\g<1>', # Remove spaces near opeartors
        re.sub(r'([-+]) \1',     r'\g<1>''\x07',                    # Prevent - - and + + from being merged
        re.sub(r'\\n',           r'\n',                             # Remove newlines
        re.sub(r'\n',            r'',                               # Remove newlines
        re.sub(r'(#.*?)\n',      r'\g<1>\\n',                       # Remove newlines
        re.sub(r'( |\t|\v|\r)+', r' ',                              # Trim whitespace
    code))))))))




# Reads a GLS source file
# Removes any useless withespace near curly braces and replaces any combination of withespace characters with a single space
# Returns the result as a string. The input code must contain no comments
def clearBuild(code:str):
    for t in tokenize.generate_tokens(io.StringIO(code).readline):
        if t.type not in [token.INDENT, token.DEDENT, token.NL, token.ENDMARKER, token.NEWLINE, token.COMMENT]:
            yield(t.string)




# Reads a GLS file and returns the preprocessed output as a string
# Comments are removed
def preprocessGls(file:str):
    return subprocess.run(
        ['glslangValidator', file, '-E'],
        capture_output = True, text = True
    ).stdout






# Reads a C++ file and returns the preprocessed output as a string
# Comments are removed
def preprocessCpp(file:str, flags:list):
    return subprocess.run(
        ['g++', *flags, '-E', '-xc++', file],
        capture_output = True, text = True
    ).stdout
