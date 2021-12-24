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
#define MAX_CODE_LINES  800100100		//FIXME use dynamic reallocations or pass the size
#define MAX_TOKENS		8100100		//FIXME use dynamic reallocations

//FIXME GLSL doesnt suppotr bool and integer matrices
//TODO check returns values








// Tokens -----------------------------------------------------------------------------------------------------------------------------------//








enum TokenID {
	//Types
	t_start = 0,
	t_b     =  0,   t_u32     =  1,   t_i32     =  2,   t_f32     =  3,   t_f64     =  4,   // Scalar types
	t_bv2   =  5,   t_u32v2   =  6,   t_i32v2   =  7,   t_f32v2   =  8,   t_f64v2   =  9,   // 2-component vectors
	t_bv3   = 10,   t_u32v3   = 11,   t_i32v3   = 12,   t_f32v3   = 13,   t_f64v3   = 14,   // 3-component vectors
	t_bv4   = 15,   t_u32v4   = 16,   t_i32v4   = 17,   t_f32v4   = 18,   t_f64v4   = 19,   // 4-component vectors
	t_bm2   = 20,   t_u32m2   = 21,   t_i32m2   = 22,   t_f32m2   = 23,   t_f64m2   = 24,   // 2x2 square matrices
	t_bm3   = 25,   t_u32m3   = 26,   t_i32m3   = 27,   t_f32m3   = 28,   t_f64m3   = 29,   // 3x3 square matrices
	t_bm4   = 30,   t_u32m4   = 31,   t_i32m4   = 32,   t_f32m4   = 33,   t_f64m4   = 34,   // 4x4 square matrices
	t_bm2x2 = 35,   t_u32m2x2 = 36,   t_i32m2x2 = 37,   t_f32m2x2 = 38,   t_f64m2x2 = 39,   // 2x2 matrices
	t_bm2x3 = 40,   t_u32m2x3 = 41,   t_i32m2x3 = 42,   t_f32m2x3 = 43,   t_f64m2x3 = 44,   // 2x3 matrices
	t_bm2x4 = 45,   t_u32m2x4 = 46,   t_i32m2x4 = 47,   t_f32m2x4 = 48,   t_f64m2x4 = 49,   // 2x4 matrices
	t_bm3x2 = 50,   t_u32m3x2 = 51,   t_i32m3x2 = 52,   t_f32m3x2 = 53,   t_f64m3x2 = 54,   // 3x2 matrices
	t_bm3x3 = 55,   t_u32m3x3 = 56,   t_i32m3x3 = 57,   t_f32m3x3 = 58,   t_f64m3x3 = 59,   // 3x3 matrices
	t_bm3x4 = 60,   t_u32m3x4 = 61,   t_i32m3x4 = 62,   t_f32m3x4 = 63,   t_f64m3x4 = 64,   // 3x4 matrices
	t_bm4x2 = 65,   t_u32m4x2 = 66,   t_i32m4x2 = 67,   t_f32m4x2 = 68,   t_f64m4x2 = 69,   // 4x2 matrices
	t_bm4x3 = 70,   t_u32m4x3 = 71,   t_i32m4x3 = 72,   t_f32m4x3 = 73,   t_f64m4x3 = 74,   // 4x3 matrices
	t_bm4x4 = 75,   t_u32m4x4 = 76,   t_i32m4x4 = 77,   t_f32m4x4 = 78,   t_f64m4x4 = 79,   // 4x4 matrices
	t_void  = 80, // Just void
	t_end = 80, //! Update this value when adding new types


	// Keywords
	k_start = 1000,
	k_if       = 1001,    k_else      = 1005,    k_elif    = 1009,                      // If-else
	k_while    = 1002,    k_for       = 1006,    k_do      = 1010,                      // Loops
	k_continue = 1003,    k_break     = 1007,    k_return  = 1011,                      // Flow control
	k_switch   = 1004,    k_case      = 1008,    k_default = 1012,                      // Switch case

	k_highp    = 1013,    k_medp      = 1016,    k_lowp    = 1019,    k_const = 1020,   // Qualifiers
	k_local    = 1014,    k_extern    = 1017,                                           // Inputs
	k_struct   = 1015,    k_preicison = 1018,                                           // Other
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


