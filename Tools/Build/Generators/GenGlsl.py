import os, sys, re;
sys.path.append(os.path.dirname(os.path.abspath(__file__)) + '/..') #FIXME USE ABSOLUTE IMPORT WHEN COMPILING THE BINARY
import Utils as u




def printError(vStr:str):
    print(f'\n{ u.bRed }GenGlsl: Error: { bRed }')
    exit(1)


def printSyntaxError(vLineN:int, vLine:str, vFile:str, vStr:str):
    print(
        f'\n{ u.bRed }GenGlsl: Syntax error on line { str(vLineN + 1) }, file "{ os.path.relpath(vFile, ".") }":'
        f'\n{ vStr }{ u.nWht }'
        f'\n    { vLine }'
        f'\n\nCompilation stopped'
    )
    exit(2)




# Tokens -----------------------------------------------------------------------------------------------------------------------------------#







pat = {
    't_path' : r'(?:\.?(?:/?(?:[a-zA-Z_\.\-0-9]+))+\/?)',   # File path
    't_id'   : r'(?:[a-zA-Z_](?:[a-zA-Z0-9_]*))',           # Identifier
    't_ppd'  : r'(?:#)',                                    # Preprocessor directive
    # 't_str'  : r'(?:".*?[^\\]")',                           # String
    't_whs'  : r'(?:[ \t\r]+)',                           # Whitespace

    'c_bool' : r'true|false',                               # Boolean
    'c_bin'  : r'b0(?:[01]+)(?:\.(?:[01]+))?',              # Binary        # 0b10100
    'c_dec'  : r'(d0)?(?:[0-9]+)(?:\.(?:[0-9]+))?',         # Decimal       # 90872     # 0d90872
    'c_oct'  : r'(o)?0(?:[0-7]+)(?:\.(?:[0-7]+))?',         # Octal         # 030507    # 0o30507
    'c_hex'  : r'x0(?:[0-9a-fA-F]+)(?:\.(?:[0-9a-fA-F]+))?' # Hexadecimal   # 0x7a0f3
}
#TODO add exponential literals
#TODO precise qualifier

