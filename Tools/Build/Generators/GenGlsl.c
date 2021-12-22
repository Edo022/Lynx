//gcc GenGlsl.c -std=c11 -o GenGlsl

#define _DEFAULT_SOURCE // Required for realpath and strsep
#include <stdio.h>
#include <ctype.h>
#include <inttypes.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <linux/limits.h>

#define MAX_ERR         4100100
#define MAX_CODE_LEN    4100100		//FIXME use dynamic reallocations
#define MAX_CODE_LINES  2100100		//FIXME use dynamic reallocations or pass the size
#define MAX_TOKENS		8100100		//FIXME use dynamic reallocations








struct Line {
	uint32_t line;
	char* str;
	size_t len;
};


// op = list(reversed(sorted(list(({'val' : t[0], 'type' : 'op', 'ctgr' : t[1], 'prec' : t[2], 'assoc' : t[3]}) for t in [

enum OperatorType {
	ot_logical,		// == !=          && || ^^ <   >   !
	ot_arithmetic,	// +  -  *  /  %  &  |  ^  <<  >>  ~
	ot_assignment,	// += -= *= /= %= &= |= ^= <<= >>= =
	ot_inc_dec,		// ++n n++ --n n--         <=  >=
	ot_ternary,		// ? :
	ot_field,		// .
	ot_list,		// ,
	ot_group,		// ()
	ot_scope,		// {}
	ot_subscript,	// []
};
struct OperatorData_t {
	char* value;			// String value of the operator
	enum OperatorType type;	// Type of the operator
	int32_t precedence;		// Operator precedence. Higher values have higher precedence
	char associativity; 	//'l'(left associative), 'r'(right associative) or 'n'(non associative)
};
struct OperatorData_t op[] = { // Sorted by length
	{ "<<=", ot_assignment, 16, 'r' }, { ">>=", ot_assignment, 16, 'r' }, { "+=", ot_assignment, 16, 'r' }, { "*=", ot_assignment, 16, 'r' }, { "-=", ot_assignment, 16, 'r' }, { "/=", ot_assignment, 16, 'r' }, { "%=", ot_assignment, 16, 'r' }, { "&=", ot_assignment, 16, 'r' }, { "|=", ot_assignment, 16, 'r' }, { "^=", ot_assignment, 16, 'r' },
	{ "<=",  ot_logical,     7, 'l' }, { ">=",  ot_logical,     7, 'l' }, { "==", ot_logical,     8, 'l' }, { "!=", ot_logical,     8, 'l' }, { "&&", ot_logical,    12, 'l' }, { "||", ot_logical,    14, 'l' }, { "^^", ot_logical,    13, 'l' },
	{ "++",  ot_inc_dec,     2, 'l' }, { "--",  ot_inc_dec,     2, 'l' }, //! Can be prefix or postfix. The actual precedence and associativity is determined after the tokenization
	{ "<<",  ot_arithmetic,  6, 'l' }, { ">>" , ot_arithmetic,  6, 'l' }, { "+",  ot_arithmetic,  5, 'l' }, { "*",  ot_arithmetic,  4, 'l' }, { "-",  ot_arithmetic,  5, 'l' }, { "/",  ot_arithmetic,  4, 'l' }, { "%",  ot_arithmetic,  4, 'l' }, { "&",  ot_arithmetic,  9, 'l' }, { "|",  ot_arithmetic, 11, 'l' }, { "^",  ot_arithmetic, 10, 'l' },
	{ "<",   ot_logical,     7, 'l' }, { ">",   ot_logical,     7, 'l' }, { "=",  ot_assignment, 16, 'r' }, { "!",  ot_logical,     3, 'r' }, { "~",  ot_arithmetic,  3, 'l' }, //! Same with unary +, -, ~
	{ "?",   ot_ternary,    15, 'r' }, { ":",   ot_ternary,    15, 'r' }, { ".",  ot_field,       2, 'l' }, { ",",  ot_list,       17, 'l' }, { "(",  ot_group,       1, 'n' }, { ")",  ot_group,       1, 'n' }, { "[",  ot_subscript,   2, 'l' }, { "]",  ot_subscript,   2, 'l' }, { "{",  ot_scope,       1, 'n' }, { "}",  ot_scope,       1, 'n' }
};



