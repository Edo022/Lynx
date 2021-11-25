#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>

#define MAX_ERR         4100100
#define MAX_CODE_LEN    4100100		//FIXME use dynamic reallocations
#define MAX_CODE_LINES  2100100		//FIXME use dynamic reallocations or pass the size
#define MAX_PATH        512			//FIXME read from system informations
#define MAX_TOKENS		8100100		//FIXME use dynamic reallocations








struct Line {
	unsigned line;
	char* str;
	size_t len;
};





enum TokenID {
	//Types
	t_b,        t_u32,        t_i32,        t_f32,        t_f64,        //Scalar types
	t_bv2,      t_u32v2,      t_i32v2,      t_f32v2,      t_f64v2,      //2-component vectors
	t_bv3,      t_u32v3,      t_i32v3,      t_f32v3,      t_f64v3,      //3-component vectors
	t_bv4,      t_u32v4,      t_i32v4,      t_f32v4,      t_f64v4,      //4-component vectors
	t_bm2,      t_u32m2,      t_i32m2,      t_f32m2,      t_f64m2,      //2x2 square matrices
	t_bm3,      t_u32m3,      t_i32m3,      t_f32m3,      t_f64m3,      //3x3 square matrices
	t_bm4,      t_u32m4,      t_i32m4,      t_f32m4,      t_f64m4,      //4x4 square matrices
	t_bm2x2,    t_u32m2x2,    t_i32m2x2,    t_f32m2x2,    t_f64m2x2,    //2x2 matrices
	t_bm2x3,    t_u32m2x3,    t_i32m2x3,    t_f32m2x3,    t_f64m2x3,    //2x3 matrices
	t_bm2x4,    t_u32m2x4,    t_i32m2x4,    t_f32m2x4,    t_f64m2x4,    //2x4 matrices
	t_bm3x2,    t_u32m3x2,    t_i32m3x2,    t_f32m3x2,    t_f64m3x2,    //3x2 matrices
	t_bm3x3,    t_u32m3x3,    t_i32m3x3,    t_f32m3x3,    t_f64m3x3,    //3x3 matrices
	t_bm3x4,    t_u32m3x4,    t_i32m3x4,    t_f32m3x4,    t_f64m3x4,    //3x4 matrices
	t_bm4x2,    t_u32m4x2,    t_i32m4x2,    t_f32m4x2,    t_f64m4x2,    //4x2 matrices
	t_bm4x3,    t_u32m4x3,    t_i32m4x3,    t_f32m4x3,    t_f64m4x3,    //4x3 matrices
	t_bm4x4,    t_u32m4x4,    t_i32m4x4,    t_f32m4x4,    t_f64m4x4,    //4x4 matrices
	t_void,                                                             //Just void

	//If-else    Loops         Flow control     Switch case
	k_if,        k_while,      k_continue,      k_switch,
	k_else,      k_for,        k_break,         k_case,
	k_elif,      k_do,         k_return,        k_default,

	//Qualifiers Inputs        Other
	k_highp,     k_local,      k_struct,
	k_medp,      k_extern,     k_preicison,
	k_lowp,
	k_const,

	//Maximum value of the TokenID enum
	tokenid_max
};



struct TypeData_t {
	char* glsltype;
	enum TokenID baseType;
	size_t x;
	size_t y;
	size_t align;
};

struct LiteralData_t{
	enum TokenID type;   //The type of the value
	// const char* value;
	char value[8];       //Contains a float, a double, an int or an unsigned int
	// unsigned base;     //2, 8, 10, 16
};

enum TokenType{
	e_type,
	e_keyword,
	e_literal,
	e_identifier
};


struct Token{
	char* value;
	enum TokenType type;
	enum TokenID id;
	void* data; //Points to a TypeData_t or a LiteralData_t
};






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




//Reads all the contents of the file vFilePath
//Returns a null terminated memory block containing the data
char* readFile(const char* vFilePath){
	FILE* f = fopen(vFilePath, "r");
	fseek(f, 0, SEEK_END);
	int size = ftell(f);
	rewind(f);

	char* data = malloc(size + 1);
	// fread(data, 1, size, f);
	// data[size] = '\0';
	int j;
	for(int i = j = 0; i < size; ++i) {
		char c = fgetc(f);
		if(c != '\r'){
			data[j] = c;
			++j;
		}
	}
	data[j] = '\0';

	fclose(f);
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
	vsnprintf(vStr, MAX_ERR, vFormat, vArgs);

	printf("\n%sGenGlsl: Error:\n%s%s", bRed, vStr, nWht);
	exit(1);
}