#TODO IMPLEMENT IN LYNX TYPES #TODO ADD RECTANGULAR MATRICES
#TODO add bool and integer matrices
#FIXME ++, --, +, - and () have different precedence based on their position
#TODO add line continuation
#TODO add semicolon
tok = {
    # Operator (type, category, precedence, associativity)
    'op' : dict((t[0], {'type' : 'op', 'ctgr' : t[1], 'prec' : t[2], 'assoc' : t[3]}) for t in [

        ('+',  'bin',  3, 'lr'),   ('-',  'bin',  3, 'lr'),   ('*',  'bin',  4, 'lr'),   ('/',   'bin',  4, 'lr'),   ('%',   'bin',  4, 'lr'),
        ('+=', 'set', 16, 'lr'),   ('-=', 'set', 16, 'lr'),   ('*=', 'set', 16, 'lr'),   ('/=',  'set', 16, 'lr'),   ('%=',  'set', 16, 'lr'),
        ('++', 'inc',  3, 'lr'),   ('--', 'dec',  3, 'lr'),   ('~',  'unr',  3, 'lr'),   ('<=',  'cmp',  7, 'lr'),   ('>=',  'cmp',  7, 'lr'),   ('==', 'cmp',  8, 'lr'),
        ('&',  'bin',  9, 'lr'),   ('^',  'bin', 10, 'lr'),   ('|',  'bin', 11, 'lr'),   ('<<',  'bin',  6, 'lr'),   ('>>',  'bin',  6, 'lr'),   ('!',  'lgc',  3, 'lr'),
        ('&=', 'set', 16, 'lr'),   ('^=', 'set', 16, 'lr'),   ('|=', 'set', 16, 'lr'),   ('<<=', 'set', 16, 'lr'),   ('>>=', 'set', 16, 'lr'),   ('=',  'set', 16, 'lr'),
        ('&&', 'lgc', 12, 'lr'),   ('^^', 'lgc', 13, 'lr'),   ('||', 'lgc', 14, 'lr'),   ('<',   'cmp',  7, 'lr'),   ('>',   'cmp',  7, 'lr'),   ('!=', 'cmp',  8, 'lr'),

        ('(',  'sep',  1, 'lr'),   ('{',  'sep',  2, 'lr'),   ('[',  'sep',  2, 'lr'),   ('?',   'sel', 15, 'lr'),   ('.',   'fld',  2, 'lr'),
        (')',  'sep',  1, 'lr'),   ('}',  'sep',  2, 'lr'),   (']',  'sep',  2, 'lr'),   (':',   'sel', 15, 'lr'),   (',',   'seq', 17, 'lr')
    ]),



    #! integer and boolean matrices are implemented as multiple arrays of the base type
    # Type (type, base type, x, y, alignment)
    'types' : dict((t[0], {'type' : 'type', 'base' : t[1], 'x' : t[2], 'y': t[3], 'align' : t[4]}) for t in [
        ('b',     'b', 1, 1,  4),    ('u32',     'u32', 1, 1,  4),    ('i32',     'i32', 1, 1,  4),    ('f32',     'f32', 1, 1,  4),    ('f64',     'f64', 1, 1,  8),    # Scalar types
        ('bv2',   'b', 2, 1,  8),    ('u32v2',   'u32', 2, 1,  8),    ('i32v2',   'i32', 2, 1,  8),    ('f32v2',   'f32', 2, 1,  8),    ('f64v2',   'f64', 2, 1, 16),    # 2-component vectors
        ('bv3',   'b', 3, 1, 16),    ('u32v3',   'u32', 3, 1, 16),    ('i32v3',   'i32', 3, 1, 16),    ('f32v3',   'f32', 3, 1, 16),    ('f64v3',   'f64', 3, 1, 32),    # 3-component vectors
        ('bv4',   'b', 4, 1, 16),    ('u32v4',   'u32', 4, 1, 16),    ('i32v4',   'i32', 4, 1, 16),    ('f32v4',   'f32', 4, 1, 16),    ('f64v4',   'f64', 4, 1, 32),    # 4-component vectors
        ('bm2',   'b', 2, 2,  8),    ('u32m2',   'u32', 2, 2,  8),    ('i32m2',   'i32', 2, 2,  8),    ('f32m2',   'f32', 2, 2,  8),    ('f64m2',   'f64', 2, 2, 16),    # 2x2 square matrices
        ('bm3',   'b', 3, 3, 16),    ('u32m3',   'u32', 3, 3, 16),    ('i32m3',   'i32', 3, 3, 16),    ('f32m3',   'f32', 3, 3, 16),    ('f64m3',   'f64', 3, 3, 32),    # 3x3 square matrices
        ('bm4',   'b', 4, 4, 16),    ('u32m4',   'u32', 4, 4, 16),    ('i32m4',   'i32', 4, 4, 16),    ('f32m4',   'f32', 4, 4, 16),    ('f64m4',   'f64', 4, 4, 32),    # 4x4 square matrices
        ('bm2x2', 'b', 2, 2,  8),    ('u32m2x2', 'u32', 2, 2,  8),    ('i32m2x2', 'i32', 2, 2,  8),    ('f32m2x2', 'f32', 2, 2,  8),    ('f64m2x2', 'f64', 2, 2, 16),    # 2x2 matrices
        ('bm2x3', 'b', 2, 3,  8),    ('u32m2x3', 'u32', 2, 3,  8),    ('i32m2x3', 'i32', 2, 3,  8),    ('f32m2x3', 'f32', 2, 3,  8),    ('f64m2x3', 'f64', 2, 3, 16),    # 2x3 matrices
        ('bm2x4', 'b', 2, 4,  8),    ('u32m2x4', 'u32', 2, 4,  8),    ('i32m2x4', 'i32', 2, 4,  8),    ('f32m2x4', 'f32', 2, 4,  8),    ('f64m2x4', 'f64', 2, 4, 16),    # 2x4 matrices
        ('bm3x2', 'b', 3, 2, 16),    ('u32m3x2', 'u32', 3, 2, 16),    ('i32m3x2', 'i32', 3, 2, 16),    ('f32m3x2', 'f32', 3, 2, 16),    ('f64m3x2', 'f64', 3, 2, 32),    # 3x2 matrices
        ('bm3x3', 'b', 3, 3, 16),    ('u32m3x3', 'u32', 3, 3, 16),    ('i32m3x3', 'i32', 3, 3, 16),    ('f32m3x3', 'f32', 3, 3, 16),    ('f64m3x3', 'f64', 3, 3, 32),    # 3x3 matrices
        ('bm3x4', 'b', 3, 4, 16),    ('u32m3x4', 'u32', 3, 4, 16),    ('i32m3x4', 'i32', 3, 4, 16),    ('f32m3x4', 'f32', 3, 4, 16),    ('f64m3x4', 'f64', 3, 4, 32),    # 3x4 matrices
        ('bm4x2', 'b', 4, 2, 16),    ('u32m4x2', 'u32', 4, 2, 16),    ('i32m4x2', 'i32', 4, 2, 16),    ('f32m4x2', 'f32', 4, 2, 16),    ('f64m4x2', 'f64', 4, 2, 32),    # 4x2 matrices
        ('bm4x3', 'b', 4, 3, 16),    ('u32m4x3', 'u32', 4, 3, 16),    ('i32m4x3', 'i32', 4, 3, 16),    ('f32m4x3', 'f32', 4, 3, 16),    ('f64m4x3', 'f64', 4, 3, 32),    # 4x3 matrices
        ('bm4x4', 'b', 4, 4, 16),    ('u32m4x4', 'u32', 4, 4, 16),    ('i32m4x4', 'i32', 4, 4, 16),    ('f32m4x4', 'f32', 4, 4, 16),    ('f64m4x4', 'f64', 4, 4, 32),    # 4x4 matrices
        ('void','void',0, 0,  0)                                                                                                                                         # No size, no alignment. Just void :c
    ]),


    # (type, category)
    'kw' : list((t[0], {'type' : 'kw', 'ctgr' : t[1]}) for t in [
        # If-else               # Loops                   # Flow control              # Switch case
        ('if',   'if'),         ('while', 'loop'),        ('continue', 'fc'),         ('switch',  'switch'),
        ('else', 'if'),         ('for',   'loop'),        ('break',    'fc'),         ('case',    'switch'),
        ('elif', 'if'),         ('do',    'loop'),        ('return',   'fc'),         ('default', 'switch'),

        # Inputs                   # Other
        ('local' , 'input'),        ('const',  'qualifier'),
        ('extern', 'input'),        ('struct', 'struct')
    ])
}


