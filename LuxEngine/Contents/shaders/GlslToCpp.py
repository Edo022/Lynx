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








def createFuncs(members:str, iext:bool):
    m = members.expandtabs(4).strip()
    ret      : str             = ''
    ext      : tuple(str, str) = None
    iext_    : bool            = False
    maxAlign : int             = 0


    offset = 0
    while len(m) > 0:
        r = re.search(r'^( |\n){1,}', m)                            #Ignore whitespace
        if r != None:                                                   #
            m = m[len(r.group(0)):]                                     #Pop parsed string from source string
            continue                                                    #Keep parsing


        r = re.search(r'(^\/\*(.|\n)*?\*\/)|(^\/\/.*?\n)', m)       #Skip comments
        if r != None:                                                   #
            ret += '\n' + r.group(0).strip()                            #Concatenate to return value
            m = m[len(r.group(0)):]                                     #Pop parsed string from source string
            continue                                                    #Keep parsing


        if iext_: break                                             #Break if the binding is external and the trailing comment has been written
        r = re.search(                                              #Search for members
            r'^()*?'                                                    # -                       # -     #Anchor to beginning
            r'((( |\n)|((\/\*(.|\n)*?\*\/)|(\/\/.*?\n)))*)'             # 2  3  4  5  6  7  8     # 2     #Skip any comment or whitespace
            r'(([biuv]?vec[234])|(double|float|bool|(u?int)))'          # 9  10 11 12             # 9     #Get type name
            r'((( |\n)|((\/\*(.|\n)*?\*\/)|(\/\/.*?\n)))*)'             # 13 14 15 16 17 18 19    # 13    #Skip any comment or whitespace
            r'([a-zA-Z_]{1,}[a-zA-Z0-9_]*)'                             # 20                      # 20    #Get variable name
            r'((( |\n)|((\/\*(.|\n)*?\*\/)|(\/\/.*?\n)))*)'             # 21 22 23 24 25 26 27    # 21    #Skip any comment or whitespace
            r'\[?'                                                      # -                       # -     #Check opening array bracket
            r'((( |\n)|((\/\*(.|\n)*?\*\/)|(\/\/.*?\n)))*)'             # 28 29 30 31 32 33 34    # 28    #Skip any comment or whitespace
            r'(\])?'                                                    # 35                      # 35    #Check closing array bracket
            r'((( |\n)|((\/\*(.|\n)*?\*\/)|(\/\/.*?\n)))*)'             # 36 37 38 39 40 41 42    # 36    #Skip any comment or whitespace
            r';'                                                        # -                       # -     #Anchor to instruction end
            r'((( )|(\/\/.*?(\n|$)))*)',                                # 43 44 45 46 47          # 43    #Skip eventual comments after member declaration
        m)                                                              #
        if r != None:                                               #If a member was found
            _type = r.group(9); _name = r.group(20); _iarr = r.group(35)#(Programmer friendly names)
            _coms = { r.group(2), r.group(13), r.group(21), r.group(28), r.group(36), r.group(43) }
            ttype:str = translateDataType(_type)                        #Translate its type
            align:int = getTypeSize(_type)                              #Get member alignment
            maxAlign = max(maxAlign, align)                             #Recalculate maximum alignment


            for _comm in _coms:                                         #Write comments
                if _comm != None and len(_comm.strip()) > 0: ret += '\n' + dedent(_comm.rstrip())

            if not iext:                                                #If the binding is not external
                ret += '\nalwaysInline ' + ttype + '& '                     #Write translated type
                offset = roundUp(offset, align)                             #Fix element offset and    #Create getter from variable name
                ret += _name + '() { return *(' + ttype + '*)' + ('(ShaderElm_b::data + ' + str(offset) + ')' if offset != 0 else 'ShaderElm_b::data') + '; }'
                offset += align                                             #Calculate raw offset of the next element
            else:                                                       #If the binding is external
                iext_ = True                                                #Set external binding variable
                ext = (ttype, _name)                                        #Save binding type and name. They will be used when writing create()

            m = m[len(r.group(0)):]                                     #Pop parsed string from source string
            continue                                                    #Keep parsing


        #umu umu
        else:                                                       #Blindly copy anything else
            m = m[1:]                                                   #

    # return dict({ 'func' : ret, 'ext' : ext, 'size' : offset + 64})
    # return dict({ 'func' : ret, 'ext' : ext, 'size' : roundUp(offset, max(maxAlign, 16)) + 64 }) #Structure size must be a multiple of 16
    return dict({ 'func' : ret, 'ext' : ext, 'size' : roundUp(offset, max(maxAlign, 16)) }) #Structure size must be a multiple of 16








