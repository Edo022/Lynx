import sys, os, re, textwrap
from math import ceil



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
        re.sub( r'^vec([234])', r' f32v\g<1>',
        re.sub(r'^bvec([234])',   r' bv\g<1>',
        re.sub(r'^ivec([234])', r' i32v\g<1>',
        re.sub(r'^uvec([234])', r' u32v\g<1>',
        re.sub(r'^dvec([234])', r' f64v\g<1>',
        re.sub(   r'^int',  r' i32',
        re.sub(  r'^uint',  r' u32',
        re.sub(  r'^bool', r' bool',
        re.sub( r'^float',  r' f32',
        re.sub(r'^double',  r' f64',
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









def getType(line : str):
    return re.findall(r'([biuv]?vec[234])|(double|float|bool|(u?int))(2( *$)|( *.*?;))', line)





def translateMembers(members:str):
    members = members.expandtabs(4).strip()
    ret:str = ''


    # for member in members.split(';'):
    #     ret += member
    offset = 0
    while len(members) > 0:
        print(len(members) )
        #Skip comments
        r = re.search(r'(^\/\*(.|\n)*?\*\/)|(^\/\/.*?\n)', members)
        if not r is None:
            ret += r.group(0)
            members = members[len(r.group(0)):]
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
            ret += translateDataType(r.group(2)) + ' ' #FIXME justify instead of adding a random space
            ret += r.group(7)

            align:int = getTypeSize(r.group(2))
            if offset % align != 0: offset = offset % align + align
            ret += '/*' + str(offset) + '*/'
            offset += align

            members = members[len(r.group(0)):]
            continue

        if members[0] in ('\t', '\n',):
            members = members[1:]

        else:
            members = members[1:]


    return ret


    # for m in members:                                                           #For each member
    #     m = m.expandtabs(4).strip()                                                 #Remove whitespace and convert tabs to spaces
    #     comment = re.search(r'//.*$', m)                                            #Remove
    #     if not comment is None: m = re.sub(r'//.*$', '', m)                         #and save single line comments
    #     oldLen = re.search(r'([biud]?vec[234].*$)|(((u?int)|float|double).*$)', m)  #Calculate old length to preserve names and comments alignment

    #     ret += (
    #         (translateLine(m).ljust((len(oldLen.group(0)) + len('alignas(n * n) ')) if not oldLen is None else 0) +
    #         (comment.group(0) if not comment is None else '')).rstrip() + '\n'
    #     )
    # return ret




def translateStructDecl(name : str, members : str):
    # return (''
    #     + ('\n\tstruct ' + layout[4] + ' : public Shader_b {\n')                        #Write struct name
    #     + ('\t\tstatic uint32 bind;\n')                                                 #Write binding
    #     + ('\t\t' + layout[4] + '();\n')                                                #Write constructor
    #     + (textwrap.indent(translateMembers(layout[5].strip().split('\n')), '\t\t'))    #Write struct members
    #     + ('\t};\n')                                                                    #Write struct closing bracket
    # )
    return (
        ('\n\nstruct ' + name + ' : public Shader_b {')                        #Write struct name
        + '\n' + textwrap.indent(translateMembers(members), '\t')
        + '\n};'                                                                    #Write struct closing bracket
    )

    # for member in members.split(';'):
    #     ret += member
    # return ret
    # return (''
        # + ('\n\tstruct ' + layoutName + ' : public Shader_b {\n')                        #Write struct name
        # + '\n'.join(re.split(r'\/\/.*\n', members))
        # + ('\n};')                                                                    #Write struct closing bracket
    # )



def translateStructDef(layout):
    return(''
        + ('\n\tuint32 ' + layout[4] + '::bind = ' + layout[2] + ';')         #Define binding
        + ('\n\t' + layout[4] + '::' + layout[4] + '(){')                     #Set type
        + ('\n\t\tShader_b::type = ' + ('Storage' if layout[3] == 'buffer' else 'Uniform') + ';')
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

    shader = re.findall(r'([\t ]*layout.*(\(.*binding[\t ]*=[\t ]([0-9][0-9]?)\))[\t ]*(buffer|uniform) (.*)\{(([^\}]|\n)*)\})', fr.read())
    if shader:
        for layout in range(0, len(shader)):                  #For each layout
            fc.write(translateStructDef(shader[layout]))          #Write .hpp
            # fh.write(translateStructDecl(shader[layout]))         #Write .cpp
            fh.write(textwrap.indent(translateStructDecl(shader[layout][4], shader[layout][5].strip()), '\t'))          #Write .hpp

    fh.write('}')                               #Write namespace closing bracket
    fc.write('}')                               #Write namespace closing bracket



#TODO ADD STRUCTURE PARSING AND TRANSLATION