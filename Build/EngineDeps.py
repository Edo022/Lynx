import Platform
import ProjectPath
import textwrap
import os.path
thisdir = os.path.dirname(os.path.abspath(__file__))




def getEngineDeps():
    depp = os.path.abspath(__file__) + "/deps/"
    spdepp = depp + ("Linux/" if Platform.Platform == "l" else "Windows/")

    return textwrap.indent(\
        '//Vulkan\n'                                                    +\
            '    "-I' + spdepp + 'Vulkan-1.2.162.0/x86_64/include",\n'  +\
            '    "-L' + spdepp + 'Vulkan-1.2.162.0/x86_64/lib",\n'      +\
            '    "-ldl", "-lvulkan",\n'                                 +\
        '//GLFW\n'                                                      +\
            '    "-I' + depp + 'Shared/glfw-3.3.2/include",\n'          +\
            '    "-I' + depp + 'Shared/glfw-3.3.2/deps",\n'             +\
            '    "-L' + depp + 'Shared/glfw-3.3.2/build/debug/src",\n'  +\
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