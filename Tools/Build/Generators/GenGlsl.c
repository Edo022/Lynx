#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>

#define MAX_ERR         4100
#define MAX_CODE_LEN    4100100
#define MAX_CODE_LINES  2100100
#define MAX_PATH        512








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



char* itoa(int val, int base){
	static char buf[32] = {0};
	int i = 30;
	for(; val && i ; --i, val /= base){
		buf[i] = "0123456789abcdef"[val % base];
    }
	return &buf[i+1];
}



//Reads all the contents of the file vFilePath
//Returns a null terminated memory block containing the data
char* readFile(const char* vFilePath){
    FILE* f = fopen(vFilePath, "r");
    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    rewind(f);

    char* data = malloc(size + 1);
    fread(data, 1, size, f);
    data[size] = '\0';
    return data;
}



//Returns the address of the vIndex-th occurrence of vChar in the vSrc string
const char* strchrn(const char* vSrc, const char vChar, const unsigned vIndex){ //TODO add start_from parameter
    unsigned n = 0;
    for(const char* c = vSrc;; ++c) {
        if(*c == '\0') return NULL;
        else if(*c == vChar) ++n;
        if(n == vIndex + 1) return c;
    }
}


//Splits vSrc based on vChar and returns the vIndex-th string as a null terminated char*
//The original string is not modified
//The returned string must be freed
//Returns NULL if the vIndex-th string does not exist
char* strtokn(const char* vSrc, const char vChar, const unsigned vIndex){ //TODO add start from parameter
    const char* a = vIndex ? strchrn(vSrc, vChar, vIndex - 1) : vSrc;
    const char* b = strchr(a + !!vIndex, vChar);
    size_t len =  b ? b - a : strlen(a);
    char* ret = malloc(len);
    memcpy(ret, a + 1, len - !!len);
    ret[len] = '\0';
    return ret;
}



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
        "%s\nGenGlsl: Syntax error on line %d, file \"%s\":" //FIXME
        "\n%s%s"
        "\n    %s"
        "\n\nCompilation stopped",
        bRed, vLineN + 1, realpath(vFile, NULL), vStr, nWht, vLine
    );
    exit(2);
}










// Preprocessor ----------------------------------------------------------------------------------------------------------------------------//








//Replaces multiline comments with the same number of newlines they contain
//Single line comments are replaced with a single space
//Returns the resulting string
char* uncomment(const char* vCode, const char* vFile){
    char* code = malloc(MAX_CODE_LEN); code[0] = '\0';
    int vCodeLen = strlen(vCode);
    int i = 0;
    while(i < vCodeLen){                              //For each character
        if(vCode[i] == '"'){                                //If the character is a double quote
            int strBegin = i;                                   //Save the string beginning for eventual errors
            strncat(code, &vCode[i], 1);                              //Paste opening "
            ++i;                                                //Skip opening "
            while(vCode[i] != '"'){                              //For each character of the string
                strncat(code, &vCode[i], 1);                                    //Paste character
                ++i;                                              //Update counter
                if(i == vCodeLen){                                 //If the string does not end
                    int vLineN = 0;                                      //
                    for(int j = 0; j < strBegin; ++j){                        //Find the line in which the string begins
                        if(vCode[j] == '\n') ++vLineN;                //[...] Print a syntax error
                    }
                    // printSyntaxError(vLineN, vCode.split('\n')[vLineN], vFile, 'Unterminated string'); //FIXME rewrite
                    const char* errorLine = strchrn(vCode, '\n', vLineN);
                    printSyntaxError(vLineN, errorLine, vFile, "Unterminated string");
                }
            }
            strncat(code, &vCode[i], 1);                                    //Paste closing "
            ++i;                                              //Skip closing "
        }
        else if(i < vCodeLen - 1){                            //If there is enough space to start a comment
            if(vCode[i] == '/' && vCode[i + 1] == '/'){                          //If the character is the beginning of a single line comment
                i += 2;                                              //Ignore //
                while(i < vCodeLen && vCode[i] != '\n'){          //For each character of the comment
                    if(vCode[i + 1] == '\n') strcat(code, "\n");                                        //Add a newline as token separator
                    ++i;                                              //Update the counter and ignore the character
                }
                ++i;                                              //Ignore \n
            }
            else if(vCode[i] == '/' && vCode[i + 1] == '*'){                        //If the character is the beginning of a multiline comment
                strcat(code, " ");                                         //Add a space as token separator
                i += 2;                                              //Ignore /*
                strcat(code, "\n");
                while(i < vCodeLen && !(vCode[i] == '*' && vCode[i + 1] == '/')){    //For each character of the comment
                    if(vCode[i] == '\n'){                                //If the character is a newline
                        strcat(code, "\n");                                        //Paste the newline
                    }
                    ++i;                                              //Update the counter and ignore the other characters
                }
                i += 2;                                              //Ignore */
            }
            else{                                               //Else
                strncat(code, &vCode[i], 1);                                    //Paste the character
                ++i;                                              //Update the counter
            }
        }
        else{                                               //Else
            strncat(code, &vCode[i], 1);                                    //Paste the character
            ++i;                                              //Update the counter
        }
    }

    return code;                                         //Return the parsed code
}