def translateStructDecl(name:str, iext:bool, type:str, binding:int, members:str, space:bool):
    t = createFuncs(members, iext)
    return dict({
        'decl' : (('\n\n' if space else '') +                                   #Fix spacing
            '\nstruct ' + name + '_t : public ShaderElm_b<' + ('Storage' if type == 'buffer' else 'Uniform') + '> {' +
            indent(                                                             # ^ Struct declaration {
                '\n' + name + '_t() {' + (                                          #Constructor {
                    ('\n\tShaderElm_b::vdata.realloc(' + str(t['size']) + ');') +       #Allocate gpu data
                    ('\n\tShaderElm_b::data.realloc('  + str(t['size']) + ');')         #Allocate local data copy
                    if not iext else ''                                                 #But only if the binding is not ext
                    ) +                                                                 #
                    '\n\tShaderElm_b::bind = ' + str(binding) + ';'                     #Set binding index
                '\n}' +                                                             # }
                t['func'],                                                          #Member access functions(){ ... }
            '\t') +                                                                 #
            '\n} ' + name + ';'                                                 # } MemberDeclaration;
        ),

        'ext': t['ext']                                                         #Forward external binding type and name
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
    fh.write(                                           #Write to file
        '\n//This file was generated automatically. Changes could be overwritten without notice'
        '\n#pragma once'                                    #Include guard
        '\n#include <LuxEngine/Types/Vectors/Vectors.hpp>'  #Base types
        '\n#include <LuxEngine/Types/VPointer.hpp>'         #GPU pointers
        '\n#include <LuxEngine/Types/Shader_t.hpp>\n\n\n'   #Base Shader struct
        '\nnamespace lux::shd{'                             #Write namespace and struct declaration
        '\n\tstruct ' + re.sub(r'^([0-9].*)$', r'_\g<1>', re.sub(r'[^a-zA-Z0-9_]', '_', shname)) + ' : public Shader_b {'
    )

    shader = re.findall(                                #Search for binding declarations
        r'(( |\n)|((\/\*(.|\n)*?\*\/)|(\/\/.*?\n)))*'       # 0 1 2 3 4 5       # -      #Skip comments and whitespace
        r'(ext)?'                                           # 6                 # 6      #Skip comments and whitespace
        r'(( |\n)|((\/\*(.|\n)*?\*\/)|(\/\/.*?\n)))*'       # 7 8 9 10 11 12    # -      #Skip comments and whitespace
        r'layout.*(\(.*binding[\t ]*=[\t ]'                 # 13                # -      #Find layout declaration
        r'([0-9][0-9]?)\))'                                 # 14                # 14     #Get layout binding
        r'[\t ]*(buffer|uniform) (.*)'                      # 15 16             # 15 16  #Get layout type and name
        r'\{(([^\}]|\n)*)\}',                               # 17 18             # 17     #Get layout members
        fr.read())                                          #
    if shader != None:                                  #If there is at leat one binding
        exts:list = []                                      #External bindings data
        for layout in shader:                               #For each layout
            _iext = layout[6] != None and len(layout[6]) > 0    #Check if it's external        #BUG CHECK LENGTH IN MEMBER PERSING TOO
            decl = translateStructDecl(                         #Translate its members
                name    = layout[16],                               #Layout name
                iext    = _iext,                                    #True if the binding is external. used to improve parsing performance
                type    = layout[15],                               #Uniform or Storage
                binding = layout[14],                               #Binding index
                members = layout[17].strip(),                       #Raw members data
                space   = layout != 0                               #Code spacing
            )                                                       #
            fh.write(indent(decl['decl'], '\t\t'))              #Write members to file
            if _iext:                                           #If it's external, save its data
                exts.insert(len(exts), (decl['ext'][0], decl['ext'][1], layout[16]))

        fh.write(indent(                                    #Write shader's create function
            '\n\n\nvoid create(' + ', '.join(('vram::ptr<' + elm[0] + ', VRam, Storage> p' + elm[1][0].upper() + elm[1][1:]) for elm in exts) + '){' +
            ''.join(('\n\t' + elm[2] + '.vdata = (vram::ptr<char, VRam, Storage>)p' + elm[1][0].upper() + elm[1][1:] + ';') for elm in exts) +
            '\n}',
        '\t\t'))

    else:
        print('No layout found. A shader must define at least one layout')

    fh.write('\n\t};\n}');                              # } //Namespace
    fh.write('//TODO remove local data in external bindings') #TODO

#TODO ADD STRUCTURE PARSING AND TRANSLATION