	// Other
	e_start = 1000000,
	e_user_defined = 1000000,  // User defined identifiers
	e_literal      = 1000001,  // Literal constants
	e_newline      = 1000002,  // Line feed
	e_identifier   = 1000003,  // User defined identifiers
	e_preprocessor = 1000004,  // # characters
	e_unknown      = 1000005,  // Anything else
	e_end
};
int isType    (enum TokenID vID){ return vID >= t_start && vID < t_end; }
int isKeyword (enum TokenID vID){ return vID >= k_start && vID < k_end; }
int isOperator(enum TokenID vID){ return vID >= o_start && vID < o_end; }


struct Token{
	const char* value;     // The string value of the token  e.g. "const", "uint32"
	size_t len;            // Length of the token
	enum TokenID id;       // The ID of the token or its type  e.g. t_uint32, t_f64, k_while, e_whitespace
	void* data;            // A memory block that contains a TypeData_t or a LiteralData_t depending on the type of the token
	const char* leading_ws;
	// const char* line;      // The line that contains the token
	size_t lineNum;        // The number if the line
	size_t start;          // Index of the token's first character in its line
};


struct Line {
	uint32_t line;
	char* str;
	size_t len;
};








// Operators --------------------------------------------------------------------------------------------------------------------------------//








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
	enum OperatorType type;	// Type of the operator
	int32_t precedence;		// Operator precedence. Higher values have higher precedence
	char associativity; 	//'l'(left associative), 'r'(right associative) or 'n'(non associative)
};
struct OperatorData_t operatorData[] = { // Sorted by length
	{ ot_assignment, 16, 'r' }, { ot_assignment, 16, 'r' }, { ot_assignment, 16, 'r' }, { ot_assignment, 16, 'r' }, { ot_assignment, 16, 'r' }, { ot_assignment, 16, 'r' }, { ot_assignment, 16, 'r' }, { ot_assignment, 16, 'r' }, { ot_assignment, 16, 'r' }, { ot_assignment, 16, 'r' },
	{ ot_logical,     7, 'l' }, { ot_logical,     7, 'l' }, { ot_logical,     8, 'l' }, { ot_logical,     8, 'l' }, { ot_logical,    12, 'l' }, { ot_logical,    14, 'l' }, { ot_logical,    13, 'l' },
	{ ot_inc_dec,     2, 'l' }, { ot_inc_dec,     2, 'l' },
	{ ot_arithmetic,  6, 'l' }, { ot_arithmetic,  6, 'l' }, { ot_arithmetic,  5, 'l' }, { ot_arithmetic,  4, 'l' }, { ot_arithmetic,  5, 'l' }, { ot_arithmetic,  4, 'l' }, { ot_arithmetic,  4, 'l' }, { ot_arithmetic,  9, 'l' }, { ot_arithmetic, 11, 'l' }, { ot_arithmetic, 10, 'l' },
	{ ot_logical,     7, 'l' }, { ot_logical,     7, 'l' }, { ot_assignment, 16, 'r' }, { ot_logical,     3, 'r' }, { ot_arithmetic,  3, 'l' },
	{ ot_ternary,    15, 'r' }, { ot_ternary,    15, 'r' }, { ot_field,       2, 'l' }, { ot_list,       17, 'l' }, { ot_group,       1, 'n' }, { ot_group,       1, 'n' }, { ot_subscript,   2, 'l' }, { ot_subscript,   2, 'l' }, { ot_scope,       1, 'n' }, { ot_scope,       1, 'n' }
	//! Both ++ and -- an be prefix or postfix. The actual precedence and associativity is determined after the tokenization
	//! Same with +, -, ~. They can be unary or arithmetic
};
const char* operatorValues[] = {
	"<<=",">>=", "+=", "*=", "-=", "/=", "%=", "&=", "|=", "^=",
	"<=", ">=",  "==", "!=", "&&", "||", "^^",
	"++", "--",
	"<<", ">>",  "+",  "*",  "-",  "/",  "%",  "&",  "|",  "^",
	"<",  ">",   "=",  "!",  "~",
	"?",  ":",   ".",  ",",  "(",  ")",  "[",  "]",  "{",  "}"
	//! Operator values and data are sorted by value length to simplify the parsing
};