//Checks if an included path is valid
//Prints an error if it's not
void checkIncludeFile(const int vLineN, const char* vLine, const char* vFile, const char* vName){ //TODO check vLine type
    //if not re.match('^' + pat['t_path'] + '$', vName) printSyntaxError(vLineN, vLine, vFile, "\"%s\" is not a valid file path", vName) //FIXME
    if(access(vName, F_OK) == 0) {
        struct stat fileStat; stat(vName, &fileStat);
        if(S_ISDIR(fileStat.st_mode)) {
            printSyntaxError(vLineN, vLine, vFile, "\"%s\" is a directory", vName);
        }
    }
    else {
        printSyntaxError(vLineN, vLine, vFile, "No such file or directory");
    }
}





//Returns the path of the included file if the line is an include statement, or NULL if it's not
char* isInclude(const char* vLine){
    //TODO add withespace parsing
    //TODO optimize strlen
    const char* s = "#include ";
    char* ret = malloc(MAX_PATH);
    if(memcmp(vLine, s, strlen(s)) == 0){
        const char c = vLine[strlen(s)];
        if(c == '"' || c == '<'){
            for(int i = strlen(s) + 1, j = 0; i < strlen(vLine); ++i, ++j){
                if(vLine[i] == (c == '<' ? '>' : '"')) {
                    ret[j] = '\0';
                    return ret;
                }
                ret[j] = vLine[i];
            }
        }
    }
    return NULL;
}



struct Line {
    unsigned line;
    char* str;
    size_t len;
};



//Creates a code with no includes by pasting all the included files together
//Returns the resulting string
//Comments are not preserved
//TODO use structs
// char* include(const char* vCode, const char* vFile, const int vLineInfo){
struct Line* include(const char* vCode, const char* vFile, const int vLineInfo, size_t* pLen){
    char* code = uncomment(vCode, vFile); //!Shredded by strsep
    struct Line* ret = malloc(sizeof(struct Line) * MAX_CODE_LINES); //ret[0] = '\0';
    char *line;//, lineStr[6 + 1 + 4]; //6 digits + '\0' + "/**/"
    // struct Line* ret;
    size_t len = 0;
    for(int i = 0; (line = strsep(&code, "\n")) != NULL; ++i){
        // sprintf(lineStr, "/*%06x*/", vLineInfo ? vLineInfo : i + 1);
        size_t lineNum = vLineInfo ? vLineInfo : i + 1;
        char* r = isInclude(line);
        if(r != NULL){                                                       // If the line is an include statement
            checkIncludeFile(i, line, vFile, r);                  // Check the included file
            char* included = readFile(r);
            size_t includedLen;
            struct Line* included2 = include(included, vFile, i + 1, &includedLen);
            // printf("%d", includedLen); //TODO REMOVE
            // strcat(ret, includedCode2);
            // ret = malloc(sizeof(struct Line*) * )
            memcpy(ret + len, included2, includedLen);
            free(included);
            free(included2);
            len += includedLen;
            //TODO save original files
        }
        else{                                                               // If not
            // strcat(ret, lineStr);
            ret[len].line = lineNum;
            ret[len].len = strlen(line);//TODO rename local len
            // strcat(ret, line);
            ret[len].str = strdup(line);
            // printf("%s\n", strdup(line));
            // strcat(ret, "\n");
            ++len;
        }
    }
    free(code);
    *pLen = len;
    return ret;
}









// Tokenizer -------------------------------------------------------------------------------------------------------------------------------//








//









// Main -------------------------------------------------------------------------------------------------------------------------------------//








void run(const char* vSrc, const char* vOut){
    const char* src = realpath(vSrc, NULL); //Resolve symbolic links
    if(access(src, F_OK) != 0) printError("\"%s\": No such file or directory", vSrc);


    //Read input file
    char* code = readFile(src);


    //Add hard coded version statement and parse the code
    // output = list(group(list(scope(list(tokenize(clear(include(code, vSrc, 0)), vSrc)), vSrc)), vSrc))
    size_t outputLen;
    struct Line* output = include(code, vSrc, 0, &outputLen);


    //Write output file
    FILE* ofile = fopen(vOut, "w");
    // fprintf(ofile, "#version 450\n%s", output);
    fprintf(ofile, "#version 450\n");
    for(int i = 0; i < outputLen; ++i) {
        // fprintf(ofile, "%s", output[i].str);
        printf("\"%s\"\n", output[i].str); fflush(stdout);
    }
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