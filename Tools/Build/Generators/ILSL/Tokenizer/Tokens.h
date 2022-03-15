#pragma once
#include "Preprocessor/Files.h"

//TODO add bool literals



enum TokenID {
	// Built-in types
	t_start = 0,
	t_b     =  0,   t_u32     =  1,   t_i32     =  2,   t_f32     =  3,   t_f64     =  4,	// Scalar types
	t_bv2   =  5,   t_u32v2   =  6,   t_i32v2   =  7,   t_f32v2   =  8,   t_f64v2   =  9,	// 2-component vectors
	t_bv3   = 10,   t_u32v3   = 11,   t_i32v3   = 12,   t_f32v3   = 13,   t_f64v3   = 14,	// 3-component vectors
	t_bv4   = 15,   t_u32v4   = 16,   t_i32v4   = 17,   t_f32v4   = 18,   t_f64v4   = 19,	// 4-component vectors
	t_bm2   = 20,   t_u32m2   = 21,   t_i32m2   = 22,   t_f32m2   = 23,   t_f64m2   = 24,	// 2x2 square matrices
	t_bm3   = 25,   t_u32m3   = 26,   t_i32m3   = 27,   t_f32m3   = 28,   t_f64m3   = 29,	// 3x3 square matrices
	t_bm4   = 30,   t_u32m4   = 31,   t_i32m4   = 32,   t_f32m4   = 33,   t_f64m4   = 34,	// 4x4 square matrices
	t_auto  = 35, // Automatic type deduction
	t_void  = 36, // Just void
	t_end = 36, //! This value has to be manually updated


	// Flow control keywords
	k_start = 1000,
	c_if       = 1000,    c_else      = 1004,    c_then    = 1008,						// If, else, then
	c_while    = 1001,    c_for       = 1005,    c_do      = 1009,						// Loops
	c_continue = 1002,    c_break     = 1006,    c_return  = 1010,						// continue, break, return
	c_select   = 1003,    c_case      = 1007,    c_default = 1011,						// Select case
	// Other keywords
	k_highp    = 1012,    k_medp      = 1014,    k_lowp    = 1016,    k_const = 1017,	// Qualifiers
	k_struct   = 1013,    k_preicison = 1015,											// Other
	k_end = 1017, //! This value has to be manually updated


	//Operators
	o_start = 2000,
	o_set_bw_lshift = 2000,    o_log_less_eq = 2010,    o_inc = 2017,    o_bw_lshift = 2020,    o_log_less = 2030,    o_ter_0      = 2035,
	o_set_bw_rshift = 2001,    o_log_more_eq = 2011,    o_dec = 2018,    o_bw_rshift = 2021,    o_log_more = 2031,    o_ter_1      = 2036,
	o_set_sum       = 2002,    o_log_eq      = 2012,    o_seq = 2019,    o_sum       = 2022,    o_set      = 2032,    o_field      = 2037,
	o_set_mul       = 2003,    o_log_diff    = 2013,                     o_mul       = 2023,    o_log_not  = 2033,    o_lgroup     = 2038,
	o_set_sub       = 2004,    o_log_and     = 2014,                     o_sub       = 2024,    o_bw_not   = 2034,    o_rgroup     = 2039,
	o_set_div       = 2005,    o_log_or      = 2015,                     o_div       = 2025,                          o_lsubscript = 2040,
	o_set_mod       = 2006,    o_log_xor     = 2016,                     o_mod       = 2026,                          o_rsubscript = 2041,
	o_set_bw_and    = 2007,                                              o_bw_and    = 2027,                          o_lscope     = 2042,
	o_set_bw_or     = 2008,                                              o_bw_or     = 2028,                          o_rscope     = 2043,
	o_set_bw_xor    = 2009,                                              o_bw_xor    = 2029,
	o_end = 2043, //! This value has to be manually updated


	// Other
	e_start = 1000000,
	e_list            = 1000000,	// ,
	e_instruction_end = 1000001,	// ;
	e_preprocessor    = 1000002,	// #
	e_user_defined    = 1000003,	// User defined identifiers
	e_literal         = 1000004,	// Literal constants
	e_unknown         = 1000005,	// Anything else
	e_end = 1000005, //! This value has to be manually updated
	//! Newlines are not saves as tokens

	// Null terminator token
	e_NULL = (uint32_t)-1
};
static inline int tok_is_type    (enum TokenID vID){ return vID >= t_start && vID <= t_end; }
static inline int tok_is_keyword (enum TokenID vID){ return vID >= k_start && vID <= k_end; } //TODO remove. prob useless
static inline int tok_is_operator(enum TokenID vID){ return vID >= o_start && vID <= o_end; }


struct Token{
	const char* value;		// The string value of the token  e.g. "const", "uint32"
	uint64_t len;			// Length of the token
	enum TokenID id;		// The ID of the token or its type  e.g. t_uint32, t_f64, k_while, e_whitespace
	void* data;				// A memory block that contains a TypeData_t or a LiteralData_t depending on the type of the token
	uint64_t abs_line;		// The absolute index of the line containing the token
	uint64_t loc_line;		// The local index of the line containing the token
	uint64_t start;			// Index of the token's first character in its line
};