# Merge and sort the tokens
all2 = {}; all2.update(tok['op']); all2.update(tok['types']); all2.update(tok['kw'])
all = dict(sorted(all2.items(), key = lambda s: len(s[0])))




# TODO ADD BUILTIN FUNCTIONS
# # Sine          # Cosine            # Tangent
# 'f_sin'     : r'sin',         'f_cos'   : r'cos',             'f_tan'   : r'tan',         # Sine,                     cosine,                     tangent
# 'f_asin'    : r'asin',        'f_acos'  : r'acos',            'f_atan'  : r'atan',        # Inverse sine,             inverse cosine,             inverse tangent
# 'f_sinh'    : r'sinh',        'f_cosh'  : r'cosh',            'f_tanh'  : r'tanh',        # Hyperbolic sine,          hyperbolic cosine,          hyperbolic tangent
# 'f_asinh'   : r'asinh',       'f_acosh' : r'acosh',           'f_atanh' : r'atanh',       # Inverse hyperbolic sine,  inverse hyperbolic cosine,  inverse hyperbolic tangent
# 'f_pow'     : r'pow',         'f_sqrt'  : r'sqrt',            'f_isqrt' : r'isqrt',       # Exponentiation,           square root,                inverse square root
# 'f_exp'     : r'exp',         'f_log'   : r'log',    # Natural exponentiation,   natural logarithm
# 'f_exp2'    : r'exp2',        'f_log2'  : r'log2',   # 2 to the power of n,      base 2 logarithm of n
# # Vectors       # Matrices          # Bits
# 'f_length'  : r'length',      'f_matrixCompMult' : r'matrixCompMult',  'f_fBitsToInt'   : r'fBitsToInt',  # Length of a vector
# 'f_dist'    : r'dist',        'f_OuterProduct'   : r'OuterProduct',    'f_fBitsToUint'  : r'fBitsToUint',
# 'f_cross'   : r'cross',       'f_transpose'      : r'transpose',       'f_iBitsToFloat' : r'iBitsToFloat',
# 'f_norm'    : r'norm',        'f_determinant'    : r'determinant',     'f_uBitsToFloat' : r'uBitsToFloat',
# 'f_reflect' : r'reflect',     'f_inverse'        : r'inverse',
# 'f_refract' : r'refract',
# 'f_faceforward' : r'faceforward',
# # Round
# 'f_floor'  : r'floor',       'f_ceil'      : r'ceil',                        # Floor, ceil
# 'f_mfloor' : r'mfloor',       'f_mceil'    : r'mceil',                      # Floor to multiple, ceil to multiple
# 'f_round'  : r'round',       'f_roundEven' : r'roundEven',                   # Truncate to integer, round to the nearest integer, round to the nearest even integer
# # Conversion    # Sign          # Comparison
# 'f_rad'    : r'rad',         'f_abs'  : r'abs',         'f_min' : r'min',
# 'f_deg'    : r'deg',         'f_sign' : r'sign',        'f_max' : r'max',
# # Other
# 'f_isnan' : r'isnan',       'f_step'  : r'step',        'f_frexp' : r'frexp',
# 'f_isinf' : r'isinf',       'f_sstep' : r'sstep',       'f_idexp' : r'idexp',
# 'f_fma'   : r'fma',         'f_trunc' : r'trunc',       'f_clamp' : r'clamp',
# 'f_mix'   : r'mix',         'f_fract' : r'fract',       'f_modf'  : r'modf',

