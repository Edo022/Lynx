import sys, os, re, textwrap




pathr = sys.argv[1]
pathw = ''
shname = ''

if not os.path.exists(pathr): print("File does not exist")
elif not pathr.split('.')[-1] == 'comp': print('File is not a vulkan shader')
else:
    shname = '.'.join(pathr.split('.')[0 : -1])
    pathw = shname + '.hpp'
    print('Writing ' + pathw)




with open(pathr, 'r') as fr, open(pathw, 'w') as fw:
    fw.write(
        '#pragma once'                                   +'\n'+\
        '#include "LuxEngine/Types/Vectors/Vectors.hpp"' +'\n\n\n\n'\
        'namespace lux::shd::' + shname.replace('.', '_') + '{'
    )

    sh = fr.read()
    r = re.findall(r'([\t ]*layout.*\(.*binding.*\).* (.*)\{(([^\}]|\n)*)\})', sh)
    #                (layout.*\(.*binding.*\).* (.*)\{(([^\}]|\n)*)\})
    #                                           (.*)
    #                                                 (([^\}]|\n)*)
    #                                                  ([^\}]|\n)

    if r:
        for i in range(0, len(r)):                                  #For each layout
            fw.write('\n\tstruct ' + r[i][1] + '{\n')                   #Write struct name
            members = r[i][2].strip().split('\n')                       #Get layout members
            for m in members:                                           #For each member
                m = m.expandtabs(4).strip()
                comment = re.search(r'//.*$', m)                            #Remove
                if not comment is None: m = re.sub(r'//.*$', '', m)         #And save single line comments
                oldLen = re.search(                                  \
                            r'(([biud]?vec([234]( *$)|( *.*;)))|'   +\
                    r'((u?int)|(float)|(double)(( *$)|( *.*;))))'   ,\
                m)
                fw.write('\t\t'                                     +\
                    re.sub( r'vec([234]( *$)|( *.*;))', r'f32v\g<1>',\
                    re.sub(r'bvec([234]( *$)|( *.*;))',   r'bv\g<1>',\
                    re.sub(r'ivec([234]( *$)|( *.*;))', r'i32v\g<1>',\
                    re.sub(r'uvec([234]( *$)|( *.*;))', r'u32v\g<1>',\
                    re.sub(r'dvec([234]( *$)|( *.*;))', r'f64v\g<1>',\
                    re.sub(      r'int(( *$)|( *.*;))',  r'i32\g<1>',\
                    re.sub(     r'uint(( *$)|( *.*;))',  r'u32\g<1>',\
                    re.sub(    r'float(( *$)|( *.*;))',  r'f32\g<1>',\
                    re.sub(   r'double(( *$)|( *.*;))',  r'f64\g<1>',\
                    m))))))))).ljust(len(oldLen.group(0)) if not oldLen is None else 0) +\
                    (comment.group(0) if not comment is None else '') +'\n'\
                )
            fw.write('\t};\n')                                          #Write struct closing bracket
        fw.write('}')                                               #Write namespace closing bracket



# data = re.sub(r'\/\*(.|\n)*\*\/', '', r[i][2].split('\n'))  #Get layout members and remove multiline comments



    # if r:
    #     for i in range(0, len(r)):                                  #For each layout
    #         fw.write('\n\tstruct ' + r[i][1] + '{\n')                   #Write struct name
    #         data = r[i][2].split('\n')                                  #Get layout members and remove multiline comments
    #         for j in range(0, len(data)):                               #For each layout member
    #             if re.match(r'(^[\t ]*\/\/.*$)', data[j]) is None:          #If it's not a single line comment
    #                 line = re.search(r'([^\t ].*$)', data[j])                   #Remove whitespaces at beginning of line
    #                 if not line is None:                                        #If there is something left
    #                     fw.write('\t\t' + line.group(0) + '\n')                     #Write it in the struct
    #         fw.write('\t};\n')                                          #Write struct closing bracket
    #     fw.write('}')                                               #Write namespace closing bracket

