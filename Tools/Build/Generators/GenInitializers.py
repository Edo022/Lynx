import os, sys, re, pathlib, subprocess, Utils as u
from argparse import Namespace as ns




# Check argv length
if len(sys.argv) != 4:
    raise Exception(f'Invalid number of arguments: { len(sys.argv) }')


# Read inputs
tmp    : str  = sys.argv[1]          # Input file
output : str  = sys.argv[2]          # Output file
flags  : list = eval(sys.argv[3])    # Flags to use for gcc



# Read macro expansions
r = list(m.groupdict() for m in re.finditer(
    r'_LNX_PARSE_INITIALIZER_GENERATOR_TYPE=(?P<type>.*?),'
    r'_LNX_PARSE_INITIALIZER_GENERATOR_NAME=(?P<name>.*?),'
    r'_LNX_PARSE_INITIALIZER_GENERATOR_FULLNS=(?P<fullNs>.*?);',
    u.preprocessCpp(tmp, [*flags, '-D__LNX_PARSE_INITIALIZER_GENERATOR__', '-D__LNX_PARSE_NO_INCLUDES__'])
))




# Write initializers header
if r!= None and len(r) > 0:
    with open(output, 'w') as f:
        f.write(
            f'\n//####################################################################################'
            f'\n// This file was generated by Tools/Build/Generators/GenInitializer'
            f'\n// Changes could be overwritten without notice'
            f'\n// !Do not include or compile this file'
            f'\n//####################################################################################'
            f'\n#include "Lynx/Core/Init.hpp"\n\n'
        )

        for i, m in enumerate(r):
            id : str = f'_lnx_init_var_{ m["name"] }'


            f.write(
                f'\n\nnamespace { m["fullNs"] }::_pvt {{'
                f'\n    extern bool { id }_is_init; //!Zero initialized'
                f'\n    extern      { m["type"] }* { id }_v;'
                f'\n'
                f'\n    struct { id }_init_t_call_t{{'
                f'\n        { id }_init_t_call_t();'
                f'\n        void set({ m["type"] }* pVar);' #FIXME PASS THE POINTER WITH THE TYPE WHEN USING ARRAYS
                f'\n    }};'
                f'\n    used static { id }_init_t_call_t { id }_init_t_call_v;'
                f'\n}}'
            )
else:
    raise Exception('Unable to read generator data from variable declared as initializer')