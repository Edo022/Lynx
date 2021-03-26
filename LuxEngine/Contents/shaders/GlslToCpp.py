import sys, os, re
from textwrap import indent, dedent
from math import ceil




def roundUp(x, b):
    return b * ceil(x / b)



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








def createFuncs(members:str):
    m = members.expandtabs(4).strip()
    ret:str = ''
    ext = None
    iext = False
    maxAlign = 0


    offset = 0
    while len(m) > 0:
        r = re.search(r'^( |\n)*', m)                               #Ignore whitespace
        if r != None:                                                   #
            m = m[len(r.group(0)):]                                     #Pop parsed string from source string


        r = re.search(r'(^\/\*(.|\n)*?\*\/)|(^\/\/.*?\n)', m)       #Skip comments
        if r != None:                                                   #
            ret += '\n' + r.group(0).strip()                            #Concatenate to return value
            m = m[len(r.group(0)):]                                     #Pop parsed string from source string


        r = re.search(                                              #Translate member
            r'^(([biuv]?vec[234])|(double|float|bool|(u?int)))'         # 1 2 3 4                 # 1     #Get type name
            r'((( |\n)|((\/\*(.|\n)*?\*\/)|(\/\/.*?\n)))*)'             # 5  6  7  8  9  10 11    # 5     #Skip any comment or whitespace
            r'([a-zA-Z_]{1,}[a-zA-Z0-9_]*)'                             # 12                      # 12    #Get variable name
            r'((( |\n)|((\/\*(.|\n)*?\*\/)|(\/\/.*?\n)))*)'             # 13 14 15 16 17 18 19    # 13    #Skip any comment or whitespace
            r'\[?'                                                      # -                       # -     #Check opening array bracket
            r'((( |\n)|((\/\*(.|\n)*?\*\/)|(\/\/.*?\n)))*)'             # 20 21 22 23 24 25 26    # 20    #Skip any comment or whitespace
            r'(\])?'                                                    # 27                      # 27    #Check closing array bracket
            r'((( |\n)|((\/\*(.|\n)*?\*\/)|(\/\/.*?\n)))*)'             # 28 29 30 31 32 33 34    # 28    #Skip any comment or whitespace
            r';'                                                        # -                       # -     #Anchor to instruction end
            r'((( )|(\/\/.*?(\n|$)))*)',                                # 35 36 37 38 39          # 35    #Skip eventual comments after member declaration
            m
        )
        if r != None:                                                   #
            _type = r.group(1); _name = r.group(12); _iext = r.group(27)#Programmer friendly names
            _coms = { r.group(5), r.group(13), r.group(20), r.group(28), r.group(35) }
            ttype:str = translateDataType(_type)                        #Translate type
            align:int = getTypeSize(_type)                              #Get member alignment
            maxAlign = max(maxAlign, align)                             #Recalculate maximum alignment


            for _comm in _coms:                                         #Write comments
                if _comm != None and len(_comm.strip()) > 0: ret += '\n' + dedent(_comm.rstrip())

            if _iext == None:                                           #If the binding is external
                ret += '\nalwaysInline ' + ttype + '& '                     #Write translated type
                offset = roundUp(offset, align)
                # if offset % align != 0: offset = offset - offset % align + align     #Fix element offset and    #Create getter from variable name
                ret += _name + '() { return *(' + ttype + '*)' + ('(ShaderElm_b::data + ' + str(offset) + ')' if offset != 0 else 'ShaderElm_b::data') + '; }'
                offset += align                                             #Calculate raw offset of the next element

            m = m[len(r.group(0)):]                                     #Pop parsed string from source string
            if iext: break                                              #Break if the binding is external and the trailing comment has been written
            if _iext != None:                                           #If the binding is external
                iext = True                                                 #Set external binding variable
                ext = (ttype, _name)                                        #Save binding type and name. They will be used when writing create()
            continue                                                    #Keep parsing


        #umu umu
        else:                                                           #Copy anything else
            m = m[1:]                                                       #

    # return dict({ 'func' : ret, 'ext' : ext, 'size' : offset if (offset % maxAlign == 0) else offset - offset % maxAlign + maxAlign })
    # return dict({ 'func' : ret, 'ext' : ext, 'size' : offset + 64})
    return dict({ 'func' : ret, 'ext' : ext, 'size' : roundUp(offset, max(maxAlign, 16)) }) #Structure size must be a multiple of 16








