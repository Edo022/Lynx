#pragma once
#include "Tokenizer/Tokens.h"




uint64_t push_str_preprocessor(const char* const line, struct Token* const tok);
uint64_t push_str_identifier  (const char* const line, struct Token* const tok);
uint64_t push_str_literal     (const char*       line, struct Token* const tok, const struct Line line_info);
uint64_t push_str_operator    (const char* const line, struct Token* const tok);
uint64_t push_str_unknown     (const char* const line, struct Token* const tok) ;
uint64_t push_str_inst_end    (const char* const line, struct Token* const tok) ;

uint64_t tokenize(struct Line* const lines, const char* const file_name, struct Token** out_tokens);