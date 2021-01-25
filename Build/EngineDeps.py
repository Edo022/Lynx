import os
import Platform
import ProjectPath

def getEngineDeps():
    s = "    " * 4
    absp = os.path.abspath(".") + "/"
    depp = absp + "deps/"
    spdepp = depp + ("Linux/" if Platform.Platform == "l" else "Windows/")

    return \
    s + "//Vulkan\n"                                                    + \
    s +     "    \"-I" + spdepp + "Vulkan-1.2.162.0/x86_64/include\",\n"+ \
    s +     "    \"-L" + spdepp + "Vulkan-1.2.162.0/x86_64/lib\",\n"    + \
    s +   """    "-ldl", "-lvulkan",\n"""                               + \
    s + "//GLFW\n"                                                      + \
    s +     "    \"-I" + depp + "Shared/glfw-3.3.2/include\",\n"        + \
    s +     "    \"-I" + depp + "Shared/glfw-3.3.2/deps\",\n"           + \
    s +     "    \"-L" + depp + "Shared/glfw-3.3.2/build/debug/src\",\n"+ \
    s +   """    "-ldl", "-lrt", "-lXrandr", "-lXi", "-lXcursor", "-lXinerama", "-lX11", "-lglfw3\""""
