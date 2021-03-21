import sys, os, re, textwrap



# 0 -            ([\t ]*layout.*(\(.*binding[\t ]*=[\t ]([0-9][0-9]?)\))[\t ]*(buffer|uniform) (.*)\{(([^\}]|\n)*)\})
# 1 -                           (\(.*binding[\t ]*=[\t ]([0-9][0-9]?)\))
# 2 Binding                                             ([0-9][0-9]?)
# 3 Type                                                                      (buffer|uniform)
# 4 Layout name                                                                                (.*)
# 5 Layout members                                                                                   (([^\}]|\n)*)
# 6 -                                                                                                 ([^\}]|\n)




def translateLine(line : str):
    return(
        re.sub(    r'vec(2( *$)|( *.*?;))', r'alignas(2 * 4)' + r' f32v\g<1>',
        re.sub(   r'bvec(2( *$)|( *.*?;))', r'alignas(2 * 4)' +   r' bv\g<1>',
        re.sub(   r'ivec(2( *$)|( *.*?;))', r'alignas(2 * 4)' + r' i32v\g<1>',
        re.sub(   r'uvec(2( *$)|( *.*?;))', r'alignas(2 * 4)' + r' u32v\g<1>',
        re.sub(   r'dvec(2( *$)|( *.*?;))', r'alignas(2 * 8)' + r' f64v\g<1>',
        re.sub( r'vec([34]( *$)|( *.*?;))', r'alignas(4 * 4)' + r' f32v\g<1>',
        re.sub(r'bvec([34]( *$)|( *.*?;))', r'alignas(4 * 4)' +   r' bv\g<1>',
        re.sub(r'ivec([34]( *$)|( *.*?;))', r'alignas(4 * 4)' + r' i32v\g<1>',
        re.sub(r'uvec([34]( *$)|( *.*?;))', r'alignas(4 * 4)' + r' u32v\g<1>',
        re.sub(r'dvec([34]( *$)|( *.*?;))', r'alignas(4 * 8)' + r' f64v\g<1>',
        re.sub(     r'int(( *$)|( *.*?;))', r'alignas(1 * 4)' +  r' i32\g<1>',
        re.sub(    r'uint(( *$)|( *.*?;))', r'alignas(1 * 4)' +  r' u32\g<1>',
        re.sub(    r'bool(( *$)|( *.*?;))', r'alignas(1 * 4)' + r' bool\g<1>',
        re.sub(   r'float(( *$)|( *.*?;))', r'alignas(1 * 4)' +  r' f32\g<1>',
        re.sub(  r'double(( *$)|( *.*?;))', r'alignas(1 * 8)' +  r' f64\g<1>',
        line))))))))))))))).strip()
    )




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

        #GLSL data types
        r = re.search(
            r'^((([biuv]?vec[234])|(double|float|bool|(u?int)))( *)'#Get type name
            r'((( |\n)*((^\/\*(.|\n)*?\*\/)|(^\/\/.*?\n)))*'        #Skip any comment or whitespace
            r'([a-zA-Z_]{1,}[a-zA-Z0-9_]*))'                        #Get variable name
            r'(.|\n)*?;)',                                          #Jump to instruction end
            members
        )
        if not r is None:
            type_:str = translateDataType(r.group(2))
            ret += type_ + '& '             #Write translated type

            align:int = getTypeSize(r.group(2))                     #Get alignment
            if offset % align != 0: offset = offset % align + align #Fix element offset and #Create getter from variable name
            ret += 'get' + r.group(7)[0].upper() + r.group(7)[1:] + '() { return *(' + type_ + '*)' + ('(Shader_b::data +' + str(offset) + ')' if offset != 0 else 'Shader_b::data') + '; }'
            offset += align                                         #Calculate raw offset of the next element

            members = members[len(r.group(0)):]                     #Pop from source string
            continue

        if members[0] in ('\t', '\n',):
            members = members[1:]

        else:
            members = members[1:]


    return ret








def translateStructDecl(name : str, members : str, space:bool):
    return (('\n' if space else '') +
        ('\nstruct ' + name + ' : public Shader_b {')                        #Write struct name
        + '\n' + textwrap.indent(translateMembers(members), '\t')
        + '\n};'                                                                    #Write struct closing bracket
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
        r'((^|\n)((\/\*(.|\n)*?\*\/)|[\t ])*'   # 1 2 3 4    #Skip comments and whitespace //FIXME FIX GROUPS INDICES COMMENTS
        r'layout.*(\(.*binding[\t ]*=[\t ]'     # 5 6        #Find layout declaration
        r'([0-9][0-9]?)\))'                     # 7          #Get layout binding
        r'[\t ]*(buffer|uniform) (.*)'          # 8 9        #Get layout type and name
        r'\{(([^\}]|\n)*)\})',                  # 10 11      #Get layout members
        fr.read())
    if shader:
        for layout in range(0, len(shader)):                    #For each layout
            # fc.write(translateStructDef(shader[layout]))            #Write .hpp and #Write .hpp
            fh.write(textwrap.indent(translateStructDecl(shader[layout][8], shader[layout][9].strip(), layout != 0), '\t'))
    else:
        print('No layout found. A shader must define at least one layout')

    fh.write('\n}'); fc.write('}')                          #Write namespace closing bracket



#TODO ADD STRUCTURE PARSING AND TRANSLATION