import os, sys, re;
sys.path.append(os.path.dirname(os.path.abspath(__file__)) + '/..') #FIXME USE ABSOLUTE IMPORT WHEN COMPILING THE BINARY
import Utils as u










# More utils -------------------------------------------------------------------------------------------------------------------------------#








def printError(vStr:str):
    print(f'\n{ u.bRed }GenGlsl: Error:\n{ vStr }{ u.bRed }')
    exit(1)


def printSyntaxError(vLineN:int, vLine:str, vFile:str, vStr:str):
    print(
        f'\n{ u.bRed }GenGlsl: Syntax error on line { str(vLineN + 1) }, file "{ os.path.relpath(vFile, ".") }":'
        f'\n{ vStr }{ u.nWht }'
        f'\n    { vLine }'
        f'\n\nCompilation stopped'
    )
    exit(2)



# Common patterns
pat = {
    't_path' : r'(?:\.?(?:/?(?:[a-zA-Z_\.\-0-9]+))+\/?)',           # File path
    't_id'   : r'(?:[a-zA-Z_](?:[a-zA-Z0-9_]*))',                   # Identifier
    't_ppd'  : r'(?:#)',                                            # Preprocessor directive
    't_whs'  : r'(?:[ \t\r]+)',                                     # Whitespace


    'c_bool'    : r'(?:true|false)',                                # Boolean literal
    'c_pnlc'    : r'(?:\.?(?:[0-9])[0-9a-zA-Z.]*)',                 # Possible numeric literal constant

    'c_bin'  : r'(?:(?:\.?'  r'0b(?:[0-1]'   r'*))|'  r'(?:0b(?:[0-1]'   r'*)(?:\.?)(?:[0-1]+)))',          # Valid binary      literal   # 0b10100
    'c_oct'  : r'(?:(?:\.?'  r'0o(?:[0-7]'   r'*))|'  r'(?:0o(?:[0-7]'   r'*)(?:\.?)(?:[0-7]+)))',          # Valid octal       literal   # 0o30507
    'c_hex'  : r'(?:(?:\.?'  r'0x(?:[0-9a-fA-F]*))|'  r'(?:0x(?:[0-9a-fA-F]*)(?:\.?)(?:[0-9a-fA-F]+)))',    # Valid hexadecimal literal   # 0x7a0f3
    'c_dec'  : r'(?:(?:\.?(?:0d)?(?:[0-9]'   r'*))|(?:(?:0d)?(?:[0-9]'   r'*)(?:\.?)(?:[0-9]+)))',          # Valid decimal     literal   # 90872     # 0d90872

    'c_bin_bgn'  : r'(?:0b)',                                       # Binary      literal beginning
    'c_oct_bgn'  : r'(?:0o)',                                       # Octal       literal beginning
    'c_hex_bgn'  : r'(?:0x)',                                       # Hexadecimal literal beginning
    'c_dec_bgn'  : r'(?:(?:0d)|[0-9])',                             # Decimal     literal beginning
}








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
    if not re.match('^' + pat['t_path'] + '$', vName): printSyntaxError(vLineN, vLine, vFile, f'"{ vName }" is not a valid file path')
    if os.path.exists(vName):
        if vName[-1] == '/' or os.path.isdir(vName): printSyntaxError(vLineN, vLine, vFile, f'"{ vName }" is a directory')
    else:                                            printSyntaxError(vLineN, vLine, vFile, "No such file or directory")




# Creates a code with no includes by pasting all the included files together
# Returns the resulting string
# Comments are not preserved
def include(vCode:str, vFile:str, vLineInfo:int):
    code = ''
    ls:list = uncomment(vCode, vFile).split('\n')
    for i, (l, ol) in enumerate(zip(ls, vCode.split('\n'))):            # For each line of the code
        if i > 0: code += '\n'                                              # Add newline
        r = re.match(r'^\s*#include(?:\s*)"(?P<path>.*)"', l)               # Check if it's an include
        if r != None:                                                       # If the line is an include statement
            checkr = checkIncludeFile(i, ol, vFile, r['path'])                  # Check the included file
            with open(r['path'], 'r') as f:                                     # Open the included file
                code += include(f.read(), r['path'], i + 1)                         # Paste the included code recursively
        else:                                                               # If not
            code += f'/*{ str(i + 1 if vLineInfo == 0 else vLineInfo).zfill(6) }*/{ l }'# Concatenate line

    return code








# Tokenizer --------------------------------------------------------------------------------------------------------------------------------#








#TODO add exponential literals
#TODO precise qualifier

