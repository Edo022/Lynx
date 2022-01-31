#pragma once
#include "Files.h"





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
	k_if       = 1000,    k_else      = 1004,    k_elif    = 1008,						// If-else
	k_while    = 1001,    k_for       = 1005,    k_do      = 1009,						// Loops
	k_continue = 1002,    k_break     = 1006,    k_return  = 1010,						// Flow control
	k_switch   = 1003,    k_case      = 1007,    k_default = 1011,						// Switch case

	k_highp    = 1012,    k_medp      = 1015,    k_lowp    = 1018,    k_const = 1029,	// Qualifiers
	k_local    = 1013,    k_extern    = 1016,											// Inputs
	k_struct   = 1014,    k_preicison = 1017,											// Other
	k_end = 1019, //! Update this value when adding new keywords


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
	e_user_defined    = 1000000,	// User defined identifiers
	e_literal         = 1000001,	// Literal constants
	e_instruction_end = 1000003,	// Semicolon
	e_preprocessor    = 1000004,	// # characters
	e_unknown         = 1000005,	// Anything else
	//! Newlines are not saves as tokens
	e_end = 1000005	//! Update this value when adding new categories
};
static inline int isType    (enum TokenID vID){ return vID >= t_start && vID <= t_end; }
static inline int isKeyword (enum TokenID vID){ return vID >= k_start && vID <= k_end; }
static inline int isOperator(enum TokenID vID){ return vID >= o_start && vID <= o_end; }


struct Token{
	const char* value;		// The string value of the token  e.g. "const", "uint32"
	uint64_t len;			// Length of the token
	enum TokenID id;		// The ID of the token or its type  e.g. t_uint32, t_f64, k_while, e_whitespace
	void* data;				// A memory block that contains a TypeData_t or a LiteralData_t depending on the type of the token
	const char* leading_ws;	// The value of the leading whitespace
	uint64_t absLine;		// The absolute index of the line containing the token
	uint64_t locLine;		// The local index of the line containing the token
	uint64_t start;			// Index of the token's first character in its line
};
