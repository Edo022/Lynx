#include <regex>
#include <string>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <cerrno>




char pf, tp;
using string = std::string;

string parse(char* p) {
    if(std::regex_match(string(p), std::regex("^\\-(d|r|l|w)\\[.*\\]$"))) {
        if(p[1] == pf || p[1] == tp) return string(" -") + string(p).substr(3, strlen(p) - 4);
        else return "";
    }
    else if(std::regex_match(string(p), std::regex("^\\-(((d|r)|(l|w))|((l|w)|(d|r)))\\[.*\\]~"))) {
        if((p[1] == pf || p[1] == tp) && (p[2] == pf || p[2] == tp)) return string(" -") + string(p).substr(4, strlen(p) - 5);
        else return "";
    }
    else return string(" ") + p;
}




int main(int argc, char* argv[]) {
    //Read engine path
    FILE* epf = fopen("./.engine/enginePath", "r");
    fseek(epf, 0, SEEK_END); int epfn = ftell(epf);
    fseek(epf, 0, SEEK_SET);
    char* enginePath = (char*)malloc(epfn + 1);
    fread(enginePath, 1, epfn, epf);


    //Read platform and type
    FILE *pff = fopen("./.engine/platform", "r");
    FILE *tpf = fopen("./.engine/type",     "r");
    pf = fgetc(pff); fclose(pff);
    tp = fgetc(tpf); fclose(tpf);
    #define gettp() (pf == 'l' ? "Linux" : "Windows")
    #define getpf() (tp == 'd' ? "Debug" : "Release")


    //Create g++ command and parse user arguments
    auto s = string("g++")                                  +
        " -pthread" + (tp == 'd' ? " -DLUX_DEBUG -rdynamic" : "")     +
        " -DenginePath=\"\\\"" + enginePath + "\\\"\""
        " " + enginePath + "/LuxEngine/getEnginePath.cpp"
        " " + enginePath + "/LuxEngine/Core/Env.cpp"
        " " + enginePath + "/Build/" + gettp() + "/LuxEngine" + getpf()
    ;
    for(auto i = 1; i < argc; ++i) s += parse(argv[i]);


    //Add app dependencies
    auto vkdep   = string(enginePath) + "/deps/" + gettp() + "/Vulkan-1.2.162.0/x86_64/";
    // auto glfwdep = string(enginePath) + "/deps/Shared/glfw-3.3.2/";
    auto glfwdep = string(enginePath) + "/deps/Shared/glfw-3.3.3/";

    s +=
        " -DGLM_FORCE_RADIANS -DGLM_FORCE_DEPTH_ZERO_TO_ONE"
        " -I" + vkdep + "include"
        " -I" + glfwdep + "include"
        " -I" + glfwdep + "deps"
        " -I" + enginePath          +
        " -I" + "."
        " -L" + vkdep + "lib"
        // " -L" + glfwdep + "build/"  + (tp == 'd' ? "debug" : "release") + "/src"
        " -L" + glfwdep + "build/src" //FIXME
        // " -lvulkan -ldl -lrt -lXrandr -lXi -lXcursor -lXinerama -lX11 -lglfw"
        " -lvulkan -ldl -lrt -lXrandr -lXi -lXcursor -lXinerama -lX11 -lglfw3"
    ;


    //Output and run command
    std::cout << (s + "\n\n");
    system(s.c_str());
    return 0;
}

//g++ lux_g++.cpp -std=c++2a -g0 -Ofast -xavx -o lux_g++