#TODO IMPLEMENT IN LYNX TYPES #TODO ADD RECTANGULAR MATRICES
#TODO add bool and integer matrices
#TODO add line continuation
#TODO add semicolon
    # Operator (type, category, precedence, associativity)
    #FIXME ++, --, +, - and () have different precedence based on their position
    #FIXME ++, -- have different associativiry based on their position
op = list(reversed(sorted(list(({'val' : t[0], 'type' : 'op', 'ctgr' : t[1], 'prec' : t[2], 'assoc' : t[3]}) for t in [
    ('+',  'bin',  3, 'lr'),   ('-',  'bin',  3, 'lr'),   ('*',  'bin',  4, 'lr'),   ('/',   'bin',  4, 'lr'),   ('%',   'bin',  4, 'lr'),
    ('+=', 'set', 16, 'rl'),   ('-=', 'set', 16, 'rl'),   ('*=', 'set', 16, 'rl'),   ('/=',  'set', 16, 'rl'),   ('%=',  'set', 16, 'rl'),
    ('++', 'inc',  3, 'lr'),   ('--', 'dec',  3, 'lr'),   ('~',  'unr',  3, 'lr'),   ('<=',  'cmp',  7, 'lr'),   ('>=',  'cmp',  7, 'lr'),   ('==', 'cmp',  8, 'lr'),
    ('&',  'bin',  9, 'lr'),   ('^',  'bin', 10, 'lr'),   ('|',  'bin', 11, 'lr'),   ('<<',  'bin',  6, 'lr'),   ('>>',  'bin',  6, 'lr'),   ('!',  'lgc',  3, 'lr'),
    ('&=', 'set', 16, 'rl'),   ('^=', 'set', 16, 'rl'),   ('|=', 'set', 16, 'rl'),   ('<<=', 'set', 16, 'rl'),   ('>>=', 'set', 16, 'rl'),   ('=',  'set', 16, 'rl'),
    ('&&', 'lgc', 12, 'lr'),   ('^^', 'lgc', 13, 'lr'),   ('||', 'lgc', 14, 'lr'),   ('<',   'cmp',  7, 'lr'),   ('>',   'cmp',  7, 'lr'),   ('!=', 'cmp',  8, 'lr'),

    ('(',  'sep',  1, 'lr'),   ('{',  'sep',  2, 'lr'),   ('[',  'sep',  2, 'lr'),   ('?',   'sel', 15, 'rl'),   ('.',   'fld',  2, 'lr'),
    (')',  'sep',  1, 'lr'),   ('}',  'sep',  2, 'lr'),   (']',  'sep',  2, 'lr'),   (':',   'sel', 15, 'rl'),   (',',   'seq', 17, 'lr')
]), key = lambda s: len(s['val']))))






