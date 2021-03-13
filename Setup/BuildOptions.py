import textwrap


def pf():
    try :
        with open('./.engine/platform', 'r') as f:
        	return f.read(1)
    except FileNotFoundError:
        return ''

def tp():
    try:
        with open('./.engine/type', 'r') as f:
        	return f.read(1)
    except FileNotFoundError:
        return ''
def enginePath():
    with open('./.engine/enginePath', 'r') as f:
   		return f.read()



vkdep = enginePath() + "/deps/" + ("Linux/" if pf() == "l" else "Windows/") + "Vulkan-1.2.170.0/x86_64/"
# glfwdep = enginePath() + "/deps/Shared/glfw-3.3.2/"
glfwdep = enginePath() + "/deps/Shared/glfw-3.3.3/"

def getEngineDeps():
    #TODO REMOVE LIB DEPENDENCIES. theyre useless when not linking
    return textwrap.indent(\
        '//Vulkan\n'                        +\
        '   "-DGLM_FORCE_RADIANS", "-DGLM_FORCE_DEPTH_ZERO_TO_ONE",\n'+\
        '    "-I' + vkdep + 'include",\n'   +\
        # '    "-L' + vkdep + 'lib",\n'       +\
        # '    "-ldl", "-lvulkan",\n'         +\
        '//GLFW\n'                          +\
        '    "-I' + glfwdep + 'include",\n' +\
        '    "-I' + glfwdep + 'deps"'\
        # '    "-L' + glfwdep + 'build/src",\n' +\
        # '    "-ldl", "-lrt", "-lXrandr", "-lXi", "-lXcursor", "-lXinerama", "-lX11", "-lglfw3"'
    , ' '*4*4)


        # '    "-L' + glfwdep + 'build/' + ('debug' if tp() == 'd' else 'release') + '/src",\n' +\
        # '    "-ldl", "-lrt", "-lXrandr", "-lXi", "-lXcursor", "-lXinerama", "-lX11", "-lglfw"'


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