#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>

#define MAX_ERR 4000










// Utils ------------------------------------------------------------------------------------------------------------------------------------//








//TODO move to utils
//TODO add c bindings
const char *nBlk = "\033[0;30m", *bBlk = "\033[1;30m", *uBlk = "\033[4;30m";
const char *nRed = "\033[0;31m", *bRed = "\033[1;31m", *uRed = "\033[4;31m";
const char *nGrn = "\033[0;32m", *bGrn = "\033[1;32m", *uGrn = "\033[4;32m";
const char *nYel = "\033[0;33m", *bYel = "\033[1;33m", *uYel = "\033[4;33m";
const char *nBlu = "\033[0;34m", *bBlu = "\033[1;34m", *uBlu = "\033[4;34m";
const char *nMag = "\033[0;35m", *bMag = "\033[1;35m", *uMag = "\033[4;35m";
const char *nCyn = "\033[0;36m", *bCyn = "\033[1;36m", *uCyn = "\033[4;36m";
const char *nWht = "\033[0;37m", *bWht = "\033[1;37m", *uWht = "\033[4;37m";





void printError(const char* vFormat, ...){
    va_list vArgs; va_start(vArgs, 0);
    char vStr[MAX_ERR];
    vsprintf(vStr, vFormat, vArgs);

    printf("\n%sGenGlsl: Error:\n%s%s", bRed, vStr, nWht);
    exit(1);
}


void printSyntaxError(const int vLineN, const char* vLine, const char* vFile, const char* vFormat, ...){
    va_list vArgs; va_start(vArgs, 0);
    char vStr[MAX_ERR];
    vsprintf(vStr, vFormat, vArgs);

    printf(
        "%s\nGenGlsl: Syntax error on line %d, file \"{ os.path.relpath(vFile, \".\") }\":" //FIXME
        "\n%s%s"
        "\n    %s"
        "\n\nCompilation stopped",
        bRed, vLineN + 1, vStr, nWht, vLine
    );
    exit(2);
}







const char* include(const char* vCode, const char* vFile, const int vLineInfo){
    return vCode;
}





void run(const char* vSrc, const char* vOut){
    const char* src = realpath(vSrc, NULL); //Resolve symbolic links
    if(access(src, F_OK) != 0) printError("\"%s\": No such file or directory", vSrc);


    //Read input file
    FILE* ifile = fopen(src, "r");                                      //Open file stream
    fseek(ifile, 0, SEEK_END); int isize = ftell(ifile); rewind(ifile); //Get file size
    char* code = malloc(isize + 1); fread(code, 1, isize, ifile);       //Read contents


    //Add hard coded version statement and parse the code
    // output = list(group(list(scope(list(tokenize(clear(include(code, vSrc, 0)), vSrc)), vSrc)), vSrc))
    const char* output = include(code, vSrc, 0);


    //Write output file
    FILE* ofile = fopen(vOut, "w");
    fprintf(ofile, "#version 450\n%s", output);
}






int main(int argc, char* argv[]){
	if(argc != 3){
		printf("GenGlsl: Wrong number of arguments");
		return 1;
	}

	run(argv[1], argv[2]);
    printf(argv[2]);
	return 0;
}