# 'c_bin' : r'b0(?:[01]+)(?:\.(?:[01]+))?',                 # Binary        # 0b10100
# 'c_dec' : r'(d0)?(?:[0-9]+)(?:\.(?:[0-9]+))?',            # Decimal       # 90872     # 0d90872
# 'c_oct' : r'(o)?0(?:[0-7]+)(?:\.(?:[0-7]+))?',            # Octal         # 030507    # 0o30507
# 'c_hex' : r'x0(?:[0-9a-fA-F]+)(?:\.(?:[0-9a-fA-F]+))?'    # Hexadecimal   # 0x7a0f3










# Preprocessor -----------------------------------------------------------------------------------------------------------------------------#








# Replaces multiline comments with the same number of newlines they contain
# Single line comments are replaced with a single space
# Returns the resulting string
def uncomment(vCode:str, vFile:str):
    code = ''       # Output code
    i = 0           # Counter
    while i < len(vCode):                               # For each character
        if vCode[i] == '"':                                 # If the character is a double quote
            strBegin:int = i                                    # Save the string beginning for eventual errors
            code += vCode[i]                                    # Paste opening "
            i += 1                                              # Skip opening "
            while vCode[i] != '"':                              # For each character of the string
                code += vCode[i]                                    # Paste character
                i += 1                                              # Update counter
                if i == len(vCode):                                 # If the string does not end
                    vLineN:int = 0                                      #
                    for j in range(0, strBegin):                        # Find the line in which the string begins
                        if vCode[j] == '\n': vLineN += 1                # [...] Print a syntax error
                    printSyntaxError(vLineN, vCode.split('\n')[vLineN], vFile, 'Unterminated string')
            code += vCode[i]                                    # Paste closing "
            i += 1                                              # Skip closing "
        elif i < len(vCode) - 1:                            # If there is enough space to start a comment
            if vCode[i:i + 2] == '//':                          # If the character is the beginning of a single line comment
                code += '\n'                                        # Add a newline as token separator
                i += 2                                              # Ignore //
                while i < len(vCode) and vCode[i] != '\n':          # For each character of the comment
                    i += 1                                              # Update the counter and ignore the character
                i += 1                                              # Ignore \n
            elif vCode[i:i + 2] == '/*':                        # If the character is the beginning of a multiline comment
                code += ' '                                         # Add a space as token separator
                i += 2                                              # Ignore /*
                while i < len(vCode) and vCode[i:i + 2] != '*/':    # For each character of the comment
                    if vCode[i] == '\n':                                # If the character is a newline
                        code += '\n'                                        # Paste the newline
                    i += 1                                              # Update the counter and ignore the other characters
                i += 2                                              # Ignore */
            else:                                               # Else
                code += vCode[i]                                    # Paste the character
                i += 1                                              # Update the counter
        else:                                               # Else
            code += vCode[i]                                    # Paste the character
            i += 1                                              # Update the counter
    return code                                         # Return the parsed code




# Parses the member list and returns the macro definition as a list of tokens
def saveMacro(vLines:list, vName:str, vMembers:str):
    #TODO
    None