def translateStructDecl(name:str, type:str, binding:int, members:str, space:bool):
    t = createFuncs(members)
    return dict({
        'decl' : (('\n\n' if space else '') +                                   #Fix spacing
            '\nstruct ' + name + '_t : public ShaderElm_b<' + ('Storage' if type == 'buffer' else 'Uniform') + '> {' +
            indent(                                                    # ^ Struct declaration {
                '\n' + name + '_t() {' + (                                          #Constructor {
                    ('\n\tShaderElm_b::vdata.realloc(' + str(t['size']) + ');') +       #Allocate gpu data
                    ('\n\tShaderElm_b::data.realloc('  + str(t['size']) + ');')         #Allocate local data copy
                    if t['ext'] is None else ''                                         #But only if the binding is not ext
                    ) +                                                                 #
                    '\n\tShaderElm_b::bind = ' + str(binding) + ';'                     #Set binding index
                '\n}' +                                                             # }
                t['func'],                                                          #Member access functions(){ ... }
            '\t') +                                                                 #
            '\n} ' + name + ';'                                                 # } MemberDeclaration;
        ),

        'ext': t['ext']                                                         #Forward ext
    })








pathr  = sys.argv[1]

if not os.path.exists(pathr): print("File does not exist")
elif not pathr.split('.')[-1] == 'comp': print('File is not a vulkan shader')
else:
    spath  = re.search(r'^(.*/).*$', pathr)
    spath = spath.group(1) if spath != None else './'

    shname = re.search(r'^(.*/)?(.*)\..*$', pathr)
    if shname != None: shname = shname.group(2)

    print('Compiling ' + spath + shname)




with open(spath + shname + '.comp', 'r') as fr, open(spath + shname + '.hpp', 'w') as fh:
    fh.write(
        '\n//This file was generated automatically. Changes could be overwritten without notice'
        '\n#pragma once'
        '\n#include <LuxEngine/Types/Vectors/Vectors.hpp>'
        '\n#include <LuxEngine/Types/Pointer.hpp>'
        '\n#include <LuxEngine/Types/VPointer.hpp>'
        '\n#include <LuxEngine/Types/Shader_t.hpp>\n\n\n'
        '\nnamespace lux::shd{'
        '\n\tstruct ' + re.sub(r'^([0-9].*)$', r'_\g<1>', re.sub(r'[^a-zA-Z0-9_]', '_', shname)) + ' : public Shader_b {'
    )

    shader = re.findall(
        r'((^|\n)((\/\*(.|\n)*?\*\/)|[\t ])*'   # 0 1 2 3    # -      #Skip comments and whitespace
        r'layout.*(\(.*binding[\t ]*=[\t ]'     # 4 5        # -      #Find layout declaration
        r'([0-9][0-9]?)\))'                     # 6          # 6      #Get layout binding
        r'[\t ]*(buffer|uniform) (.*)'          # 7 8        # 7 8    #Get layout type and name
        r'\{(([^\}]|\n)*)\})',                  # 9 10       # 9      #Get layout members
        fr.read())
    if shader != None:
        exts:list = []
        for layout in range(0, len(shader)):                    #For each layout
            decl = translateStructDecl(
                name    = shader[layout][8],
                type    = shader[layout][7],
                binding = shader[layout][6],
                members = shader[layout][9].strip(),
                space   = layout != 0
            )
            fh.write(indent(decl['decl'], '\t\t'))
            if(decl['ext'] != None): exts.insert(len(exts), (decl['ext'][0], decl['ext'][1], shader[layout][8]))

        fh.write(indent(
            '\n\n\nvoid create(' + ', '.join(('vram::ptr<' + elm[0] + ', VRam, Storage> p' + elm[1][0].upper() + elm[1][1:]) for elm in exts) + '){' +
            ''.join(('\n\t' + elm[2] + '.vdata = (vram::ptr<char, VRam, Storage>)p' + elm[1][0].upper() + elm[1][1:] + ';') for elm in exts) +
            '\n}',
        '\t\t'))

    else:
        print('No layout found. A shader must define at least one layout')

    fh.write('\n\t};\n}');                      #Write namespace closing bracket
    fh.write('//TODO remove local data in external bindings') #TODO

#TODO ADD STRUCTURE PARSING AND TRANSLATION