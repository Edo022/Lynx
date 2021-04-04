#include <string>
#include <fstream>
#include <iostream>
#include <cstring>


using std::string;
string enginePath;
string helpstr = ""
   "Usage:"                                                         "\n"
   "    luxg++ [options] <files...>"                                "\n"
   ""                                                               "\n"
   "Options:"                                                       "\n"
   "    -mode=<platform><type>      Specify target platform and type. This option is always required    e.g. -mode=ld           e.g. -mode=wr"                  "\n"
   "    --build-engine              Build engine instead of user application (off by default)"                                                                  "\n"
   "    -l[<options...>]            Activate option(s) only when building for Linux                     e.g. -l[-pthread]       e.g. -l[ -pthread -Dlinux ]"    "\n"
   "    -w[<options...>]            Activate option(s) only when building for Windows                   e.g. -w[-mthread]       e.g. -w[ -mthread -Dwin10 ]"    "\n"
   "    -d[<options...>]            Activate option(s) only when building in Debug   mode               e.g. -d[-Og]            e.g. -d[ -Og -g3 ]"             "\n"
   "    -r[<options...>]            Activate option(s) only when building in Release mode               e.g. -r[-O3]            e.g. -r[ -O3 -g0 ]"             "\n"
   "    --help                      Display this information"                                                                                                   "\n"
   ""                                                               "\n"
   "    Files with extension .comp are treated as GLSL compute shaders. They are compiled into .spv by GLSLC and parsed to generate a C++ interface"            "\n"
   "        By default, the .spv has the same name of the .comp and is placed in the same directory"                                                            "\n"
   "        You can specify a different name or location with the syntax:"                                                                                      "\n"
   "        <path/to/inputfile>.comp;<path/to/outputfile>.spv;                                           e.g. ./shaders/Cube.comp;./compiled/CompiledCube.spv)" "\n"
   "    Anything else is forwarded to G++"                          "\n"
   "    G++ --help option is shadowed and cannot be used from here" "\n"
   ""                                                               "\n"
;





string escape(const char* s){
    string r;
    for(int i = 0; i < strlen(s); ++i) switch(s[i]){
        case '\\': r += "\\\\"; break;
        case '\'': r += "\\\'"; break;
        case '\"': r += "\\\""; break;
        case '\n': break; //TODO move to python? (?? maybe. idk)
        //!^ Skip eventual newlines between -<l|w|d|r>['<option>'] options quotes
        //!^     the quotes are parsed out by the linux shell.
        //!^     The C++ wrapper wraps all the arguments in single quotes,
        //!^     but they also get parsed out when calling the Python script
        case '\r': r += "\\r"; break;
        case '\t': r += "\\t"; break;
        case '\v': r += "\\v"; break;
        case '\b': r += "\\b"; break;
        case '\a': r += "\\a"; break;
        default: r += s[i];
    }
    return r;
}




int main(int argc, char* argv[]) {
    //Read engine path
    {
        FILE* epf = fopen("./.engine/enginePath", "r");
        fseek(epf, 0, SEEK_END); int epfn = ftell(epf);
        fseek(epf, 0, SEEK_SET);
        char enginePath_[epfn + 1]; enginePath_[epfn] = '\0';
        fread(enginePath_, 1, epfn, epf);
        enginePath = enginePath_;
    }


    //Parse --help out
    for(int i = 1; i < argc; ++i) if(0 == strcmp(argv[i], "--help")) {
        argv[i] = nullptr;
        std::cout << helpstr;
    }


    //If the command is not just luxg++ --help
    if(argc == 2 && argv[1] || argc > 2){
        //Pack options
        string opt = "$'";
        for(int i = 1; i < argc; ++i) if(argv[i]){
            opt += escape(argv[i]);
            if((i < argc - 1) | (i == argc - 2 && !argv[argc - 1])) opt += "\\002";
        }
        opt += "'";


        //Run command
        return system((string("python3.9 ") + enginePath + "/Build/luxg++.py " + opt).c_str());
    }
}

//g++ luxg++.cpp -std=c++2a -g0 -Ofast -xavx -o luxg++