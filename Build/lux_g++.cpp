#include <string>
#include <fstream>
#include <iostream>
#include <cstring>


using std::string;
string enginePath;





string escape(const char* s){
    string r;
    for(int i = 0; i < strlen(s); ++i) switch(s[i]){
        case '\\': r += "\\\\"; break;
        case '\'': r += "\\\'"; break;
        case '\"': r += "\\\""; break;
        case '\n': r += "\\n"; break;
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

    //Pack options
    string opt = "$'";
    for(int i = 1; i < argc; ++i) {
        opt += escape(argv[i]);
        if(i < argc - 1) opt += "\\002";
    }
    opt += "'";

    //Run command
    std::cout <<  (string("python3.9 ") + enginePath + "/Build/luxg++.py " + opt) + '\n';
    return system((string("python3.9 ") + enginePath + "/Build/luxg++.py " + opt).c_str());
}

//g++ lux_g++.cpp -std=c++2a -g0 -Ofast -xavx -o lux_g++