const char* tokenValues[] = {
	//Types
	"b",        "u32",        "i32",        "f32",        "f64",        //Scalar types
	"bv2",      "u32v2",      "i32v2",      "f32v2",      "f64v2",      //2-component vectors
	"bv3",      "u32v3",      "i32v3",      "f32v3",      "f64v3",      //3-component vectors
	"bv4",      "u32v4",      "i32v4",      "f32v4",      "f64v4",      //4-component vectors
	"bm2",      "u32m2",      "i32m2",      "f32m2",      "f64m2",      //2x2 square matrices
	"bm3",      "u32m3",      "i32m3",      "f32m3",      "f64m3",      //3x3 square matrices
	"bm4",      "u32m4",      "i32m4",      "f32m4",      "f64m4",      //4x4 square matrices
	"bm2x2",    "u32m2x2",    "i32m2x2",    "f32m2x2",    "f64m2x2",    //2x2 matrices
	"bm2x3",    "u32m2x3",    "i32m2x3",    "f32m2x3",    "f64m2x3",    //2x3 matrices
	"bm2x4",    "u32m2x4",    "i32m2x4",    "f32m2x4",    "f64m2x4",    //2x4 matrices
	"bm3x2",    "u32m3x2",    "i32m3x2",    "f32m3x2",    "f64m3x2",    //3x2 matrices
	"bm3x3",    "u32m3x3",    "i32m3x3",    "f32m3x3",    "f64m3x3",    //3x3 matrices
	"bm3x4",    "u32m3x4",    "i32m3x4",    "f32m3x4",    "f64m3x4",    //3x4 matrices
	"bm4x2",    "u32m4x2",    "i32m4x2",    "f32m4x2",    "f64m4x2",    //4x2 matrices
	"bm4x3",    "u32m4x3",    "i32m4x3",    "f32m4x3",    "f64m4x3",    //4x3 matrices
	"bm4x4",    "u32m4x4",    "i32m4x4",    "f32m4x4",    "f64m4x4",    //4x4 matrices
	"void",                                                             //Just void

	//If-else      Loops           Flow control       Switch case
	"if",          "while",        "continue",        "switch",
	"else",        "for",          "break",           "case",
	"elif",        "do",           "return",          "default",

	//Qualifiers   Inputs          Other
	"highp",       "local",        "struct",
	"medp",        "extern",       "preicison",
	"lowp",
	"const"
};


enum TokenID {
	//Types
	t_start = 0,
	t_b      =  0,   t_u32      = 17,   t_i32      = 33,   t_f32      = 49,   t_f64      = 65,   // Scalar types
	t_bv2    =  1,   t_u32v2    = 18,   t_i32v2    = 34,   t_f32v2    = 50,   t_f64v2    = 66,   // 2-component vectors
	t_bv3    =  2,   t_u32v3    = 19,   t_i32v3    = 35,   t_f32v3    = 51,   t_f64v3    = 67,   // 3-component vectors
	t_bv4    =  3,   t_u32v4    = 20,   t_i32v4    = 36,   t_f32v4    = 52,   t_f64v4    = 68,   // 4-component vectors
	t_bm2    =  4,   t_u32m2    = 21,   t_i32m2    = 37,   t_f32m2    = 53,   t_f64m2    = 69,   // 2x2 square matrices
	t_bm3    =  5,   t_u32m3    = 22,   t_i32m3    = 38,   t_f32m3    = 54,   t_f64m3    = 70,   // 3x3 square matrices
	t_bm4    =  6,   t_u32m4    = 23,   t_i32m4    = 39,   t_f32m4    = 55,   t_f64m4    = 71,   // 4x4 square matrices
	t_bm2x2  =  7,   t_u32m2x2  = 24,   t_i32m2x2  = 40,   t_f32m2x2  = 56,   t_f64m2x2  = 72,   // 2x2 matrices
	t_bm2x3  =  8,   t_u32m2x3  = 25,   t_i32m2x3  = 41,   t_f32m2x3  = 57,   t_f64m2x3  = 73,   // 2x3 matrices
	t_bm2x4  =  9,   t_u32m2x4  = 26,   t_i32m2x4  = 42,   t_f32m2x4  = 58,   t_f64m2x4  = 74,   // 2x4 matrices
	t_bm3x2  = 10,   t_u32m3x2  = 27,   t_i32m3x2  = 43,   t_f32m3x2  = 59,   t_f64m3x2  = 75,   // 3x2 matrices
	t_bm3x3  = 11,   t_u32m3x3  = 28,   t_i32m3x3  = 44,   t_f32m3x3  = 60,   t_f64m3x3  = 76,   // 3x3 matrices
	t_bm3x4  = 12,   t_u32m3x4  = 29,   t_i32m3x4  = 45,   t_f32m3x4  = 61,   t_f64m3x4  = 77,   // 3x4 matrices
	t_bm4x2  = 13,   t_u32m4x2  = 30,   t_i32m4x2  = 46,   t_f32m4x2  = 62,   t_f64m4x2  = 78,   // 4x2 matrices
	t_bm4x3  = 14,   t_u32m4x3  = 31,   t_i32m4x3  = 47,   t_f32m4x3  = 63,   t_f64m4x3  = 79,   // 4x3 matrices
	t_bm4x4  = 15,   t_u32m4x4  = 32,   t_i32m4x4  = 48,   t_f32m4x4  = 64,   t_f64m4x4  = 80,   // 4x4 matrices
	t_void   = 16, // Just void
	t_end = 80, //! Update this value when adding new types