# Checks if an included path is valid
# Prints an error if it's not
def checkIncludeFile(vLineN:str, vLine:list, vFile:str, vName:str):
    if not re.match('^' + tok['path'] + '$', vName): printSyntaxError(vLineN, vLine, vFile, f'"{ vName }" is not a valid file path')
    if os.path.exists(vName):
        if vName[-1] == '/' or os.path.isdir(vName): printSyntaxError(vLineN, vLine, vFile, f'"{ vName }" is a directory')
    else:                                            printSyntaxError(vLineN, vLine, vFile, "No such file or directory")




# Creates a code with no includes by pasting all the included files together
# Returns the resulting string
# Comments are not preserved
def include(vCode:str, vFile:str):
    code = ''
    ls:list = uncomment(vCode, vFile).split('\n')
    for i, (l, ol) in enumerate(zip(ls, vCode.split('\n'))):        # For each line of the code
        ri = re.match(r'^\s*#include(?:\s*)"(?P<path>.*)"', l)          # Check if it's an include
        if ri != None:                                                  # If the line is an include statement
            checkr = checkIncludeFile(i, ol, vFile, ri['path'])             # Check the included file
            with open(ri['path'], 'r') as f:                                # Open the included file
                code += include(f.read(), ri['path'])                           # Paste the included code recursively
        else:                                                           # If not
            code += l                                                       # Concatenate line
        code += '\n'                                                    # Add newline

    return code










# Preprocesses an ILSL code
# Pastes all the included files, expands the macros and removes any comment or trailing whitespace
# Unknown preprocessor directives cause an error
def preprocess(vCode:str, vFile:str):
    return include(vCode, vFile)








# Tokenizer --------------------------------------------------------------------------------------------------------------------------------#








# Reads an ILSL file and returns its content as a list of tokens
# Any combination of whitespace character is replaced with a single space
# Preprocessor directives are expanded
# Comments are ignored
def tokenize(vCode:str, vFile:str):
    lines = vCode.split('\n')
    for vLineN, l in enumerate(lines):
        i : int = 0
        while i < len(l):
            for j, t in enumerate(all.items()):
                if l[i:].startswith(t[0]):
                    yield(t)
                    i += len(t[0])
                    break

                if l[i] == ';':
                    yield((';', 'semicolon'))
                    i += 1
                    break

                r = re.match(pat['t_ppd'],  l[i:])
                if r != None: yield((r.group(0), 'preprocessor')); i += len(r.group(0)); break
                r = re.match(pat['t_whs'],  l[i:])
                if r != None: yield((r.group(0), 'whitespace'));   i += len(r.group(0)); break
                r = re.match(pat['t_id'],   l[i:])
                if r != None: yield((r.group(0), 'identifier'));   i += len(r.group(0)); break

                r = re.match(pat['c_bin'],  l[i:])
                if r != None: yield((r.group(0), 'literal')); i += len(r.group(0)); break
                r = re.match(pat['c_oct'],  l[i:])
                if r != None: yield((r.group(0), 'literal')); i += len(r.group(0)); break
                r = re.match(pat['c_dec'],  l[i:])
                if r != None: yield((r.group(0), 'literal')); i += len(r.group(0)); break
                r = re.match(pat['c_hex'],  l[i:])
                if r != None: yield((r.group(0), 'literal')); i += len(r.group(0)); break
                r = re.match(pat['c_bool'], l[i:])
                if r != None: yield((r.group(0), 'literal')); i += len(r.group(0)); break

                elif j == len(all) - 1:
                    # printSyntaxError(vLineN, vLine, vFile, vStr)
                    printSyntaxError(vLineN, l, vFile, f'Unknown token "{ l[i] }"')
                    return
        yield('\n')









# Main -------------------------------------------------------------------------------------------------------------------------------------#








# Removes the trailing whitespace of each line
# Consecutive newline characters are preserved
def clear(vCode:str):
    return re.sub(r'[ \t\v]+(\n|$)', r'\n', vCode)




def run(vSrc:str, vOut:str):
    # Read input file
    with open(vSrc) as f:
        code = f.read()

    # Add hard coded version statement and parse the code
    ts = list(tokenize(clear(preprocess('/*000000*/\n#version 450\n' + code, vSrc)), vSrc))


    # Write output file
    with open(vOut, 'w') as outFile:
        outFile.write(str(ts))






if len(sys.argv) != 3: raise Exception('GenGlsl: Wrong number of arguments')
run(sys.argv[1], sys.argv[2])
sys.exit(0)
