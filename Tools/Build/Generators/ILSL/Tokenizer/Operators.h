#pragma once
#include <inttypes.h>




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

static struct OperatorData_t operator_data[] = { // Sorted by length
	{ ot_assignment, 16, 'r' }, { ot_assignment, 16, 'r' }, { ot_assignment, 16, 'r' }, { ot_assignment, 16, 'r' }, { ot_assignment, 16, 'r' }, { ot_assignment, 16, 'r' }, { ot_assignment, 16, 'r' }, { ot_assignment, 16, 'r' }, { ot_assignment, 16, 'r' }, { ot_assignment, 16, 'r' },
	{ ot_logical,     7, 'l' }, { ot_logical,     7, 'l' }, { ot_logical,     8, 'l' }, { ot_logical,     8, 'l' }, { ot_logical,    12, 'l' }, { ot_logical,    14, 'l' }, { ot_logical,    13, 'l' },
	{ ot_inc_dec,     2, 'l' }, { ot_inc_dec,     2, 'l' },
	{ ot_arithmetic,  6, 'l' }, { ot_arithmetic,  6, 'l' }, { ot_arithmetic,  5, 'l' }, { ot_arithmetic,  4, 'l' }, { ot_arithmetic,  5, 'l' }, { ot_arithmetic,  4, 'l' }, { ot_arithmetic,  4, 'l' }, { ot_arithmetic,  9, 'l' }, { ot_arithmetic, 11, 'l' }, { ot_arithmetic, 10, 'l' },
	{ ot_logical,     7, 'l' }, { ot_logical,     7, 'l' }, { ot_assignment, 16, 'r' }, { ot_logical,     3, 'r' }, { ot_arithmetic,  3, 'l' },
	{ ot_ternary,    15, 'r' }, { ot_ternary,    15, 'r' }, { ot_field,       2, 'l' }, { ot_list,       17, 'l' }, { ot_group,       1, 'n' }, { ot_group,       1, 'n' }, { ot_subscript,   2, 'l' }, { ot_subscript,   2, 'l' }, { ot_scope,       1, 'n' }, { ot_scope,       1, 'n' }
	//! Both ++ and -- an be prefix or postfix. The actual precedence and associativity is determined after the tokenization
	//! Same with +, -, ~. They can be unary or arithmetic
};

static const char* operator_values[] = {
	"<<=",">>=", "+=", "*=", "-=", "/=", "%=", "&=", "|=", "^=",
	"<=", ">=",  "==", "!=", "&&", "||", "^^",
	"++", "--",
	"<<", ">>",  "+",  "*",  "-",  "/",  "%",  "&",  "|",  "^",
	"<",  ">",   "=",  "!",  "~",
	"?",  ":",   ".",  ",",  "(",  ")",  "[",  "]",  "{",  "}"
	//! Operator values and data are sorted by value length to simplify the parsing
};