	// Keywords
	k_start = 1000,
	// If-else           Loops                Flow control            Switch case
	k_if   = 1001,       k_while = 1004,      k_continue = 1007,      k_switch  = 1010,
	k_else = 1002,       k_for   = 1005,      k_break    = 1008,      k_case    = 1011,
	k_elif = 1003,       k_do    = 1006,      k_return   = 1009,      k_default = 1012,

	k_highp = 1013,
	k_medp  = 1014,
	k_lowp  = 1015,      k_local  = 1017,     k_struct    = 1019,
	k_const = 1016,      k_extern = 1018,     k_preicison = 1020,
	// Qualifiers        Inputs               Other
	k_end = 1020, //! Update this value when adding new keywords


	//Operators
	o_start = 2000,
	o_set_bw_lshift = 2000,    o_log_less_eq = 2010,    o_inc = 2017,    o_bw_lshift = 2019,    o_log_less = 2029,    o_ter_0      = 2034,
	o_set_bw_rshift = 2001,    o_log_more_eq = 2011,    o_dec = 2018,    o_bw_rshift = 2020,    o_log_more = 2030,    o_ter_1      = 2035,
	o_set_sum       = 2002,    o_log_eq      = 2012,                     o_sum       = 2021,    o_set      = 2031,    o_field      = 2036,
	o_set_mul       = 2003,    o_log_diff    = 2013,                     o_mul       = 2022,    o_log_not  = 2032,    o_list       = 2037,
	o_set_sub       = 2004,    o_log_and     = 2014,                     o_sub       = 2023,    o_bw_not   = 2033,    o_lgroup     = 2038,
	o_set_div       = 2005,    o_log_or      = 2015,                     o_div       = 2024,                          o_rgroup     = 2039,
	o_set_mod       = 2006,    o_log_xor     = 2016,                     o_mod       = 2025,                          o_lsubscript = 2040,
	o_set_bw_and    = 2007,                                              o_bw_and    = 2026,                          o_rsubscript = 2041,
	o_set_bw_or     = 2008,                                              o_bw_or     = 2027,                          o_lscope     = 2042,
	o_set_bw_xor    = 2009,                                              o_bw_xor    = 2028,                          o_rscope     = 2043,
	o_end = 2043, //! Update this value when adding new operators


	e_user_defined = 1000000,  // User defined identifiers
	e_literal      = 1000001,  // Literal constants
	e_newline      = 1000002,  // Line feed
	e_identifier   = 1000003,  // User defined identifiers
	e_preprocessor = 1000004,  // # characters
	e_unknown      = 1000005,  // Anything else

	e_max = e_unknown
};