void printSyntaxError(const int vLineN, const char* vLine, const char* vFile, const char* vFormat, ...){
	va_list vArgs; va_start(vArgs, 0);
	char vStr[MAX_ERR];
	vsnprintf(vStr, MAX_ERR, vFormat, vArgs);

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









//Removes the trailing whitespace of each line
//Consecutive newline characters are preserved
void clear(struct Line* vLines, const size_t vNum){
	for(size_t i = 0; i < vNum; ++i){
		struct Line* l = &vLines[i];
		for(int j = l->len - 1; j >= 0; --j){
			char c = l->str[j];
			if(c != '\t' && c != '\r' && c != ' ' && c != '\n') {
				l->str[j + 1] = '\0';
				l->len = j;
				break;
			}
		}
		// printf("<%d - %d/%d - \"%s\">\n", l->line, i, vNum, l->str); //TODO REMOVE
	}
	// return re.sub(r'[ \t\v]+(\n|$)', r'\g<1>', vCode)
}




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




//Creates a code with no includes by pasting all the included files together
//Returns the resulting string
//Comments are not preserved
//TODO use structs
// char* include(const char* vCode, const char* vFile, const int vLineInfo){
struct Line* include(const char* vCode, const char* vFile, const int vLineInfo, size_t* pNum){
	// printf("a"); fflush(stdout); //TODO REMOVE
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
			memcpy(ret + len, included2, sizeof(struct Line) * includedLen);
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
			ret[len].str = strdup(line); //FIXME use the string left by strsep //FIXME write somewhere that the strings in the array must not be freed as they are all in the same block
			// ret[len].str[ret[len].len - 1] = '\0'; //Remove \n
			// printf("%s\n", strdup(line));
			// strcat(ret, "\n");
			++len;
		}
	}
	free(code); //FIXME dont free and assign the strings to the array elements
	*pNum = len;
	// printf("b"); fflush(stdout); //TODO REMOVE
	return ret;
}









// Tokenizer -------------------------------------------------------------------------------------------------------------------------------//


