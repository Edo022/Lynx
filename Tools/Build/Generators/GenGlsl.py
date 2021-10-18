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







#TODO IMPLEMENT IN LYNX TYPES #TODO ADD RECTANGULAR MATRICES
# A list of all recognized tokens in ILSL
# b = binary      a = assign      l = logical       t = type    k = keyword     f = function        c = literal constant
tok = {
    #! %,  &,  |,  ^,  >>  and <<  are not implemented in GLSL. Use the functions
    #! %=, &=, |=, ^=, >>= and <<= are not implemented in GLSL. Use the functions

    'b_sum' : r'\+',        'b_sub' : r'-',         'b_mul' : r'\*',        'b_div' : r'\/',        'b_mod' : r'%',
    'a_sum' : r'\+=',       'a_sub' : r'-=',        'a_mul' : r'\*=',       'a_div' : r'\/=',       'a_mod' : r'%=',
    'inc'   : r'\+\+',      'dec'   : r'--',        'u_not' : r'~',

                                                                            'l_le'  : r'<=',        'l_me'  : r'>=',        'l_eq'  : r'==',
    'b_and' : r'&',         'b_xor' : r'\^',        'b_or'  : r'\|',        'b_ls'  : r'<<',        'b_rs'  : r'>>',        'l_not' : r'!',
    'a_and' : r'&=',        'a_xor' : r'\^=',       'a_or'  : r'\|=',       'a_ls'  : r'<<=',       'a_rs'  : r'>>=',       'a_set' : r'=',
    'l_and' : r'&&',        'l_xor' : r'\^\^',      'l_or'  : r'\|\|',      'l_l'   : r'<',         'l_m'   : r'>',         'l_neq' : r'!=',

    'lr'    : r'\(',        'rr'    : r'\)',        'lc'    : r'\{',        'rc'    : r'\}',        'lf'    : r'\[',        'rs'   : r'\]',
    'ternc' : r'\?',        'terna' : r':',         'esc'   : r'\\',        'comma' : r',',         'field' : r'\.',        'sc'   : r';',


    'path' : r'(?:\.?(?:/?(?:[a-zA-Z_\.\-0-9]+))+\/?)',     # Path
    'id'   : r'(?:[a-zA-Z_](?:[a-zA-Z0-9_]*))',             # Identifier
    'nid'  : r'(?:[^a-zA-Z_]|^)',                           # Not an identifier


    't_scl'  : r'(?:b|f32|f64|u32|i31)',                    # Scalar types
    't_vec'  : r'(?:b|f32|f64|u32|i31)v[234]',              # Vectors
    't_mat'  : r'(?:b|f32|f64|u32|i31)m[234](?:x[234])?',   # Matrices
    't_void' : r'void',                                     # just void


    # If-else                   # Loops                 # Flow control              # Switch case
    'k_if'   : r'if',          'k_whl' : r'while',      'k_cnt' : r'continue',      'k_swch' : r'switch',
    'k_else' : r'else',        'k_for' : r'for',        'k_brk' : r'break',         'k_case' : r'case',
    'k_elif' : r'elif',        'k_do'  : r'do',         'k_ret' : r'return',        'k_def'  : r'default',

    # Inputs                    # Other
    'k_loc' : r'local',         'k_cst' : r'const',
    'k_ext' : r'extern',        'k_str' : r'struct',


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
}
# tok_b = list(t for t in tok)
# tok_a = list(t for t in tok)
# tok_l = list(t for t in tok)
# ok_op = tok_b + tok_a + tok_l

# tok_t = list(t for t in tok)
# tok_k = list(t for t in tok)
# # tok_f = list(t for t in tok)
# tok_c = list(t for t in tok)






# Tokenizer --------------------------------------------------------------------------------------------------------------------------------#








# Reads an ILSL file and returns its content as a list of tokens
# Any combination of whitespace character is replaced with a single space
# Preprocessor directives are expanded
# Comments are ignored
def tokenize(vFile:str, vFlags:list):
    code = preprocessC(vFile, vFlags) #TODO add include paths

    # Checks if a regex vTok matches the vStr, starting from the character at index vIndex
    # If it matches, the matched string is returned
    # If it does not match, None is returned
    # 0 length matches are considered not matched
    def check(vStr:str, vIndex:int, vTok:str):
        t = re.match('^' + vTok, vStr[vIndex:])
        if t != None and len(t.group(0)) > 0:
            return t.group(0)
        else:
            return None

    # Checks a list of tokens on vStr, starting from the character at index vIndex
    # returns the first matching token
    def loop(vStr:str, vTokens:list, vIndex:int):
        for t in vTokens:
            r = check(vStr, vIndex, t)
            if r != None: return r

    i  : int = 0
    ts : str = []




    tokKeywords
    while i < len(code):
        r = loop(code, )







# Preprocessor -----------------------------------------------------------------------------------------------------------------------------#








# Removes the trailing whitespace of each line
# Consecutive newline characters are preserved
def clear(vCode:str):
    return re.sub(r'\s+(\n|$)', r'\n', code)




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







# Main -------------------------------------------------------------------------------------------------------------------------------------#








def run(vSrc:str, vOut:str):
    # ts = list(tokenizeIlsl(vSrc, []))
    with open(vSrc) as f:
        code = f.read()

    s = '#version 450\n'    # Add hard coded version statement
    s += preprocess(code, vSrc)
    # i = 0
    # while i < len(ts):  # For each token
    #     s += ' '            # Add space separator
    #     s += ts[i]          # Concatenate token
    #     i += 1              # Update counter



    with open(vOut, 'w') as outFile:
        outFile.write(s)






if len(sys.argv) != 3: raise Exception('GenGlsl: Wrong number of arguments')
run(sys.argv[1], sys.argv[2])
sys.exit(0)
