import textwrap







def pf():
    f = open('./.engine/platform', 'r')
    c = f.read(1)
    f.close()
    return c
def tp():
    f = open('./.engine/type', 'r')
    c = f.read(1)
    f.close()
    return c
def enginePath():
    f = open('./.engine/enginePath', 'r')
    c = f.read()
    f.close()
    return c



vkdep = enginePath() + "/deps/" + ("Linux/" if pf() == "l" else "Windows/") + "Vulkan-1.2.162.0/x86_64/"
glfwdep = enginePath() + "/deps/Shared/glfw-3.3.2/"

def getEngineDeps():
    return textwrap.indent(\
        '//Vulkan\n'                        +\
        '    "-I' + vkdep + 'include",\n'   +\
        '    "-L' + vkdep + 'lib",\n'       +\
        '    "-ldl", "-lvulkan",\n'         +\
        '//GLFW\n'                          +\
        '    "-I' + glfwdep + 'include",\n' +\
        '    "-I' + glfwdep + 'deps",\n'    +\
        '    "-L' + glfwdep + 'build/' + ('debug' if tp() == 'd' else 'release') + '/src",\n' +\
        '    "-ldl", "-lrt", "-lXrandr", "-lXi", "-lXcursor", "-lXinerama", "-lX11", "-lglfw3"'
    , ' '*4*4)







def getDebugOptions():
    return textwrap.indent(\
		'//Max debug infos, additional gprof informations, no optimizations, no inline, Lux Engine security checks\n'   +\
		'    "-g3", "-ggdb3", "-pg", "-rdynamic", "-fverbose-asm", "-DLUX_DEBUG",\n'                                    +\
		'    "-O0", "-fno-elide-constructors", "-fno-inline-small-functions", "-fno-inline"'
    , ' '*4*4)




def getReleaseOptions():
    return textwrap.indent(\
        '//No debug infos, optimizations for fast code\n' +\
	    '    "-g0", "-Ofast", "-frename-registers", "-funroll-loops"'
    , ' '*4*4)