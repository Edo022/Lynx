import os, sys; sys.path.append(os.path.dirname(os.path.abspath(__file__)) + '/..') #FIXME USE ABSOLUTE IMPORT WHEN COMPILING IN BINARIES
import Utils as u




# Reads an ILSL file and returns its content as a list of tokens
# Any combination of whitespace character is replaced with a single space
# Preprocessor directives are expanded
# Comments are ignored
def tokenizeIlsl(vFile:str, vFlags:list):
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
    tok = {
        'op' : {
            #! %,  &,  |,  ^,  >>  and <<  are not implemented in GLSL. Use the functions
            #! %=, &=, |=, ^=, >>= and <<= are not implemented in GLSL. Use the functions
            'unary'      : [r'\+\+',   r'--',      r'!',       r'~'],
            'binary'     : [r'\+',     r'\*',      r'-',       r'\/',      r'%',       r'&',       r'\|',      r'\^',      r'<<',      r'>>'],
            'assignment' : [r'\+=',    r'\*=',     r'-=',      r'\/=',     r'%=',      r'&=',      r'\|=',     r'\^=',     r'<<=',     r'>>=',     r'='],
            'logical'    : [r'&&',     r'\|\|',    r'\^\^',    r'==',      r'!=',      r'<',       r'>',       r'<=',      r'>='],
            'group'      : [r'\(',     r'\)',      r'\{',      r'\}',      r'\[',      r'\]'],
            'other'      : [r'\?',     r':',       r'\\',      r',',       r'\.',      r';']
        },
        'types' : {
            'scalar'  : [r'[ui]32',                     r'f(32|64)',                    r'bool'],
            'vector'  : [r'[ui]32v[234]',               r'f(32|64)v[234]',              r'bv[234]'],
            'matrix'  : [r'[ui]32m[234](?:x[234])?',    r'f(32|64)m[234](?:x[234])?',   r'bm[234](?:x[234])?'],        #TODO IMPLEMENT IN LYNX TYPES #TODO ADD RECTANGULAR MATRICES
            'other'   : [r'void']
        },
        'keyword' : [
            r'if', r'else', r'while', r'for', r'break', r'continue', r'do', r'switch', r'case', r'default', r'return',
            r'local, extern, const, struct'
        ],
        'functions' : [
            r'rad',     r'deg',                     # Degrees to radiants, radiants to degrees
            r'sin',     r'cos',     r'tan',         # Sine, cosine, tangent
            r'asin',    r'acos',    r'atan',        # Inverse sine, inverse cosine, arg tangent
            r'sinh',    r'cosh',    r'tanh',        # Hyperbolic sine, hyperbolic cosine, hyperbolic tangent
            r'asinh',   r'acosh',   r'atanh',       # Inverse hyperbolic sine, inverse hyperbolic cosine, inverse hyperbolic tangent
            r'pow',     r'sqrt',    r'isqrt',       # Power, square root, inverse square root
            r'exp',     r'log',                     # Natural exponentiation, natural logarithm
            r'exp2',    r'log2',                    # 2 to the power of n, base 2 logarithm of n
            r'abs',     r'sign',                    # Absolute value, sign (-1 | 1)
            r'floor',   r'ceil',                    # Floor, ceil
            r'mfloor'   r'mceil',                   # Floor to multiple, ceil to multiple
            r'trunc',   r'round',   r'roundEven'    # Truncate to integer, round to the nearest integer, round to the nearest even integer
            r'fract',   r'modf',
            r'min',     r'max',
            r'clamp', r'mix', r'step', r'sstep',
            r'isnan', r'isinf',
            r'fBitsToInt', r'fBitsToUint', r'iBitsToFloat', r'uBitsToFloat',
            r'fma', r'frexp', r'idexp',
            r'length', r'dist', r'cross', r'norm', r'reflect', r'refract', r'faceforward',
            r'matrixCompMult', r'OuterProduct', r'transpose', r'determinant', r'inverse'
        ],
        'literal' : [
            r'b0(?:[01]+)(?:\.(?:[01]+))?',
            r'(d0)?(?:[0-9]+)(?:\.(?:[0-9]+))?',
            r'(o)?0(?:[0-7]+)(?:\.(?:[0-7]+))?',
            r'x0(?:[0-9a-fA-F]+)(?:\.(?:[0-9a-fA-F]+))?'
        ]
    }




    tokKeywords
    while i < len(code):
        r = loop(code, )






def preprocess(vCode:str):
    return code






def printError(vStr:str):
    print(f'{ u.bred }GenGlsl: Error: { vStr }')
    exit(-1)



def run(vSrc:str, vOut:str):
    # ts = list(tokenizeIlsl(vSrc, []))
    with open(vSrc) as f:
        code = f.read()

    s = '#version 450\n'    # Add hard coded version statement
    s += preprocess(code)
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