//FIXME GLSL doesnt suppotr bool and integer matrices
const struct TypeData_t typeData[] = {
	{ "bool",    t_b, 1, 1,  4 },    { "uint",    t_u32, 1, 1,  4 },    { "int",     t_i32, 1, 1,  4 },    { "float",  t_f32, 1, 1,  4 },    { "double",  t_f64, 1, 1,  8 },
	{ "bvec2",   t_b, 2, 1,  8 },    { "uvec2",   t_u32, 2, 1,  8 },    { "ivec2",   t_i32, 2, 1,  8 },    { "vec2",   t_f32, 2, 1,  8 },    { "dvec2",   t_f64, 2, 1, 16 },
	{ "bvec3",   t_b, 3, 1, 16 },    { "uvec3",   t_u32, 3, 1, 16 },    { "ivec3",   t_i32, 3, 1, 16 },    { "vec3",   t_f32, 3, 1, 16 },    { "dvec3",   t_f64, 3, 1, 32 },
	{ "bvec4",   t_b, 4, 1, 16 },    { "uvec4",   t_u32, 4, 1, 16 },    { "ivec4",   t_i32, 4, 1, 16 },    { "vec4",   t_f32, 4, 1, 16 },    { "dvec4",   t_f64, 4, 1, 32 },
	{ "bmat2",   t_b, 2, 2,  8 },    { "umat2",   t_u32, 2, 2,  8 },    { "imat2",   t_i32, 2, 2,  8 },    { "mat2",   t_f32, 2, 2,  8 },    { "dmat2",   t_f64, 2, 2, 16 },
	{ "bmat3",   t_b, 3, 3, 16 },    { "umat3",   t_u32, 3, 3, 16 },    { "imat3",   t_i32, 3, 3, 16 },    { "mat3",   t_f32, 3, 3, 16 },    { "dmat3",   t_f64, 3, 3, 32 },
	{ "bmat4",   t_b, 4, 4, 16 },    { "umat4",   t_u32, 4, 4, 16 },    { "imat4",   t_i32, 4, 4, 16 },    { "mat4",   t_f32, 4, 4, 16 },    { "dmat4",   t_f64, 4, 4, 32 },
	{ "bmat2x2", t_b, 2, 2,  8 },    { "umat2x2", t_u32, 2, 2,  8 },    { "imat2x2", t_i32, 2, 2,  8 },    { "mat2x2", t_f32, 2, 2,  8 },    { "dmat2x2", t_f64, 2, 2, 16 },
	{ "bmat2x3", t_b, 2, 3,  8 },    { "umat2x3", t_u32, 2, 3,  8 },    { "imat2x3", t_i32, 2, 3,  8 },    { "mat2x3", t_f32, 2, 3,  8 },    { "dmat2x3", t_f64, 2, 3, 16 },
	{ "bmat2x4", t_b, 2, 4,  8 },    { "umat2x4", t_u32, 2, 4,  8 },    { "imat2x4", t_i32, 2, 4,  8 },    { "mat2x4", t_f32, 2, 4,  8 },    { "dmat2x4", t_f64, 2, 4, 16 },
	{ "bmat3x2", t_b, 3, 2, 16 },    { "umat3x2", t_u32, 3, 2, 16 },    { "imat3x2", t_i32, 3, 2, 16 },    { "mat3x2", t_f32, 3, 2, 16 },    { "dmat3x2", t_f64, 3, 2, 32 },
	{ "bmat3x3", t_b, 3, 3, 16 },    { "umat3x3", t_u32, 3, 3, 16 },    { "imat3x3", t_i32, 3, 3, 16 },    { "mat3x3", t_f32, 3, 3, 16 },    { "dmat3x3", t_f64, 3, 3, 32 },
	{ "bmat3x4", t_b, 3, 4, 16 },    { "umat3x4", t_u32, 3, 4, 16 },    { "imat3x4", t_i32, 3, 4, 16 },    { "mat3x4", t_f32, 3, 4, 16 },    { "dmat3x4", t_f64, 3, 4, 32 },
	{ "bmat4x2", t_b, 4, 2, 16 },    { "umat4x2", t_u32, 4, 2, 16 },    { "imat4x2", t_i32, 4, 2, 16 },    { "mat4x2", t_f32, 4, 2, 16 },    { "dmat4x2", t_f64, 4, 2, 32 },
	{ "bmat4x3", t_b, 4, 3, 16 },    { "umat4x3", t_u32, 4, 3, 16 },    { "imat4x3", t_i32, 4, 3, 16 },    { "mat4x3", t_f32, 4, 3, 16 },    { "dmat4x3", t_f64, 4, 3, 32 },
	{ "bmat4x1", t_b, 4, 4, 16 },    { "umat4x1", t_u32, 4, 4, 16 },    { "imat4x1", t_i32, 4, 4, 16 },    { "mat4x1", t_f32, 4, 4, 16 },    { "dmat4x1", t_f64, 4, 4, 32 },
	{ "void", t_void, 0, 0,  0 }
};


struct Token* tokenize(struct Line* vLines, const size_t vLineNum, const char* vFile, size_t* pNum){
	struct Token* ret = malloc(sizeof(struct Token) * MAX_TOKENS);
	for(int i = 0; i < vLineNum; ++i){
		ret[i].value = vLines[i].str;
	}
	*pNum = vLineNum;
	return ret;
}


// struct TokKeyword {

// };









// Main -------------------------------------------------------------------------------------------------------------------------------------//








void run(const char* vSrc, const char* vOut){
	const char* src = realpath(vSrc, NULL); //Resolve symbolic links
	if(access(src, F_OK) != 0) printError("\"%s\": No such file or directory", vSrc);


	//Read input file
	// char* code = readFile(src);
	const char* code = readFile(src);
	char* line;

	//Add hard coded version statement and parse the code
	// output = list(group(list(scope(list(tokenize(clear(include(code, vSrc, 0)), vSrc)), vSrc)), vSrc))
	size_t outputLinesNum;
	struct Line* outputLines = include(code, vSrc, 0, &outputLinesNum);
	clear(outputLines, outputLinesNum);
	size_t outputTokensNum;
	struct Token* outputTokens = tokenize(outputLines, outputLinesNum, vSrc, &outputTokensNum);


	//Write output file
	FILE* ofile = fopen(vOut, "w");
	// fprintf(ofile, "#version 450\n%s", output);
	fprintf(ofile, "#version 450\n");
	for(int i = 0; i < outputTokensNum; ++i) {
		fprintf(ofile, "%s\n", outputTokens[i].value);
		// printf("\"%s\"\n", output2[i].str); fflush(stdout);
	}
		// printf("\"%s\"\n", output2[0].str); fflush(stdout);
	// printf("aaaaaaaaaaa"); fflush(stdout);
	fclose(ofile);
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