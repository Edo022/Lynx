#include <regex>
#include <string>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <cerrno>




char pf, tp;

std::string parse(char* p){
    if(std::regex_match(std::string(p), std::regex("^\\-(d|r|l|w)\\[.*\\]$"))){
        if(p[1] == pf || p[1] == tp) return std::string(" -") + std::string(p).substr(3, strlen(p) - 4);
        else return "";
    }
    else if(std::regex_match(std::string(p), std::regex("^\\-(((d|r)|(l|w))|((l|w)|(d|r)))\\[.*\\]~"))){
        if((p[1] == pf || p[1] == tp) && (p[2] == pf || p[2] == tp)) return std::string(" -") + std::string(p).substr(4, strlen(p) - 5);
        else return "";
    }
    else return std::string(" ") + p;
}




int main(int argc, char* argv[]){
    //Read engine path
    FILE* epf = fopen("./.engine/enginePath", "r");
    fseek(epf, 0, SEEK_END);
    int epfn = ftell(epf);
    fseek(epf, 0, SEEK_SET);
    char* enginePath = (char*)malloc(epfn + 1);
    fread(enginePath, 1, epfn, epf);


    //Read platform and type
    FILE *pff, *tpf;
    pff = fopen("./.engine/platform", "r");
    tpf = fopen("./.engine/type", "r");
    pf = fgetc(pff); fclose(pff);
    tp = fgetc(tpf); fclose(tpf);


    //Create g++ command and parse user arguments
    std::string s((tp == 'd') ? "g++ -DLUX_DEBUG" : "g++");
    for(auto i = 1; i < argc; ++i) s += parse(argv[i]);
    s += " -pthread ";


    //Add app dependencies
    std::string vkdep = std::string(enginePath) + "/deps/" + (pf == 'l' ? "Linux/" : "Windows/") + "Vulkan-1.2.162.0/x86_64/";
    std::string glfwdep = std::string(enginePath) + "/deps/Shared/glfw-3.3.2/";

    std::string appDeps;
    appDeps +=
         "-I" + vkdep + "include"   +
        " -I" + glfwdep + "include" +
        " -I" + glfwdep + "deps"    +
        " -I" + enginePath          +
        " -I.";

    s += appDeps;

    //Output and run command
    std::cout << (s + '\n');
    system(s.c_str());
    return 0;
}