struct TypeData_t {
	char* glsltype;        // The corresponding GLSL type
	enum TokenID baseType; // Base type of the type  e.g. the base type of a f32 matrix is f32
	size_t x;              // Width   e.g. a 2x3 matrix has x = 2, a scalar type has x = 1
	size_t y;              // Height  e.g. a 2x3 matrix has y = 3, a scalar type has x = 1
	size_t align;          // Alignment of the type in bytes
};

//FIXME GLSL doesnt suppotr bool and integer matrices
struct TypeData_t typeData[] = {
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




struct LiteralData_t{
	enum TokenID type;          // Type of the value. This can only be the ID of a base type
	char value[sizeof(double)]; // Actual value. Contains a float, a double, an int or an unsigned int, depending on the type
};

struct Token{
	char* value;           // The string value of the token  e.g. "const", "uint32"
	size_t len;            // Length of the token
	enum TokenID id;       // The ID of the token or its type  e.g. t_uint32, t_f64, k_while, e_whitespace
	void* data;            // A memory block that contains a TypeData_t or a LiteralData_t depending on the type of the token
	char* leading_ws;
	char* line;            // The line that contains the token
	size_t lineNum;        // The number if the line
	size_t start;          // Index of the token's first character in its line
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




/**
 * @brief
 *
 * @param vStr
 * @param vBase min 2, max 36 //TODO
 * @return double
 */
double bstrtolf(const char* vStr, const int32_t vBase){
    double res = 0, div = 1;
    for(int32_t dec = 0; *vStr; ++vStr) {
        if(*vStr == '.') dec = 1;
        else {
			char digit = tolower(*vStr);
            double digitVal = digit - ((digit >= 'a') ? 'a' - 10 : '0');
            if(dec) { div *= vBase; res += digitVal / div; }
            else    { res *= vBase; res += digitVal; }
        }
    }
    return res;
}




//Reads all the contents of the file vFilePath
//Returns a null terminated memory block containing the data
char* readFile(const char* vFilePath){
	FILE* f = fopen(vFilePath, "r");
	fseek(f, 0, SEEK_END);
	int32_t size = ftell(f);
	rewind(f);

	char* data = malloc(size + 1);
	// fread(data, 1, size, f);
	// data[size] = '\0';
	int32_t j;
	for(int32_t i = j = 0; i < size; ++i) {
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
const char* strchrn(const char* vSrc, const char vChar, const uint32_t vIndex){ //TODO add start_from parameter
	uint32_t n = 0;
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
char* strtokn(const char* vSrc, const char vChar, const uint32_t vIndex){ //TODO add start from parameter
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


void printSyntaxError(const int32_t vLineN, const char* vLine, const char* vFile, const char* vFormat, ...){
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
//TODO replace tabs with spaces
void clear(struct Line* vLines, const size_t vNum){
	for(size_t i = 0; i < vNum; ++i){
		struct Line* l = &vLines[i];
		for(int32_t j = l->len - 1; j >= 0; --j){
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
	int32_t vCodeLen = strlen(vCode);
	int32_t i = 0;
	while(i < vCodeLen){                              //For each character
		if(vCode[i] == '"'){                                //If the character is a double quote
			int32_t strBegin = i;                                   //Save the string beginning for eventual errors
			strncat(code, &vCode[i], 1);                              //Paste opening "
			++i;                                                //Skip opening "
			while(vCode[i] != '"'){                              //For each character of the string
				strncat(code, &vCode[i], 1);                                    //Paste character
				++i;                                              //Update counter
				if(i == vCodeLen){                                 //If the string does not end
					int32_t vLineN = 0;                                      //
					for(int32_t j = 0; j < strBegin; ++j){                        //Find the line in which the string begins
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
void checkIncludeFile(const int32_t vLineN, const char* vLine, const char* vFile, const char* vName){ //TODO check vLine type
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





/**
 * @brief Checks if vLine is an include statement and returns the path of the included file
 * @param vLine The line to parse
 * @return The path of the included file, or NULL if the line is not an include statement
 */
char* isInclude(const char* vLine){
	//TODO add withespace parsing
	//TODO optimize strlen
	const char* inc_start = "#include ";			// Beginning of the include statements
	size_t i = strlen(inc_start);					// Skip the length of include_start
	char* ret = malloc(PATH_MAX);					// Allocate space for the included file path
	if(!memcmp(vLine, inc_start, i)){				// If the line is an include statement
		const char c = vLine[i];						// Check if '<' or '"' are used
		if(c == '"' || c == '<'){						// If they are
			++i;											//
			for(size_t j = 0; i < strlen(vLine); ++i, ++j){	// Get the file path
				if(vLine[i] == (c == '<' ? '>' : '"')) {	//
					ret[j] = '\0';							//
					return ret;								//Return the path
				}
				ret[j] = vLine[i];
			}
		}
	}
	return NULL;
}




/**
 * @brief Creates a code with no includes by recursively pasting all the included files together
 *     Comments are not preserved
 * @param vCode The code containing the include statements
 * @param vFile The path of the file
 * @param vLineInfo The absolute line from which the file was included. 0 if the file is not included
 * @param pNum The address of a size_t variable where to store the total number of lines
 * @return An array of Line structures of size *pNum containing the lines of all the included files
 */
struct Line* include(const char* vCode, const char* vFile, const int32_t vLineInfo, size_t* pNum){
	char* code = uncomment(vCode, vFile);
	struct Line* ret = malloc(sizeof(struct Line) * MAX_CODE_LINES); //ret[0] = '\0';
	char *line;//, lineStr[6 + 1 + 4]; //6 digits + '\0' + "/**/"
	size_t len = 0;
	for(int32_t i = 0; (line = strsep(&code, "\n")) != NULL; ++i){
		size_t lineNum = vLineInfo ? vLineInfo : i + 1;
		char* r = isInclude(line);
		if(r != NULL){								// If the line is an include statement
			checkIncludeFile(i, line, vFile, r);		// Check the included file
			char* included = readFile(r);
			size_t includedLen;
			struct Line* included2 = include(included, vFile, i + 1, &includedLen);
			memcpy(ret + len, included2, sizeof(struct Line) * includedLen);
			free(included);
			free(included2);
			len += includedLen;
			//TODO save original files
		}
		else{ // If not
			ret[len].line = lineNum;
			ret[len].len = strlen(line);//TODO rename local len
			ret[len].str = line;
			++len;
		}
	}
	*pNum = len;
	return ret;
}









// Tokenizer -------------------------------------------------------------------------------------------------------------------------------//












size_t findSpaces(const char* vLine){
	char c = *vLine;
	for(int32_t i = 0;; c = vLine[++i]) {
		if(!(c == '\t' || c == ' ')) return i;
	}
}




size_t getPreprocessor(const char* vLine, struct Token* const pToken){
	if(*vLine == '#'){
		pToken->value = strdup("#");
		pToken->len   = 1;
		pToken->id    = e_preprocessor;
		pToken->data  = NULL;
		return 1;
	}
	return 0;
}




size_t getIdentifier(const char* vLine, struct Token* const pToken){
	size_t i;
	if(isalpha(vLine[0]) || vLine[0] == '_'){
		for(i = 1; isalnum(vLine[i]) || vLine[i] == '_'; ++i);	// Get the length
		pToken->value = strndup(vLine, i);						// Save the identifier
		pToken->len   = i;										// Save the length

		// Types
		for(int32_t t = t_start; t < t_end; ++t){				// For each hard coded type
			if(!strcmp(pToken->value, tokenValues[t - t_start])){	// If it matches the current identifier
				pToken->id   = t;										// Set token id to the corresponding type id
				pToken->data = &typeData[t - t_start];					// Set token data to the hard coded data of the corresponding type
				return i;												// Return the length
			}
		}

		// Keywords
		for(int32_t t = k_start; t < k_end; ++t){				// For each hard coded keyword
			if(!strcmp(pToken->value, tokenValues[t - k_start])){	// If it matches the current identifier
				pToken->id   = t;										// Set token id to the corresponding identifier id
				pToken->data = NULL;									// Set token data to NULL
				return i;												// Return the length
			}
		}

		// User defined
		pToken->id    = e_user_defined;							// Set token id to user defined identifier
		pToken->data  = NULL;									// Set token data to NULL
		return i;												// Return the length
	}
	return 0;
}




//TODO comment
//TODO check empty literals like 0x, 0d, 0o
//TODO check decimal literals starting with .
size_t getLiteral(const char* vLine, struct Token* const pToken, const char* iLine, const uint32_t iLineNum, const char* iFileName){
	if(isdigit(vLine[0])){
		pToken->data = malloc(sizeof(struct LiteralData_t));			// Allocate a block for the data

		size_t isValid = 1, i, base;									// 1 if the literal is valid, loop index and literal length, literal base
		size_t offset = (vLine[0] == '0' && isalpha(vLine[1])) ? 2 : 0;	// 0 or 2 if the literal has a prefix
		const char* baseName;											// The complete name of the base

		// Get literal base and complete base name
		if     (vLine[0] == '0' && vLine[1] == 'b') { base =  2; baseName = "binary";      }
		else if(vLine[0] == '0' && vLine[1] == 'o') { base =  8; baseName = "octal";       }
		else if(vLine[0] == '0' && vLine[1] == 'x') { base = 16; baseName = "hexadecimal"; }
		else                                        { base = 10; baseName = "decimal";     }

		// Get literal length and type and check if it's valid
		enum TokenID* literalType = &((struct LiteralData_t*)pToken->data)->type;	// Cache the address of the type
		*literalType = t_u32;														// Default type is u32, set to f64 if the loop finds a '.'
		for(i = offset; isalnum(vLine[i]) || vLine[i] == '.'; ++i){
			if(vLine[i] != '.') isValid &= !!(base == 16 ? isxdigit(vLine[i]) : vLine[i] >= '0' && vLine[i] <= '0' + base);
			else if(*literalType == t_u32) *literalType = t_f64; else isValid = 0;
		}

		// Get and save the string value of the literal and print an error if it's not valid
		pToken->value = strndup(vLine, i);
		if(!isValid) printSyntaxError(iLineNum, iLine, iFileName, "Invalid %s literal \"%s\"", baseName, pToken->value);

		// Convert the literal to an unsigned or a double and save it in the value of the allocated data
		if(*literalType == t_u32) *(uint32_t*)(&((struct LiteralData_t*)pToken->data)->value) =  strtoul(pToken->value + offset, NULL, base);
		else                        *(double*)(&((struct LiteralData_t*)pToken->data)->value) = bstrtolf(pToken->value + offset,       base);

		pToken->len = i;			// Save the length
		pToken->id  = e_literal;	// Set token id to literal
		return i;					// Return the length
	}
	return 0;
}




size_t getUnknown(const char* vLine, struct Token* const pToken) {
	pToken->value = strndup(vLine, 1);
	pToken->len   = 1;
	pToken->id    = e_unknown;
	pToken->data  = NULL;
	return 1;
}



size_t getOperator(const char* vLine, struct Token* const pToken){
	// size_t i, opLen;
	// for(int32_t o = o_start; o < o_end; ++o){					// For each hard coded operator
	// 	opLen = strlen(op[o - o_start].value);						// Cache operator length
	// 	if(!strncmp(pToken->value, op[o - o_start].value, opLen)){	// If it matches the current operator
	// 		pToken->value = op[o - o_start].value;						// Save the operator
	// 		pToken->len   = i;											// Save the length
	// 		pToken->id    = o;											// Set token id to the corresponding operator id
	// 		pToken->data  = &op[o - o_start];							// Set token data to the hard coded data of the corresponding operator
	// 		return i;													// Return the length
	// 	}
	// }
	return 0;
}








//TODO REMOVE LINE STRDUP AND LINE FIELD
//TODO replace tabs with spaces
struct Token* tokenize(struct Line* vLines, const size_t vLineNum, const char* vFile, size_t* pNum, const char* iFileName){
	struct Token* ret = malloc(sizeof(struct Token) * MAX_TOKENS);
	size_t tok_j = 0;
	for(size_t i = 0; i < vLineNum; ++i){
		char* l = vLines[i].str;
		size_t lLen = strlen(l);
		char* leading_ws = NULL;
		for(size_t j = 0; j < lLen; ++tok_j){
			struct Token* curToken = ret + tok_j;	// Cache the address of the current token
			curToken->line    = l;					// Set the line //TODO remove line
			curToken->lineNum = i;					// Set the number of the line
			curToken->start   = j;					// Set the start index to j
			curToken->leading_ws = leading_ws ? strdup(leading_ws) : NULL; // Save leading whitespace


			// Get leading whitespace
			size_t wsLen = findSpaces(l + j);
			if(wsLen){
				leading_ws = strndup(l + j, wsLen);
				j += wsLen; --tok_j; continue;
			}
			else leading_ws = NULL;

			// Find the first token, save into the array and update j
			size_t tokLen;
			if     (tokLen = getPreprocessor (l + j, curToken)){}
			else if(tokLen = getIdentifier   (l + j, curToken)){}
			else if(tokLen = getLiteral      (l + j, curToken, l, vLines[i].line, iFileName)){}
			else if(tokLen = getOperator     (l + j, curToken)){}
			else    tokLen = getUnknown      (l + j, curToken);
			j += tokLen;
		}

		//Add newline token
		if(i < vLineNum - 1) {
			struct Token* curToken = ret + tok_j;
			curToken->leading_ws = leading_ws ? strdup(leading_ws) : NULL;

			curToken->value   = strdup("\n");
			curToken->len     = 1;
			curToken->id      = e_newline;
			curToken->data    = NULL;
			curToken->line    = l;
			curToken->lineNum = i;
			curToken->start   = 0;

			++tok_j;
		}
	}
	*pNum = tok_j;
	return ret;
}








// idk --------------------------------------------------------------------------------------------------------------------------------------//




char* translate(const struct Token* vTokens, const size_t vTokensNum){
	char* ret = malloc(MAX_CODE_LEN);
	size_t j = 0;
	for(size_t i = 0; i < vTokensNum; ++i){
		const struct Token* curTok = vTokens + i;
		if(curTok->leading_ws){
			strcpy(ret + j, curTok->leading_ws);
			j += strlen(curTok->leading_ws);
		}

		if(curTok->id == e_literal){
			struct LiteralData_t* tokData = curTok->data;
			char strValue[64];
			if(tokData->type == t_u32) snprintf(strValue, 64, "%d", *(uint32_t*)tokData->value);
			else                       snprintf(strValue, 64, "%lf",  *(double*)tokData->value);
			strcpy(ret + j, strValue);
			j += strlen(strValue);
		}
		else if(curTok->id >= t_start && curTok->id < t_end){
			strcpy(ret + j, typeData[curTok->id].glsltype);
			j +=     strlen(typeData[curTok->id].glsltype);
		}
		else{
			strcpy(ret + j, curTok->value);
			j += curTok->len;
		}
	}
	ret[j + 1] = '\0';
	return ret;
}




// Main -------------------------------------------------------------------------------------------------------------------------------------//








void run(const char* vSrc, const char* vOut){
	const char* src = realpath(vSrc, NULL); //Resolve symbolic links
	if(access(src, F_OK) != 0) printError("\"%s\": No such file or directory", vSrc);


	//Read input file
	const char* code = readFile(src);
	char* line;

	//Add hard coded version statement and parse the code
	size_t outputLinesNum;
	struct Line* outputLines = include(code, vSrc, 0, &outputLinesNum);
	clear(outputLines, outputLinesNum);

	size_t outputTokensNum;
	struct Token* outputTokens = tokenize(outputLines, outputLinesNum, vSrc, &outputTokensNum, vSrc);

	char* outputStr = translate(outputTokens, outputTokensNum);

	//Write output file
	FILE* ofile = fopen(vOut, "w");
	fprintf(ofile, "#version 450\n");
	// for(int i = 0; i < outputTokensNum; ++i) {
		// fprintf(
		// 	ofile, "{ lineNum : %09d, len : %04d, start : %04d, id : %03d, data : %x, value : \"%s\", leading : \"%s\" }\n",
		// 	outputTokens[i].lineNum, outputTokens[i].len, outputTokens[i].start, outputTokens[i].id, outputTokens[i].data, (outputTokens[i].id == e_newline) ? "\\n" : outputTokens[i].value, outputTokens[i].leading_ws
		// );
	// }
	fprintf(ofile, "%s", outputStr);
	fclose(ofile);
}






int32_t main(int32_t argc, char* argv[]){
	if(argc != 3){
		printf("GenGlsl: Wrong number of arguments");
		return 1;
	}

	run(argv[1], argv[2]);
	printf(argv[2]);
	return 0;
}