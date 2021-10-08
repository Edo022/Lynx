import sys, re, os, pathlib, subprocess
from argparse import Namespace as ns




# Check argv length
if len(sys.argv) != 3:
    print(f'Invalid number of arguments: { len(sys.argv) }')
    sys.exit(-1)


# Check source file path
source = sys.argv[1]
output = sys.argv[2]
if not os.path.exists(source):
    print(f'Unable to find source file "{ source }"')
    sys.exit(-2)



# Get the source code and parse out unnecessary whitespace and comments
# code = subprocess.run(['g++', '-fpreprocessed', '-dD', '-E', source], capture_output = True, text = True).stdout
# ncode = subprocess.run(['g++', '-E', source], capture_output = False, text = True).stdout #TODO add include paths
ncode = subprocess.run(['g++', '-E', '-I.', '-I./src', '-D__LNX_INITIALIZER_GENERATOR__', '-DLNX_DBG', source], capture_output = True, text = True).stdout #TODO add include paths
#                                                                                    FIXME read includes. dont use hard coded LNX_DBG
# ncode:str = (
#     re.sub(r' ?([()\[\]{}+*-\/.!<>=&^|?:%,;])( )?',  r'\g<1>',      # Remove spaces near opeartors
#     re.sub(r'\n',       r'',                                        # Remove newlines
#     re.sub(r' +',       r' ',                                       # Remove whitespace
#     code.expandtabs(4)                                              # Convert tabs to spaces
# ))))


# Get init macro calls
r = list(m.groupdict() for m in re.finditer(
    r'_LNX_INITIALIZER_GENERATOR_TYPE=(?P<type>.*?),_LNX_INITIALIZER_GENERATOR_NAME=(?P<name>.*?),_LNX_INITIALIZER_GENERATOR_FULLNS=(?P<fullNs>.*?);', ncode
))


# Write initializers header
if r!= None and len(r) > 0:
    with open(output, 'w') as f:
        f.write(
            f'\n//####################################################################################'
            f'\n// This file was generated by Tools/Build/GenInitializer'
            f'\n// Changes could be overwritten without notice'
            f'\n// !Do not include or compile this file'
            f'\n//####################################################################################'
            f'\n#include "Lynx/Core/Init.hpp"\n\n'
        )

        for i, m in enumerate(r):
            id : str = f'_lnx_init_var_{ m["name"] }'

            if len(m['name']) > 4 and m['name'][-4:] == '_FUN':
                f.write(
                    f'\n\nnamespace { m["fullNs"] }::_pvt{{'
                    f'\n    struct _lnx_init_fun_{ m["name"] }_t;'
                    f'\n}}'
                )


            f.write(
                f'\n\nnamespace { m["fullNs"] }::_pvt {{'
                f'\n    used        { m["type"] } { id }_get();'
                f'\n    used static { m["type"] } { id }_v = { id }_get();'
                f'\n    extern bool { id }_is_init;/*Zero initialized before global constructors*/'
                f'\n}}'
            )