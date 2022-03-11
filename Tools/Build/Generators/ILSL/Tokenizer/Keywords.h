#pragma once
#include "Tokenizer/Tokens.h"



static const char* keyword_to_str(const enum TokenID id){
	static const char* values[] = {
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
	return values[id - k_start];
}