// Types ------------------------------------------------------------------------------------------------------------------------------------//








struct TypeData_t {
	char* glslType;        // The corresponding GLSL type
	enum TokenID baseType; // Base type of the type  e.g. the base type of a f32 matrix is f32
	size_t x;              // Width   e.g. a 2x3 matrix has x = 2, a scalar type has x = 1
	size_t y;              // Height  e.g. a 2x3 matrix has y = 3, a scalar type has x = 1
	size_t align;          // Alignment of the type in bytes
};
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
const char* typeValues[] = {
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
	"void"                                                              //Just void
};








// Keywords ---------------------------------------------------------------------------------------------------------------------------------//








const char* keywordValues[] = {
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








// Literals ---------------------------------------------------------------------------------------------------------------------------------//








struct LiteralData_t{
	enum TokenID type;          // Type of the value. This can only be the ID of a base type
	char value[sizeof(double)]; // Actual value. Contains a float, a double, an int or an unsigned int, depending on the type
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




/**
 * @brief Reads all the contents of the file vFilePath
 *     Removes any '\r' character
 * @param vFilePath The path of the file
 * @return A null terminated memory block containing the data
 */
char* readFile(const char* vFilePath){
	FILE* f = fopen(vFilePath, "r");
	fseek(f, 0, SEEK_END);
	int32_t size = ftell(f);
	rewind(f);

	char* data = malloc(size + 1);
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



/**
 * @brief Returns the address of the vIndex-th occurrence of vChar in the vSrc string
 * @param vSrc The source string
 * @param vChar The character to find
 * @param vIndex The index of the occurrence to find
 * @return The address of the vIndex-th occurrence of vChar
 */
const char* strchrn(const char* vSrc, const char vChar, const uint32_t vIndex){
	uint32_t n = 0;
	for(const char* c = vSrc;; ++c) {
		if(*c == '\0') return NULL;
		else if(*c == vChar) ++n;
		if(n == vIndex + 1) return c;
	}
}
//TODO add start_from parameter


/**
 * @brief Splits vSrc based on vChar and returns the vIndex-th string as a null terminated char*
 *     The original string is not modified
 *     The returned string must be freed
 * @param vSrc The source string
 * @param vChar The character used to split the string
 * @param vIndex The index of the resulting string to return
 * @return The vIndex-th resulting string, or NULL if it does not exist
 */
char* strtokn(const char* vSrc, const char vChar, const uint32_t vIndex){
	const char* a = vIndex ? strchrn(vSrc, vChar, vIndex - 1) : vSrc;
	const char* b = strchr(a + !!vIndex, vChar);
	size_t len =  b ? b - a : strlen(a);
	char* ret = malloc(len);
	memcpy(ret, a + 1, len - !!len);
	ret[len] = '\0';
	return ret;
}
//TODO add start from parameter




/**
 * @brief Prints an error using the printf format specifiers
 *     This function does not return
 * @param vFormat The format string
 * @param ... A list of arguments for the format string
 */
void printError(const char* vFormat, ...){
	va_list vArgs; va_start(vArgs, 0);
	char vStr[MAX_ERR];
	vsnprintf(vStr, MAX_ERR, vFormat, vArgs);

	printf("\n%sGenGlsl: Error:\n%s%s", bRed, vStr, nWht);
	exit(1);
}


/**
 * @brief Prints an error using the printf format specifiers, specifying the line and file in which the error occurred
 *     This function does not return
 * @param iLineN The number of the line. Used to print additional informations
 * @param iLine The line value. Used to print additional informations
 * @param iFile The name of the file. Used to print additional informations
 * @param vFormat The format string
 * @param ... A list of arguments for the format string
 */
void printSyntaxError(const int32_t iLineN, const char* iLine, const char* iFile, const char* vFormat, ...){
	va_list vArgs; va_start(vArgs, 0);
	char vStr[MAX_ERR];
	vsnprintf(vStr, MAX_ERR, vFormat, vArgs);

	printf(
		"%s\nGenGlsl: Syntax error on line %d, file \"%s\":" //FIXME
		"\n%s%s"
		"\n    %s"
		"\n\nCompilation stopped",
		bRed, iLineN + 1, realpath(iFile, NULL), vStr, nWht, iLine
	);
	exit(2);
}








// Preprocessor ----------------------------------------------------------------------------------------------------------------------------//








/**
 * @brief Removes the trailing whitespace of each line
 *     Consecutive newline characters are preserved
 * @param vLines An array of Line structs containing the lines to clear
 * @param vNum The number of lines
 */
void clear(struct Line* vLines, const size_t vNum){
	for(size_t i = 0; i < vNum; ++i){
		struct Line* l = &vLines[i];

		for(int64_t j = l->len - 1;; --j){
			char c = l->str[j];

			//Lines containing only whitespace or empty lines
			if(j < 0){
				l->str[0] = '\0';
				l->len = 0;
				break;
			}
			// Lines with trailing whitespace or none
			else if(c != '\t' && c != '\r' && c != ' ' && c != '\n') {
				l->str[j + 1] = '\0';
				l->len = j;
				break;
			}
		}

	}
}
//TODO replace tabs with spaces




/**
 * @brief Replaces multiline comments with a space + a number of '\n' corresponding to the number of newlines they contain
 *     Single line comments are replaced with one '\n' character
 * @param vCode The code to uncomment
 * @param iFile The path of the file containing the code. This is used to print syntax errors
 * @return The resulting string
 */
char* uncomment(const char* vCode, const char* iFile){
	char* code = malloc(MAX_CODE_LEN); code[0] = '\0';
	int32_t vCodeLen = strlen(vCode);
	int32_t i = 0;
	while(i < vCodeLen){								//For each character
		if(vCode[i] == '"'){								//If the character is a double quote
			int32_t strBegin = i;								//Save the string beginning for eventual errors
			strcat(code, "\"");									//Paste opening "
			++i;												//Skip opening "
			while(vCode[i] != '"'){								//For each character of the string
				strncat(code, &vCode[i], 1);						//Paste character
				++i;												//Update counter
				if(i == vCodeLen){									//If the string does not end
					int32_t vLineN = 0;										//
					for(int32_t j = 0; j < strBegin; ++j){				//Find the line in which the string begins
						if(vCode[j] == '\n') ++vLineN;						//
					}													// [...] Print a syntax error
					const char* errorLine = strchrn(vCode, '\n', vLineN);
					printSyntaxError(vLineN, errorLine, iFile, "Unterminated string");
				}
			}
			strcat(code, "\"");									//Paste closing '"'
			++i;												//Skip closing '"'
		}
		else if(i < vCodeLen - 1){							//If there is enough space to start a comment
			if(vCode[i] == '/' && vCode[i + 1] == '/'){			//If the character is the beginning of a single line comment
				i += 2;												//Ignore "//"
				while(i < vCodeLen && vCode[i] != '\n') ++i;		//Ignore the whole comment
				++i;												//Ignore '\n'
				strcat(code, "\n"); 								// Write '\n'
			}
			else if(vCode[i] == '/' && vCode[i + 1] == '*'){	//If the character is the beginning of a multiline comment
				strcat(code, " ");									//Add a space as token separator
				i += 2;												//Ignore "/*"
				while(i < vCodeLen && !(vCode[i] == '*' && vCode[i + 1] == '/')){ //For each character of the comment
					if(vCode[i] == '\n'){								//If the character is a newline
						strcat(code, "\n");									//Paste the newline
					}
					++i;												//Update the counter and ignore the other characters
				}
				i += 2;												//Ignore "*/"
			}
			else{												//Else
				strncat(code, &vCode[i], 1);						//Paste the character
				++i;												//Update the counter
			}
		}
		else{												//Else
			strncat(code, &vCode[i], 1);						//Paste the character
			++i;												//Update the counter
		}
	}

	return code; //Return the parsed code
}
//FIXME dont use strcat and strncat or just start from an index




/**
 * @brief Checks if an included path is valid
 *     Prints an error if it's not
 * @param iLineN The number of the line. Used to print errors
 * @param iLine The line value. Used to print errors
 * @param iFile The file path. Used to print errors
 * @param vPath The path of the included file
 */
void checkIncludeFile(const int32_t iLineN, const char* iLine, const char* iFile, const char* vPath){ //TODO check vLine type
	if(access(vPath, F_OK) == 0) {
		struct stat fileStat; stat(vPath, &fileStat);
		if(S_ISDIR(fileStat.st_mode)) {
			printSyntaxError(iLineN, iLine, iFile, "\"%s\" is a directory", vPath);
		}
	}
	else {
		printSyntaxError(iLineN, iLine, iFile, "No such file or directory");
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
		const char c = vLine[i];						// Check if either '<' or '"' are used
		if(c == '"' || c == '<'){						// If they are
			++i;											//
			for(size_t j = 0; i < strlen(vLine); ++i, ++j){	// Get the file path
				if(vLine[i] == (c == '<' ? '>' : '"')) {	//
					ret[j] = '\0';							//
					return ret;								// Return the path
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
 * @param pLineNum The address of a size_t variable where to store the total number of lines
 * @return An array of Line structures of size *pNum containing the lines of all the included files
 */
struct Line* include(const char* vCode, const char* vFile, const int32_t vLineInfo, size_t* pLineNum){
	char* code = uncomment(vCode, vFile);
	struct Line* ret = malloc(sizeof(struct Line) * MAX_CODE_LINES);
	char *line;
	size_t totLineNum = 0;
	for(int32_t i = 0; (line = strsep(&code, "\n")) != NULL; ++i){
		size_t lineNum = vLineInfo ? vLineInfo : i + 1;
		char* r = isInclude(line);
		if(r != NULL){								// If the line is an include statement
			checkIncludeFile(i, line, vFile, r);		// Check the included file
			char* included = readFile(r);
			size_t includedLen;
			struct Line* included2 = include(included, vFile, i + 1, &includedLen);
			memcpy(ret + totLineNum, included2, sizeof(struct Line) * includedLen);
			free(included);
			free(included2);
			totLineNum += includedLen;
			//TODO save original files
		}
		else{ // If not
			ret[totLineNum].line = lineNum;
			ret[totLineNum].len = strlen(line);//TODO rename local len
			ret[totLineNum].str = line;
			++totLineNum;
		}
	}
	*pLineNum = totLineNum;
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
			if(!strcmp(pToken->value, typeValues[t - t_start])){	// If it matches the current identifier
				pToken->id   = t;										// Set token id to the corresponding type id
				pToken->data = &typeData[t - t_start];					// Set token data to the hard coded data of the corresponding type
				return i;												// Return the length
			}
		}

		// Keywords
		for(int32_t k = k_start; k < k_end; ++k){				// For each hard coded keyword
			if(!strcmp(pToken->value, keywordValues[k - k_start])){	// If it matches the current identifier
				pToken->id   = k;										// Set token id to the corresponding identifier id
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
//TODO check empty literals like 0x, 0d, 0o
//TODO check decimal literals starting with .




size_t getOperator(const char* vLine, struct Token* const pToken){
	size_t opLen;
	for(int32_t o = o_start; o < o_end; ++o){				// For each hard coded operator
		opLen = strlen(operatorValues[o - o_start]);			// Cache operator length
		if(!strncmp(vLine, operatorValues[o - o_start], opLen)){// If it matches the current operator
			pToken->value = operatorValues[o - o_start];			// Save the operator
			pToken->len   = opLen;									// Save the length
			pToken->id    = o;										// Set token id to the corresponding operator id
			pToken->data  = &operatorData[o - o_start];				// Set token data to the hard coded data of the corresponding operator
			return opLen;											// Return the length
		}
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
			curToken->lineNum = i;
			curToken->start   = 0;

			++tok_j;
		}
	}
	*pNum = tok_j;
	return ret;
}








// Syntax ----------------------------------------------------------------------------------------------------------------------------------//








//








// Output assembler -------------------------------------------------------------------------------------------------------------------------//








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
		else if(isType(curTok->id)){
			strcpy(ret + j, ((struct TypeData_t*)(curTok->data))->glslType);
			j +=     strlen(((struct TypeData_t*)(curTok->data))->glslType);
		}
		else if(isKeyword(curTok->id)){
			strcpy(ret + j, keywordValues[curTok->id - k_start]);
			j +=     strlen(keywordValues[curTok->id - k_start]);
		}
		else if(isOperator(curTok->id)){
			strcpy(ret + j, operatorValues[curTok->id - o_start]);
			j +=     strlen(operatorValues[curTok->id - o_start]);
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