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
        2 * 4 if not re.search(r'^[biu]?vec2',           type_) is None else
        2 * 8 if not re.search(r'^dvec2',                type_) is None else
        4 * 4 if not re.search(r'^[biu]?vec[34]',        type_) is None else
        4 * 8 if not re.search(r'^dvec[34]',             type_) is None else
        4     if not re.search(r'^((u?int)|float|bool)', type_) is None else
        8     if not re.search(r'^double',               type_) is None else
        99999999 #If something bad happens, make it obvious
    )








def translateMembers(members:str):
    members = members.expandtabs(4).strip()
    ret:str = ''


    offset = 0
    while len(members) > 0:
        #Skip comments
        r = re.search(r'(^\/\*(.|\n)*?\*\/)|(^\/\/.*?\n)', members)
        if not r is None:
            ret += r.group(0)                                       #Concatenate to return value
            members = members[len(r.group(0)):]                     #Pop from source string
            continue


        #Translate member
        r = re.search(
            r'^((([biuv]?vec[234])|(double|float|bool|(u?int)))'    # 1 2 3 4 5              # 2     #Get type name
            r'(((( |\n)*|((\/\*(.|\n)*?\*\/)|(\/\/.*?\n)))*)'       # 6 7 8 9 10 11 12 13    # 7     #Skip any comment or whitespace
            r'([a-zA-Z_]{1,}[a-zA-Z0-9_]*))'                        # 14                     # 14    #Get variable name
            r'((.|\n)*?);)',                                        # 15 16                  # 15    #Jump to instruction end
            members
        )
        if not r is None:
            type_:str = translateDataType(r.group(2))

            ret += r.group(7).strip()
            ret += type_ + '& '             #Write translated type
            ret += r.group(15).strip()

            align:int = getTypeSize(r.group(2))                     #Get alignment
            if offset % align != 0: offset = offset % align + align #Fix element offset and #Create getter from variable name
            ret += r.group(14) + '() { return *(' + type_ + '*)' + ('(Shader_b::data +' + str(offset) + ')' if offset != 0 else 'Shader_b::data') + '; }'
            offset += align                                         #Calculate raw offset of the next element

            members = members[len(r.group(0)):]                     #Pop from source string
            continue


        else:
            ret += members[0]
            members = members[1:]
    return ret








def translateStructDecl(name : str, members : str, space:bool):
    return (('\n' if space else '') +
        ('\nstruct ' + name + ' : public Shader_b {')               #Write struct name
        + '\n' + textwrap.indent(translateMembers(members), '\t')       #Write struct members
        + '\n};'                                                        #Write struct closing bracket
    )




def translateStructDef(layout):
    return(''
        + ('\n\tuint32 ' + layout[9] + '::bind = ' + layout[7] + ';')         #Define binding
        + ('\n\t' + layout[9] + '::' + layout[9] + '(){')                     #Set type
        + ('\n\t\tShader_b::type = ' + ('Storage' if layout[8] == 'buffer' else 'Uniform') + ';')
        + ('\n\t}\n')
    )








pathr = sys.argv[1]
shname = ''

if not os.path.exists(pathr): print("File does not exist")
elif not pathr.split('.')[-1] == 'comp': print('File is not a vulkan shader')
else:
    shname = '.'.join(pathr.split('.')[0 : -1])
    print('Writing ' + shname)




with open(pathr, 'r') as fr, open(shname + '.hpp', 'w') as fh, open(shname + '.cpp', 'w') as fc:
    fh.write(
        '\n//This file was generated automatically. Changes could be overwritten without notice\n'
        '#pragma once\n'
        '#include <LuxEngine/Types/Vectors/Vectors.hpp>\n'
        '#include <LuxEngine/Types/Pointer.hpp>\n'
        '#include <LuxEngine/Types/VPointer.hpp>\n'
        '#include <LuxEngine/Types/Shader_t.hpp>\n\n\n\n'
        'namespace lux::shd::' + shname.replace('.', '_') + '{'
    )
    fc.write(
        '\n//This file was generated automatically. Changes could be overwritten without notice\n'
        '#include <' + shname + '.hpp>\n\n\n\n'
        'namespace lux::shd::' + shname.replace('.', '_') + '{'
    )

    shader = re.findall(
        r'((^|\n)((\/\*(.|\n)*?\*\/)|[\t ])*'   # 0 1 2 3    # -      #Skip comments and whitespace
        r'layout.*(\(.*binding[\t ]*=[\t ]'     # 4 5        # -      #Find layout declaration
        r'([0-9][0-9]?)\))'                     # 6          # 6      #Get layout binding
        r'[\t ]*(buffer|uniform) (.*)'          # 7 8        # 7 8    #Get layout type and name
        r'\{(([^\}]|\n)*)\})',                  # 9 10       # 9      #Get layout members
        fr.read())
    if shader:
        for layout in range(0, len(shader)):                    #For each layout
            # fc.write(translateStructDef(shader[layout]))            #Write .hpp and #Write .hpp
            fh.write(textwrap.indent(translateStructDecl(shader[layout][8], shader[layout][9].strip(), layout != 0), '\t'))
    else:
        print('No layout found. A shader must define at least one layout')

    fh.write('\n}'); fc.write('}')                          #Write namespace closing bracket



#TODO ADD STRUCTURE PARSING AND TRANSLATION