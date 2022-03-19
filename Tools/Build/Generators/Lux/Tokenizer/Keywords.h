#pragma once
#include "Tokenizer/Tokens.h"



static const char* keyword_to_str(const enum TokenID id){
	static const char* values[] = {
		//If, then     Loops           Iterations, return Select case
		"if",          "while",        "continue",        "select",
		"else",        "for",          "break",           "case",
	    "then",        "do",           "return",          "default",

		//Qualifiers   Other
		"highp",       "struct",
		"medp",        "preicison",
		"lowp",
		"const"
	};
	return values[id - k_start];
}