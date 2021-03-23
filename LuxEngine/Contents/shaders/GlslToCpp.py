import sys, os, re, textwrap








def translateDataType(glslType : str):
    return(
        re.sub( r'^vec([234])', r'f32v\g<1>',
        re.sub(r'^bvec([234])',   r'bv\g<1>',
        re.sub(r'^ivec([234])', r'i32v\g<1>',
        re.sub(r'^uvec([234])', r'u32v\g<1>',
        re.sub(r'^dvec([234])', r'f64v\g<1>',
        re.sub(   r'^int',  r'i32',
        re.sub(  r'^uint',  r'u32',
        re.sub(  r'^bool', r'bool',
        re.sub( r'^float',  r'f32',
        re.sub(r'^double',  r'f64',
        glslType))))))))))
    )




def getTypeSize(type_ : str):
    return(
        2 * 4 if re.search(r'^[biu]?vec2',           type_) != None else
        2 * 8 if re.search(r'^dvec2',                type_) != None else
        4 * 4 if re.search(r'^[biu]?vec[34]',        type_) != None else
        4 * 8 if re.search(r'^dvec[34]',             type_) != None else
        4     if re.search(r'^((u?int)|float|bool)', type_) != None else
        8     if re.search(r'^double',               type_) != None else
        99999999 #If something bad happens, make it obvious
    )








def translateMembers(members:str):
    m = members.expandtabs(4).strip()
    ret:str = ''


    offset = 0
    while len(m) > 0:
        #Ignore whitespace
        r = re.search(r'^( |\n)*', m)
        if r != None:
            m = m[len(r.group(0)):]                     #Pop from source string


        #Skip comments
        r = re.search(r'(^\/\*(.|\n)*?\*\/)|(^\/\/.*?\n)', m)
        if r != None:
            ret += '\n' + r.group(0).strip()            #Concatenate to return value
            m = m[len(r.group(0)):]                     #Pop from source string


        #Translate member
        r = re.search(
            r'^(([biuv]?vec[234])|(double|float|bool|(u?int)))'     # 1 2 3 4                 # 1     #Get type name
            r'((( |\n)|((\/\*(.|\n)*?\*\/)|(\/\/.*?\n)))*)'           # 5  6  7  8  9  10 11    # 5     #Skip any comment or whitespace
            r'([a-zA-Z_]{1,}[a-zA-Z0-9_]*)'                            # 12                      # 12    #Get variable name
            r'((( |\n)|((\/\*(.|\n)*?\*\/)|(\/\/.*?\n)))*)'            # 13 14 15 16 17 18 19    # 13    #Skip any comment or whitespace
            r'\[?'                                                     # -                       # -     #Check opening array bracket
            r'((( |\n)|((\/\*(.|\n)*?\*\/)|(\/\/.*?\n)))*)'            # 20 21 22 23 24 25 26    # 20    #Skip any comment or whitespace
            r'(\])?'                                                   # 27                      # 27    #Check closing array bracket
            r'((( |\n)|((\/\*(.|\n)*?\*\/)|(\/\/.*?\n)))*)'            # 28 29 30 31 32 33 34    # 28    #Skip any comment or whitespace
            r';'                                                      # -                       # -     #Anchor to instruction end
            r'((( )|(\/\/.*?(\n|$)))*)',                            # 35 36 37 38 39          # 35    #Skip eventual comments after member declaration
            m
        )
        if r != None:
            type_:str = translateDataType(r.group(1))

            if r.group( 5) != None and len(r.group( 5).strip()) > 0: ret += '\n' + textwrap.dedent(r.group( 5)).rstrip() #Write comments
            if r.group(13) != None and len(r.group(13).strip()) > 0: ret += '\n' + textwrap.dedent(r.group(13)).rstrip() #Write comments
            if r.group(20) != None and len(r.group(20).strip()) > 0: ret += '\n' + textwrap.dedent(r.group(20)).rstrip() #Write comments
            if r.group(28) != None and len(r.group(28).strip()) > 0: ret += '\n' + textwrap.dedent(r.group(28)).rstrip() #Write comments
            if r.group(35) != None and len(r.group(35).strip()) > 0: ret += '\n' + textwrap.dedent(r.group(35)).rstrip() #Write comments
            ret += '\nalwaysInline ' + type_ + '& '                              #Write translated type

            #Find arrays
            if r.group(27) is not None: ret += '[]'


            align:int = getTypeSize(r.group(1))                     #Get alignment
            if offset % align != 0: offset = offset % align + align #Fix element offset and #Create getter from variable name
            ret += r.group(12) + '() { return *(' + type_ + '*)' + ('(Shader_b::data + ' + str(offset) + ')' if offset != 0 else 'Shader_b::data') + '; }'
            offset += align                                         #Calculate raw offset of the next element

            m = m[len(r.group(0)):]                                 #Pop from source string
            continue


        #umu umu
        else:
            m = m[1:]


    return ret, offset








