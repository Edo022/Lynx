//exec          gcc GenGlsl.c -std=c11 -o GenGlsl
//exec          gcc GenGlsl.c -std=c11 -ggdb3 -g3 -O0 -o GenGlsl
//run           Tools/Build/Generators/GenGlsl src/Lynx/shaders/Volume.ilsl ../.engine/.tmp/glsl-Volume.ilsl.comp


#define _DEFAULT_SOURCE // Required for realpath and strsep
#include <stdio.h>
#include <ctype.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <linux/limits.h>

#define MAX_ERR         4100100
#define MAX_CODE_LEN    800100100	//FIXME use dynamic reallocations
#define MAX_CODE_LINES  4100100		//FIXME use dynamic reallocations or pass the size
#define MAX_TOKENS      8100100		//FIXME use dynamic reallocations

//FIXME GLSL doesnt suppotr bool and integer matrices
//TODO check returns values
//TODO ADD MACROS
//TODO ADD INCLUDE LISTS








// Files ------------------------------------------------------------------------------------------------------------------------------------//








struct File {			// A struct containing informations about a file
	char* path;				// The path to the file
	struct File* from;		// The file from which this file was included, or 0 for source files
	uint64_t fromLine;		// The line from which this file was included, unset for source files
};
struct File* files;		// A list of all the compiled files. Reallocated dynamically
uint64_t filesNum = 0;		// The number of compiled files




struct Line {			// A struct containing informations about a line
	char* value;			// String value of the line
	uint64_t len;			// Length of the line, '\n' or '\0' excluded
	struct File* file;		// A pointer to the file containing this line
	uint32_t lineNum;		// The number of the line. Starts from 0
};









// Tokens -----------------------------------------------------------------------------------------------------------------------------------//








enum TokenID {
	//Types
	t_start = 0,
	t_b     =  0,   t_u32     =  1,   t_i32     =  2,   t_f32     =  3,   t_f64     =  4,	// Scalar types
	t_bv2   =  5,   t_u32v2   =  6,   t_i32v2   =  7,   t_f32v2   =  8,   t_f64v2   =  9,	// 2-component vectors
	t_bv3   = 10,   t_u32v3   = 11,   t_i32v3   = 12,   t_f32v3   = 13,   t_f64v3   = 14,	// 3-component vectors
	t_bv4   = 15,   t_u32v4   = 16,   t_i32v4   = 17,   t_f32v4   = 18,   t_f64v4   = 19,	// 4-component vectors
	t_bm2   = 20,   t_u32m2   = 21,   t_i32m2   = 22,   t_f32m2   = 23,   t_f64m2   = 24,	// 2x2 square matrices
	t_bm3   = 25,   t_u32m3   = 26,   t_i32m3   = 27,   t_f32m3   = 28,   t_f64m3   = 29,	// 3x3 square matrices
	t_bm4   = 30,   t_u32m4   = 31,   t_i32m4   = 32,   t_f32m4   = 33,   t_f64m4   = 34,	// 4x4 square matrices
	t_bm2x2 = 35,   t_u32m2x2 = 36,   t_i32m2x2 = 37,   t_f32m2x2 = 38,   t_f64m2x2 = 39,	// 2x2 matrices
	t_bm2x3 = 40,   t_u32m2x3 = 41,   t_i32m2x3 = 42,   t_f32m2x3 = 43,   t_f64m2x3 = 44,	// 2x3 matrices
	t_bm2x4 = 45,   t_u32m2x4 = 46,   t_i32m2x4 = 47,   t_f32m2x4 = 48,   t_f64m2x4 = 49,	// 2x4 matrices
	t_bm3x2 = 50,   t_u32m3x2 = 51,   t_i32m3x2 = 52,   t_f32m3x2 = 53,   t_f64m3x2 = 54,	// 3x2 matrices
	t_bm3x3 = 55,   t_u32m3x3 = 56,   t_i32m3x3 = 57,   t_f32m3x3 = 58,   t_f64m3x3 = 59,	// 3x3 matrices
	t_bm3x4 = 60,   t_u32m3x4 = 61,   t_i32m3x4 = 62,   t_f32m3x4 = 63,   t_f64m3x4 = 64,	// 3x4 matrices
	t_bm4x2 = 65,   t_u32m4x2 = 66,   t_i32m4x2 = 67,   t_f32m4x2 = 68,   t_f64m4x2 = 69,	// 4x2 matrices
	t_bm4x3 = 70,   t_u32m4x3 = 71,   t_i32m4x3 = 72,   t_f32m4x3 = 73,   t_f64m4x3 = 74,	// 4x3 matrices
	t_bm4x4 = 75,   t_u32m4x4 = 76,   t_i32m4x4 = 77,   t_f32m4x4 = 78,   t_f64m4x4 = 79,	// 4x4 matrices
	t_void  = 80, // Just void
	t_end = 80, //! Update this value when adding new types


