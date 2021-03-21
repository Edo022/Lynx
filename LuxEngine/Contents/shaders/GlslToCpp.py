import sys, os, re




pathr = sys.argv[1]
shname = ''

if not os.path.exists(pathr): print("File does not exist")
elif not pathr.split('.')[-1] == 'comp': print('File is not a vulkan shader')
else:
    shname = '.'.join(pathr.split('.')[0 : -1])
    print('Writing ' + shname)




with open(pathr, 'r') as fr, open(shname + '.hpp', 'w') as fh, open(shname + '.cpp', 'w') as fc:
    fh.write(
        '\n//This file was generated automatically. Changes could be overwritten without notice\n'+\
        '#pragma once'                                   +'\n'+\
        '#include <LuxEngine/Types/Vectors/Vectors.hpp>' +'\n'+\
        '#include <LuxEngine/Types/VPointer.hpp>'        +'\n'+\
        '#include <LuxEngine/Types/Shader_t.hpp>'        +'\n\n\n\n'\
        'namespace lux::shd::' + shname.replace('.', '_') + '{'
    )
    fc.write(
        '\n//This file was generated automatically. Changes could be overwritten without notice\n'\
        '#include <' + shname + '.hpp>'        +'\n\n\n\n'\
        'namespace lux::shd::' + shname.replace('.', '_') + '{'
    )

    sh = fr.read()
    r = re.findall(r'([\t ]*layout.*(\(.*binding[\t ]*=[\t ]([0-9][0-9]?)\))[\t ]*(buffer|uniform) (.*)\{(([^\}]|\n)*)\})', sh)
    # 0 -            ([\t ]*layout.*(\(.*binding[\t ]*=[\t ]([0-9][0-9]?)\))[\t ]*(buffer|uniform) (.*)\{(([^\}]|\n)*)\})
    # 1 -                           (\(.*binding[\t ]*=[\t ]([0-9][0-9]?)\))
    # 2 Binding                                             ([0-9][0-9]?)
    # 3 Type                                                                      (buffer|uniform)
    # 4 Layout name                                                                                (.*)
    # 5 Layout members                                                                                   (([^\}]|\n)*)
    # 6 -                                                                                                 ([^\}]|\n)

    if r:
        for i in range(0, len(r)):                                                  #For each layout
            fh.write('\n\tstruct ' + r[i][4] + ' : public Shader_b {\n')            #Write struct name
            fh.write('\t\tstatic uint32     bind;\n')                               #Write binding
            fh.write('\t\t' + r[i][4] + '();\n')                                    #Write constructor

            fc.write('\n\tuint32 ' + r[i][4] + '::bind = ' + r[i][2] + ';')         #Define binding
            fc.write('\n\t' + r[i][4] + '::' + r[i][4] + '(){')                     #Set type
            fc.write('\n\t\tShader_b::type = ' + ('Storage' if r[i][3] == 'buffer' else 'Uniform') + ';')
            fc.write('\n\t}\n')

            members = r[i][5].strip().split('\n')                                   #Get layout members
            for m in members:                                                       #For each member
                m = m.expandtabs(4).strip()
                comment = re.search(r'//.*$', m)                                        #Remove
                if not comment is None: m = re.sub(r'//.*$', '', m)                     #And save single line comments
                oldLen = re.search(r'([biud]?vec[234].*$)|(((u?int)|float|double).*$)', m)
                fh.write(('\t\t'                                     +\
                    re.sub(    r'vec(2( *$)|( *.*?;))', r'alignas(2 * 4)' + r' f32v\g<1>',\
                    re.sub(   r'bvec(2( *$)|( *.*?;))', r'alignas(2 * 4)' +   r' bv\g<1>',\
                    re.sub(   r'ivec(2( *$)|( *.*?;))', r'alignas(2 * 4)' + r' i32v\g<1>',\
                    re.sub(   r'uvec(2( *$)|( *.*?;))', r'alignas(2 * 4)' + r' u32v\g<1>',\
                    re.sub(   r'dvec(2( *$)|( *.*?;))', r'alignas(2 * 8)' + r' f64v\g<1>',\
                    re.sub( r'vec([34]( *$)|( *.*?;))', r'alignas(4 * 4)' + r' f32v\g<1>',\
                    re.sub(r'bvec([34]( *$)|( *.*?;))', r'alignas(4 * 4)' +   r' bv\g<1>',\
                    re.sub(r'ivec([34]( *$)|( *.*?;))', r'alignas(4 * 4)' + r' i32v\g<1>',\
                    re.sub(r'uvec([34]( *$)|( *.*?;))', r'alignas(4 * 4)' + r' u32v\g<1>',\
                    re.sub(r'dvec([34]( *$)|( *.*?;))', r'alignas(4 * 8)' + r' f64v\g<1>',\
                    re.sub(     r'int(( *$)|( *.*?;))', r'alignas(1 * 4)' +  r' i32\g<1>',\
                    re.sub(    r'uint(( *$)|( *.*?;))', r'alignas(1 * 4)' +  r' u32\g<1>',\
                    re.sub(    r'bool(( *$)|( *.*?;))', r'alignas(1 * 4)' + r' bool\g<1>',\
                    re.sub(   r'float(( *$)|( *.*?;))', r'alignas(1 * 4)' +  r' f32\g<1>',\
                    re.sub(  r'double(( *$)|( *.*?;))', r'alignas(1 * 8)' +  r' f64\g<1>',\
                    m))))))))))))))).strip().ljust((len(oldLen.group(0)) + len('alignas(n * n) ')) if not oldLen is None else 0) +\
                    (comment.group(0) if not comment is None else '')).rstrip() +'\n'\
                )
            fh.write('\t};\n')                                              #Write struct closing bracket
        fh.write('}')                                                   #Write namespace closing bracket
        fc.write('}')                                                   #Write namespace closing bracket



#TODO ADD STRUCTURE PARSING AND TRANSLATION