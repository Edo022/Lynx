from .Data.Platform import Platform as pf
from .Data.Type import Type as tp
from .Data.ProjectPath import *
import textwrap
import os.path
thisdir = os.path.dirname(os.path.abspath(__file__))
vkdep = EnginePath + "/deps/" + ("Linux/" if pf == "l" else "Windows/") + "Vulkan-1.2.162.0/x86_64/"
glfwdep = EnginePath + "/deps/Shared/glfw-3.3.2/"








def getEngineDeps():
    return textwrap.indent(\
        '//Vulkan\n'                        +\
        '    "-I' + vkdep + 'include",\n'   +\
        '    "-L' + vkdep + 'lib",\n'       +\
        '    "-ldl", "-lvulkan",\n'         +\
        '//GLFW\n'                          +\
        '    "-I' + glfwdep + 'include",\n' +\
        '    "-I' + glfwdep + 'deps",\n'    +\
        '    "-L' + glfwdep + 'build/' + ('debug' if tp == 'd' else 'release') + '/src",\n' +\
        '    "-ldl", "-lrt", "-lXrandr", "-lXi", "-lXcursor", "-lXinerama", "-lX11", "-lglfw3"'
    , ' '*4*4)




def getAppDeps():
    return \
        '-I' + vkdep + 'include'            +\
        ' -I' + glfwdep + 'include'         +\
        ' -I' + glfwdep + 'deps'            +\
        ' -I' + EnginePath                  +\
        ' -I.'









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