	// Keywords
	k_start = 1000,
	k_if       = 1001,    k_else      = 1005,    k_elif    = 1009,						// If-else
	k_while    = 1002,    k_for       = 1006,    k_do      = 1010,						// Loops
	k_continue = 1003,    k_break     = 1007,    k_return  = 1011,						// Flow control
	k_switch   = 1004,    k_case      = 1008,    k_default = 1012,						// Switch case

	k_highp    = 1013,    k_medp      = 1016,    k_lowp    = 1019,    k_const = 1020,	// Qualifiers
	k_local    = 1014,    k_extern    = 1017,											// Inputs
	k_struct   = 1015,    k_preicison = 1018,											// Other
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
	e_user_defined = 1000000,	// User defined identifiers
	e_literal      = 1000001,	// Literal constants
	e_newline      = 1000002,	// Line feed
	e_identifier   = 1000003,	// User defined identifiers
	e_preprocessor = 1000004,	// # characters
	e_unknown      = 1000005,	// Anything else
	e_end
};
int isType    (enum TokenID vID){ return vID >= t_start && vID < t_end; }
int isKeyword (enum TokenID vID){ return vID >= k_start && vID < k_end; }
int isOperator(enum TokenID vID){ return vID >= o_start && vID < o_end; }


struct Token{
	const char* value;		// The string value of the token  e.g. "const", "uint32"
	uint64_t len;			// Length of the token
	enum TokenID id;		// The ID of the token or its type  e.g. t_uint32, t_f64, k_while, e_whitespace
	void* data;				// A memory block that contains a TypeData_t or a LiteralData_t depending on the type of the token
	const char* leading_ws;	// The value of the leading whitespace
	uint64_t lineNum;		// The number if the line
	uint64_t start;			// Index of the token's first character in its line
};








// idk how this is called qwq ---------------------------------------------------------------------------------------------------------------//








struct Var;
struct Function;
struct Struct;

struct Scope{
    struct Scope* parent;		// The parent scope of the scope
    struct Function* functionv;	// An array  of functions declared in the scope
    uint64_t functionc;			// The numer of functions declared in the scope
    struct Var* varv;			// An array  of variables declared in the scope
    uint64_t varc;				// The numer of variables declared in the scope
    struct Struct* structv;		// An array  of structures declared in the scope
    uint64_t strctc;			// The numer of structures declared in the scope
};

struct Var{
    struct Scope* parent;		// The parent scope of the variable
    enum TokenID type;			// THe type of the variable
    bool is_const;				// True if the function has const type, false otherwise
    char* name;					// The name of the function
};

struct Struct{
    struct Scope* parent;		// The parent scope of the struct
    char* name;					// The name of the struct
    struct Var* memberv;		// An array of members
    uint64_t memberc;			// The number of members
};


