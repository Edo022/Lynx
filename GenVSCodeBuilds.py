import sys, os
thisdir = os.path.dirname(os.path.abspath(__file__))




if len(sys.argv) != 2:
    exit(-1)

pdir = sys.argv[1]
if not os.path.exists(pdir):
    print("The specified path does not exist")
    exit(-2)




print('Those files will be created or overwritten:\n' +\
    os.path.abspath(pdir) + '/.vscode/tasks.json\n' +\
    os.path.abspath(pdir) + '/.vscode/c_cpp_properties.json\n' +\
    'Continue?'
)
if sys.stdin.read(1) == 'y':
    if not os.path.exists(pdir + "/.vscode"):
        os.mkdir(pdir + "/.vscode")

    EnginePath_ = os.path.relpath(thisdir, pdir)
    pf = open('./.engine/enginePath', 'w')
    # pf.write("ProjectPath = \"" + os.path.relpath(pdir, thisdir) + "\"\n")
    pf.write(EnginePath_)
    pf.close()

    uf = open(pdir + '/.vscode/tasks.json', 'w')
    #"cd", \"""" + EnginePath_ + '", "\\n", "python3", "-m", "' + ('Build/lux_g++').replace('/', '.') + """\",
    uf.write(\
            """[[
                //Your build
                //-l[<option>], -w[<option>], -d[<option>] and -r[<option>] can be used to specify different g++ options based on the configuration
                //e.g. -d[g3] -r[g0]        //use g3 in debug mode and g0 in release
                //e.g. -wr[g0]              //use g0 only when building in release mode for windows
                //The engine library is automatically linked
            ]]""")
    uf.close()

    os.system('python3 ' + EnginePath_ + '/Build/SetPlatform.py l')
    os.system('python3 ' + EnginePath_ + '/Build/SetType.py d')