def translateStructDecl(name : str, members : str, space:bool):
    translated = translateMembers(members)
    return (('\n\n' if space else '') +
        ('\nstruct ' + name + ' : public Shader_b {')               #Write struct name
        + '\n' + textwrap.indent(name + '() { Shader_b::data.realloc(' + str(translated[1]) + '); }' + translated[0], '\t')       #Write struct members
        + '\n};'                                                    #Write struct closing bracket
    )




def translateStructDef(layout):
    return(''
        + ('\n\tuint32 ' + layout[9] + '::bind = ' + layout[7] + ';')         #Define binding
        + ('\n\t' + layout[9] + '::' + layout[9] + '(){')                     #Set type
        + ('\n\t\tShader_b::type = ' + ('Storage' if layout[8] == 'buffer' else 'Uniform') + ';')
        + ('\n\t}\n')
    )








pathr  = sys.argv[1]

if not os.path.exists(pathr): print("File does not exist")
elif not pathr.split('.')[-1] == 'comp': print('File is not a vulkan shader')
else:
    spath  = re.search(r'^(.*/).*$', pathr)
    spath = spath.group(1) if spath != None else './'

    shname = re.search(r'^(.*/)?(.*)\..*$', pathr)
    if shname != None: shname = shname.group(2)

    print('Writing ' + spath + shname)




with open(spath + shname + '.comp', 'r') as fr, open(spath + shname + '.hpp', 'w') as fh, open(spath + shname + '.cpp', 'w') as fc:
    fh.write(
        '\n//This file was generated automatically. Changes could be overwritten without notice'
        '\n#pragma once'
        '\n#include <LuxEngine/Types/Vectors/Vectors.hpp>'
        '\n#include <LuxEngine/Types/Pointer.hpp>'
        '\n#include <LuxEngine/Types/VPointer.hpp>'
        '\n#include <LuxEngine/Types/Shader_t.hpp>\n\n\n'
        '\nnamespace lux::shd{'
        '\n\tstruct ' + re.sub(r'^([0-9].*)$', r'_\g<1>', re.sub(r'[^a-zA-Z0-9_]', '_', shname)) + '{'
    )
    fc.write(
        '\n//This file was generated automatically. Changes could be overwritten without notice\n'
        '#include <' + shname + '.hpp>\n\n\n\n'
        'namespace lux::shd::' + shname.replace('.', '_') + '{'
    )

    shader = re.findall(
        r'((^|\n)((\/\*(.|\n)*?\*\/)|[\t ])*'   # 0 1 2 3    # -      #Skip comments and whitespace
        r'layout.*(\(.*binding[\t ]*=[\t ]'      # 4 5        # -      #Find layout declaration
        r'([0-9][0-9]?)\))'                      # 6          # 6      #Get layout binding
        r'[\t ]*(buffer|uniform) (.*)'           # 7 8        # 7 8    #Get layout type and name
        r'\{(([^\}]|\n)*)\})',                  # 9 10       # 9      #Get layout members
        fr.read())
    if shader != None:
        for layout in range(0, len(shader)):                    #For each layout
            # fc.write(translateStructDef(shader[layout]))            #Write .hpp and #Write .hpp
            fh.write(textwrap.indent(translateStructDecl(shader[layout][8], shader[layout][9].strip(), layout != 0), '\t\t'))
    else:
        print('No layout found. A shader must define at least one layout')

    fh.write('\n\t};\n}'); fc.write('}')                          #Write namespace closing bracket



#TODO ADD STRUCTURE PARSING AND TRANSLATION