tok = [
    #! integer and boolean matrices are implemented as multiple arrays of the base type
    # Type (type, base type, x, y, alignment)
    *list(({'val' : t[0], 'type' : 'tn', 'base' : t[1], 'x' : t[2], 'y': t[3], 'align' : t[4]}) for t in [
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
    *list(({'val' : t[0], 'type' : 'kw', 'ctgr' : t[1]}) for t in [
        # If-else               # Loops                     # Flow control              # Switch case
        ('if',   'if'),         ('while', 'loop'),          ('continue', 'fc'),         ('switch',  'switch'),
        ('else', 'if'),         ('for',   'loop'),          ('break',    'fc'),         ('case',    'switch'),
        ('elif', 'if'),         ('do',    'loop'),          ('return',   'fc'),         ('default', 'switch'),

        # Type qualifiers       # Inputs                    # Other
        ('highp',  'tq'),       ('local' , 'input'),        ('struct',    'other'),
        ('medp',   'tq'),       ('extern', 'input'),        ('precision', 'other'),
        ('lowp',   'tq'),
        ('const',  'tq')
    ])
]


# # Merge and sort the tokens from the largest one
# all = tok2['op'] + tok2['types'] + tok2['kw']
# all = sorted(all2, key = lambda s: len(s['val']))[::-1]




#TODO ADD OUTPUT QUALIFIER 'out'
#TODO add token beginning and length


# Reads an ILSL file and returns its content as a list of tokens
# Any combination of whitespace character is replaced with a single space
# Preprocessor directives are expanded
# Comments are ignored
def tokenize(vCode:str, vFile:str):
    lines = vCode.split('\n')
    for vLineN, decLine in enumerate(lines):
        lineInfo : str = re.match(r'\/\*.*?\*\/', decLine).group(0)
        l        : str = decLine[len(lineInfo):]
        i        : int = 0
        yield({'val' : lineInfo, 'type' : 'lineInfo'})


        while i < len(l):
            # Match preprocessor directives and whitespace
            if (r := re.match(pat['t_ppd'], l[i:])) != None: i += len(r.group(0)); yield({'val' : r.group(0), 'type' : 'ppd'}); continue
            if (r := re.match(pat['t_whs'], l[i:])) != None: i += len(r.group(0)); yield({'val' : r.group(0), 'type' : 'ws'});  continue


            # Match instruction end
            elif l[i] == ';':
                i += 1
                yield({'val' : ';', 'type' : 'sc'})
                continue


            # Match identifiers
            elif (r := re.match(pat['t_id'],  l[i:])) != None:
                id = r.group(0)

                # Save builtin identifiers and operators
                found:bool = False
                for t in tok:
                    if id == t['val']: #FIXME 'do' matches 'dot'
                        found = True
                        i += len(t['val'])
                        yield(t); break

                # Save defined identifiers
                if not found:
                    i += len(id)
                    yield({'val' : id, 'type' : 'id'})

                continue


            # Match literals
            elif (pnlc := re.match(pat['c_pnlc'], l[i:])) != None:
                nlc = pnlc.group(0)

                if (r := re.match(pat['c_bool'] + '$', nlc)) != None: i += len(r.group(0)); yield({'val' : r.group(0), 'type' : 'lc', 'base' :'b'}); continue
                if (r := re.match(pat['c_bin']  + '$', nlc)) != None: i += len(r.group(0)); yield({'val' : r.group(0), 'type' : 'lc', 'base' :  2}); continue
                if (r := re.match(pat['c_oct']  + '$', nlc)) != None: i += len(r.group(0)); yield({'val' : r.group(0), 'type' : 'lc', 'base' :  8}); continue
                if (r := re.match(pat['c_dec']  + '$', nlc)) != None: i += len(r.group(0)); yield({'val' : r.group(0), 'type' : 'lc', 'base' : 10}); continue
                if (r := re.match(pat['c_hex']  + '$', nlc)) != None: i += len(r.group(0)); yield({'val' : r.group(0), 'type' : 'lc', 'base' : 16}); continue

                # Check invalid literals
                if (r := re.match(pat['c_bin_bgn'], nlc)) != None: printSyntaxError(vLineN, l, vFile, f'Invalid binary '  f'literal "{ nlc }"')
                if (r := re.match(pat['c_oct_bgn'], nlc)) != None: printSyntaxError(vLineN, l, vFile, f'Invalid octal '   f'literal "{ nlc }"')
                if (r := re.match(pat['c_dec_bgn'], nlc)) != None: printSyntaxError(vLineN, l, vFile, f'Invalid decimal ' f'literal "{ nlc }"')
                if (r := re.match(pat['c_hex_bgn'], nlc)) != None: printSyntaxError(vLineN, l, vFile, f'Invalid hexadecimal literal "{ nlc }"')

                # Go to unknown token


            # Match operators
            found:bool = False
            for o in op:
                if l[i].startswith(o['val']):
                    found = True
                    i += len(o['val'])
                    yield(t); break
            if found:
                continue


            # Unknown tokens
            printSyntaxError(vLineN, l, vFile, f'Unknown token "{ l[i] }"')

        yield({'val' : '\n', 'type' : 'nl'})









# Grouping ---------------------------------------------------------------------------------------------------------------------------------#







translateType = {

}


def group(): #TODO
    None #TODO









# Main -------------------------------------------------------------------------------------------------------------------------------------#








# Removes the trailing whitespace of each line
# Consecutive newline characters are preserved
def clear(vCode:str):
    return re.sub(r'[ \t\v]+(\n|$)', r'\g<1>', vCode)




def run(vSrc:str, vOut:str):
    if not os.path.exists(vSrc): printError(f'"{ vSrc }": No such file or directory')

    # Read input file
    with open(vSrc) as f:
        code = f.read()

    # Add hard coded version statement and parse the code
    # ts = list(translate(tokenize(clear(include(code, vSrc, 0)), vSrc)))
    ts = list(tokenize(clear(include(code, vSrc, 0)), vSrc))


    # Write output file
    with open(vOut, 'w') as outFile:
        outFile.write('#version 450\n' + (''.join('\n' + str(t) for t in ts)))






if len(sys.argv) != 3: raise Exception('GenGlsl: Wrong number of arguments') #TODO replace with argparse
run(sys.argv[1], sys.argv[2])
sys.exit(0)