struct Function{
    struct Scope* parent;		// The parent scope of the function
    enum TokenID type;			// The type of the function
    char* name;					// The name of the function
    // struct Var* paramv;			// An array of parameters //TODO same the variables in the function's scope
    // uint64_t paramc;			// The number of parameters
    struct Scope scope;			// The scope of the function
    struct Token* exec; 		//runtime lines as a list of tokens
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
static struct OperatorData_t operatorData[] = { // Sorted by length
	{ ot_assignment, 16, 'r' }, { ot_assignment, 16, 'r' }, { ot_assignment, 16, 'r' }, { ot_assignment, 16, 'r' }, { ot_assignment, 16, 'r' }, { ot_assignment, 16, 'r' }, { ot_assignment, 16, 'r' }, { ot_assignment, 16, 'r' }, { ot_assignment, 16, 'r' }, { ot_assignment, 16, 'r' },
	{ ot_logical,     7, 'l' }, { ot_logical,     7, 'l' }, { ot_logical,     8, 'l' }, { ot_logical,     8, 'l' }, { ot_logical,    12, 'l' }, { ot_logical,    14, 'l' }, { ot_logical,    13, 'l' },
	{ ot_inc_dec,     2, 'l' }, { ot_inc_dec,     2, 'l' },
	{ ot_arithmetic,  6, 'l' }, { ot_arithmetic,  6, 'l' }, { ot_arithmetic,  5, 'l' }, { ot_arithmetic,  4, 'l' }, { ot_arithmetic,  5, 'l' }, { ot_arithmetic,  4, 'l' }, { ot_arithmetic,  4, 'l' }, { ot_arithmetic,  9, 'l' }, { ot_arithmetic, 11, 'l' }, { ot_arithmetic, 10, 'l' },
	{ ot_logical,     7, 'l' }, { ot_logical,     7, 'l' }, { ot_assignment, 16, 'r' }, { ot_logical,     3, 'r' }, { ot_arithmetic,  3, 'l' },
	{ ot_ternary,    15, 'r' }, { ot_ternary,    15, 'r' }, { ot_field,       2, 'l' }, { ot_list,       17, 'l' }, { ot_group,       1, 'n' }, { ot_group,       1, 'n' }, { ot_subscript,   2, 'l' }, { ot_subscript,   2, 'l' }, { ot_scope,       1, 'n' }, { ot_scope,       1, 'n' }
	//! Both ++ and -- an be prefix or postfix. The actual precedence and associativity is determined after the tokenization
	//! Same with +, -, ~. They can be unary or arithmetic
};
static const char* operatorValues[] = {
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
	uint64_t x;            // Width   e.g. a 2x3 matrix has x = 2, a scalar type has x = 1
	uint64_t y;            // Height  e.g. a 2x3 matrix has y = 3, a scalar type has x = 1
	uint64_t align;        // Alignment of the type in bytes
};
static struct TypeData_t typeData[] = {
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
static const char* typeValues[] = {
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








const char *nBlk = "\033[0;30m", *bBlk = "\033[1;30m", *uBlk = "\033[4;30m";
const char *nRed = "\033[0;31m", *bRed = "\033[1;31m", *uRed = "\033[4;31m";
const char *nGrn = "\033[0;32m", *bGrn = "\033[1;32m", *uGrn = "\033[4;32m";
const char *nYel = "\033[0;33m", *bYel = "\033[1;33m", *uYel = "\033[4;33m";
const char *nBlu = "\033[0;34m", *bBlu = "\033[1;34m", *uBlu = "\033[4;34m";
const char *nMag = "\033[0;35m", *bMag = "\033[1;35m", *uMag = "\033[4;35m";
const char *nCyn = "\033[0;36m", *bCyn = "\033[1;36m", *uCyn = "\033[4;36m";
const char *nWht = "\033[0;37m", *bWht = "\033[1;37m", *uWht = "\033[4;37m";



// Bit scan reverse //TODO add to Lynx Engine
size_t bsr(size_t value){
	size_t ret;
	__asm__("bsr %1, %0" : "=r"(ret) : "r"(value));
	return ret;
}

// Bit scanf forward //TODO add to Lynx Engine
size_t bsf(size_t value){
	size_t ret;
	__asm__("bsf %1, %0" : "=r"(ret) : "r"(value));
	return ret;
}




/**
 * @brief Reallocates pAlloc to (vSize) * (the closest power of 2 that can contain (vOldNum + 1) elements)
 *     //TODO
 * @param pAlloc The memory block to reallocate
 *     If the number of elements allocated in pAlloc is smaller than vOldNum, it must be a power of 2
 * @param vSize The size of each element
 * @param vOldNum The number of elements that are currently allocated //TODO
 * @return The address of the new memory block. This is the same as pAlloc if it has enough free space
 */
void* realloc2(void* pAlloc, uint64_t vSize, uint64_t vOldNum){
	uint64_t step = 0b10 << bsr(vOldNum);
	if(vOldNum + 1 >= step) pAlloc = realloc(pAlloc, vSize * step);
	return pAlloc;
}






/**
 * @brief //TODOvv
 * @param vStr //TODO
 * @param vBase min 2, max 36 //TODO
 * @return //TODO
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
 *     Removes any '\r' and '\v' character
 *     Tabs are replaced with spaces
 * @param vFilePath The path of the file
 * @param vTabSize The length of tab characters. Max 8
 * @return A null terminated memory block containing the data
 */
char* readFile(const char* vFilePath, uint64_t vTabSize){
	if(vTabSize > 8) vTabSize = 8;
	FILE* const f = fopen(vFilePath, "r");
	fseek(f, 0, SEEK_END);
	const uint64_t size = ftell(f);
	rewind(f);

	char* const data = malloc(vTabSize * size + 1);
	uint64_t i, j, line_i, newlines = 0;
	for(i = j = line_i = 0; i < size; ++i) {
		const char c = fgetc(f);
		switch(c){
			case '\r': break;
			case '\v': break;
			case '\t': {
				const uint64_t n = vTabSize - line_i % vTabSize;
				strncpy(data + j, "        ", n);
				j += n;
				line_i += n;
				break;
			}
			case '\n': {
				if(j && data[j - 1] == '\\') {
					--j;
					++newlines;
				}
				else {
					data[j++] = c;
					for(; newlines; --newlines) data[j++] = '\n';
					line_i = 0;
				}
				break;
			}
			default: {
				data[j++] = c;
				++line_i;
				break;
			}
		}
	}
	data[j] = '\0';

	fclose(f);
	return data;
}
//TODO add newline continuation



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
	const char* const a = vIndex ? strchrn(vSrc, vChar, vIndex - 1) : vSrc;
	const char* const b = strchr(a + !!vIndex, vChar);
	const uint64_t len =  b ? b - a : strlen(a);
	char* const ret = malloc(len);
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
void printSyntaxError(const struct Line iLineInfo, const char* const vFormat, ...){
	va_list vArgs; va_start(vArgs, vFormat);
	char vStr[MAX_ERR];
	vsnprintf(vStr, MAX_ERR, vFormat, vArgs);


	printf("%s\nGenGlsl: Syntax error on line %s:%d", bRed, realpath(iLineInfo.file->path, NULL), iLineInfo.lineNum + 1);
	for(struct File* f = iLineInfo.file; f->from; f = f->from){
		printf("\n                Included from %s:%d", realpath(f->from->path, NULL), f->fromLine + 1);
	}
	printf("\n%s%s\n    %s\n\nCompilation stopped", vStr, nWht, iLineInfo.value);

	va_end(vArgs);
	exit(2);
}








// Preprocessor ----------------------------------------------------------------------------------------------------------------------------//








/**
 * @brief Removes the trailing whitespace of each line
 *     Consecutive newline characters are preserved
 * @param vLines An array of Line structs containing the lines to clear
 * @param vNum The number of lines
 */
void clear(struct Line* vLines, const uint64_t vNum){
	for(uint64_t i = 0; i < vNum; ++i){
		struct Line* const l = &vLines[i];

		for(int64_t j = l->len - 1;; --j){
			//Lines containing only whitespace or empty lines
			if(j < 0){
				l->value[0] = '\0';
				l->len = 0;
				break;
			}
			// Lines with trailing whitespace or none
			else if(l->value[j] != ' ') {
			//!^ '\r' are removed, tabs are replaced with spaces and newlines are not part of the line value
				l->value[j + 1] = '\0';
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
 */ //FIXME WRITE THE RESULT IN THE SAME ALLOCATION AND DONT RETURN IT
char* uncomment(const char* vCode, struct File* const iFile){
	char* code = malloc(MAX_CODE_LEN); code[0] = '\0';
	const uint64_t vCodeLen = strlen(vCode);
	uint64_t i = 0;


	//For each character
	while(i < vCodeLen){

		//If the character is a double quote
		if(vCode[i] == '"'){
			//Check if the string ends
			const char* const str_begin = vCode + i;
			int ends = 1;
			for(++i; vCode[i] != '"'; ++i){
				if(!vCode[i] || vCode[i] == '\n') {
					ends = 0; break;
				}
			}

			// If it does, paste it and skip closing '"'
			if(ends) {
				strncat(code, str_begin, i - (str_begin - vCode) + 1);
				++i;
			}
			// If it doesn't, print a syntax error
			else{
				uint64_t vLineN = 0; for(const char* c = vCode; c < str_begin; ++c) vLineN += *c == '\n';
				const char* errorLine = vLineN ? strchrn(vCode, '\n', vLineN - 1) : vCode;
				const char* iLineValue = strndup(errorLine, strchrn(vCode, '\n', vLineN) - errorLine);

				struct Line iLineInfo;
				iLineInfo.len     = strlen(iLineValue);
				iLineInfo.value   = strdup(iLineValue);
				iLineInfo.file    = iFile;
				iLineInfo.lineNum = vLineN;
				printSyntaxError(iLineInfo, "Unterminated string");
			}
		}

		// If it's not and there is enough space to start a comment
		else if(i < vCodeLen - 1){
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

		// Else paste whatever character was read
		else{
			strncat(code, &vCode[i], 1);
			++i;
		}
	}


	//Return the parsed code
	return code;
}
//FIXME dont use strcat and strncat or just start from an index




/**
 * @brief Checks if an included path is valid
 *     Prints an error if it's not
 * @param iLineValue The line value. Used to print errors
 * @param iLineNum The number of the line. Used to print errors
 * @param iFile The file path. Used to print errors
 * @param vPath The path of the included file
 */
void checkIncludeFile(const struct Line iLineInfo, const char* vPath){ //TODO check vLine type
	if(access(vPath, F_OK) == 0) {
		struct stat fileStat; stat(vPath, &fileStat);
		if(S_ISDIR(fileStat.st_mode)) {
			printSyntaxError(iLineInfo, "\"%s\" is a directory", vPath);
		}
	}
	else {
		printSyntaxError(iLineInfo, "No such file or directory");
	}
}




uint64_t findSpaces(const char* const vLine);
/**
 * @brief Checks if vLine is an include statement and returns the path of the included file
 * @param vLineValue The line to parse
 * @return The path of the included file, or NULL if the line is not an include statement
 */
char* isInclude(const struct Line iLineInfo){
	// Check #
	uint64_t i = findSpaces(iLineInfo.value);
	if(iLineInfo.value[i] != '#') return NULL;
	i += findSpaces(iLineInfo.value + ++i);

	// Check "include "
	const char* const include_value = "include ";
	const uint64_t include_len = strlen(include_value);
	if(memcmp(iLineInfo.value + i, include_value, include_len)) return NULL;
	i += include_len;

	// Get included file path
	char* const ret = malloc(PATH_MAX);				// Allocate space for the included file path
	const char c = iLineInfo.value[i] == '<' ? '>' : iLineInfo.value[i] == '"' ? '"' : '\0';
	if(c){												// If either '<' or '"' are used
		++i;												// Ignore '<' or '"'
		const uint64_t line_len = strlen(iLineInfo.value);		// Cache the length of the line
		for(uint64_t j = 0; i < line_len; ++i, ++j){		// Get the file path
			if(iLineInfo.value[i] == c) {						//
				ret[j] = '\0';								//
				return ret;									// Return the path
			}
			ret[j] = iLineInfo.value[i];
		}
	}
	printSyntaxError(iLineInfo, "Invalid include statement");
}




/**
 * @brief Creates a code with no includes by recursively pasting all the included files together
 *     Comments are not preserved
 * @param vCode The code containing the include statements
 * @param vFile The path of the file
 * @param vLineInfo The absolute line from which the file was included. UINT64_MAX if the file is not included
 * @param pLineNum The address of a uint64_t variable where to store the total number of lines
 * @return An array of Line structures of size *pNum containing the lines of all the included files
 */
struct Line* include(const char* const vFile, const uint64_t vFromLine, struct File* vFromFile, uint64_t* const pLineNum){
	//Reallocate file array
	files = realloc2(files, sizeof(struct File), filesNum);

	files[filesNum].path = strdup(vFile);
	files[filesNum].fromLine = vFromLine;
	files[filesNum].from = vFromFile;
	++filesNum;



	char* code = readFile(vFile, 4);									// Read the file
	code = uncomment(code, &files[filesNum - 1]);											// Uncomment it
	struct Line* const ret = malloc(sizeof(struct Line) * MAX_CODE_LINES);	// Allocate the return array



	char *line; uint64_t totLineNum = 0;
	for(uint64_t i = 0; (line = strsep(&code, "\n")) != NULL; ++i){			// For each line of the code
		// const uint64_t lineNum = vFromLine == UINT64_MAX ? i : vFromLine;		// Get the number of the line from which the file was included

		struct Line tmp_isinclude_info; //TODO
		tmp_isinclude_info.file = &files[filesNum - 1];
		tmp_isinclude_info.value = line;
		tmp_isinclude_info.len = strlen(line);
		tmp_isinclude_info.lineNum = i;
		char* const r = isInclude(tmp_isinclude_info);								// Check the line
		if(r != NULL){															// If the line is an include statement
			checkIncludeFile(tmp_isinclude_info, r);									// Check the included file
			uint64_t includedLen;													//
			struct Line* included = include(r, i, &files[filesNum - 1], &includedLen);// Get the lines of the included file
			memcpy(ret + totLineNum, included, sizeof(struct Line) * includedLen);	// Copy them in the return array
			free(included);															// Free the saved lines
			totLineNum += includedLen;												// Update the line counter
		}
		else{																	// If it's not
			// ret[totLineNum].lineNum = lineNum;										// Set the line numer
			ret[totLineNum].lineNum = i;										// Set the line numer
			ret[totLineNum].len     = strlen(line);									// Set the line length
			ret[totLineNum].value   = line;											// Set the line value
			ret[totLineNum].file    = &files[filesNum - 1];							// Set the line file
			++totLineNum;															// Update the line counter
		}
	}

	*pLineNum = totLineNum;
	return ret;
}







// Tokenizer -------------------------------------------------------------------------------------------------------------------------------//








uint64_t findSpaces(const char* const vLine){
	for(uint64_t i = 0;; ++i) {
		if(vLine[i] != ' ') return i;
	}
}




uint64_t getPreprocessor(const char* const vLine, struct Token* const pToken){
	if(*vLine == '#'){
		pToken->value = strdup("#");
		pToken->len   = 1;
		pToken->id    = e_preprocessor;
		pToken->data  = NULL;
		return 1;
	}
	return 0;
}




uint64_t getIdentifier(const char* const vLine, struct Token* const pToken){
	if(isalpha(vLine[0]) || vLine[0] == '_'){
		uint64_t i;
		for(i = 1; isalnum(vLine[i]) || vLine[i] == '_'; ++i);	// Get the length
		pToken->value = strndup(vLine, i);						// Save the identifier
		pToken->len   = i;										// Save the length

		// Types
		for(uint64_t t = t_start; t < t_end; ++t){				// For each hard coded type
			if(!strcmp(pToken->value, typeValues[t - t_start])){	// If it matches the current identifier
				pToken->id   = t;										// Set token id to the corresponding type id
				pToken->data = &typeData[t - t_start];					// Set token data to the hard coded data of the corresponding type
				return i;												// Return the length
			}
		}

		// Keywords
		for(uint64_t k = k_start; k < k_end; ++k){				// For each hard coded keyword
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
uint64_t getLiteral(const char* vLine, struct Token* const pToken, const struct Line iLineInfo ){
	if(isdigit(vLine[0])){
		pToken->data = malloc(sizeof(struct LiteralData_t));			// Allocate a block for the data

		uint64_t isValid = 1, i, base;									// 1 if the literal is valid, loop index and literal length, literal base
		uint64_t offset = (vLine[0] == '0' && isalpha(vLine[1])) ? 2 : 0;	// 0 or 2 if the literal has a prefix
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
		if(!isValid) printSyntaxError(iLineInfo, "Invalid %s literal \"%s\"", baseName, pToken->value);

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




uint64_t getOperator(const char* const vLine, struct Token* const pToken){
	for(uint64_t opLen, o = o_start; o < o_end; ++o){			// For each hard coded operator
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




uint64_t getUnknown(const char* const vLine, struct Token* const pToken) {
	pToken->value = strndup(vLine, 1);
	pToken->len   = 1;
	pToken->id    = e_unknown;
	pToken->data  = NULL;
	return 1;
}








//TODO replace tabs with spaces
struct Token* tokenize(struct Line* const vLines, const uint64_t vLineNum, uint64_t* pNum, const char* const iFileName){
	struct Token* const ret = malloc(sizeof(struct Token) * MAX_TOKENS);
	uint64_t tok_j = 0;
	for(uint64_t i = 0; i < vLineNum; ++i){
		char* const l = vLines[i].value;
		const uint64_t lLen = strlen(l);
		char* leading_ws = NULL;
		for(uint64_t j = 0; j < lLen; ++tok_j){
			struct Token* const curToken = ret + tok_j;	// Cache the address of the current token
			curToken->lineNum = i;						// Set the number of the line
			curToken->start   = j;						// Set the start index to j
			curToken->leading_ws = leading_ws ? strdup(leading_ws) : NULL; // Save leading whitespace


			// Get leading whitespace
			const uint64_t wsLen = findSpaces(l + j);
			if(wsLen){
				leading_ws = strndup(l + j, wsLen);
				j += wsLen; --tok_j; continue;
			}
			else leading_ws = NULL;

			// Find the first token, save into the array and update j
			uint64_t tokLen;
			if     (tokLen = getPreprocessor (l + j, curToken)){}
			else if(tokLen = getIdentifier   (l + j, curToken)){}
			else if(tokLen = getLiteral      (l + j, curToken, vLines[i])){}
			else if(tokLen = getOperator     (l + j, curToken)){}
			else    tokLen = getUnknown      (l + j, curToken);
			j += tokLen;
		}

		//Add newline token
		if(i < vLineNum - 1) {
			struct Token* const curToken = ret + tok_j;
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








void checkSyntax(const struct Token* const vTokens, const uint64_t vTokenNum, const struct Line* const iLines, const char* const iFileName){
	struct Scope g;			// Global scope
	size_t lineNum = 0;		// Line number

	for(size_t i = 0; i < vTokenNum;){
		const struct Token* t = &vTokens[i];
		if(isType(t->id)){ ++i; }
		else if(t->id == e_newline){
			++lineNum; ++i;
		}
		else if(t->id == e_preprocessor) {
			//FIXME actually check the syntax and save the arguments
			while(vTokens[i].id != e_newline) ++i;
		}
		else printSyntaxError(iLines[lineNum], "Unexpected token \"%s\"", t->value);
	}
}








// Output assembler -------------------------------------------------------------------------------------------------------------------------//








char* translate(const struct Token* vTokens, const uint64_t vTokensNum){
	char* const ret = malloc(MAX_CODE_LEN);
	uint64_t j = 0;
	for(uint64_t i = 0; i < vTokensNum; ++i){
		const struct Token* const curTok = vTokens + i;
		if(curTok->leading_ws){
			strcpy(ret + j, curTok->leading_ws);
			j += strlen(curTok->leading_ws);
		}

		if(curTok->id == e_literal){
			struct LiteralData_t* const tokData = curTok->data;
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








void run(const char* const vSrc, const char* const vOut){
	files = malloc(sizeof(struct File) * 2);

	//Read input file
	const char* const src = realpath(vSrc, NULL); //Resolve symbolic links
	if(access(src, F_OK) != 0) printError("\"%s\": No such file or directory", vSrc);

	//Add hard coded version statement and parse the code
	uint64_t outputLinesNum;
	struct Line* const outputLines = include(vSrc, UINT64_MAX, NULL, &outputLinesNum);
	clear(outputLines, outputLinesNum);

	// Tokenize the code
	uint64_t outputTokensNum;
	struct Token* const outputTokens = tokenize(outputLines, outputLinesNum, &outputTokensNum, vSrc);

	// Check the syntax and write the GLSL code
	checkSyntax(outputTokens, outputTokensNum, outputLines, vSrc);
	char* const outputStr = translate(outputTokens, outputTokensNum);

	//Write output file
	FILE* ofile = fopen(vOut, "w");
	fprintf(ofile, "#version 450\n");
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