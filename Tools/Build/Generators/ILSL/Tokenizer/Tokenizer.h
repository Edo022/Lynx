#pragma once
#include "Tokenizer/Tokens.h"




uint64_t pushStrPreprocessor   (const char* const vLine, struct Token* const pToken);
uint64_t pushStrIdentifier     (const char* const vLine, struct Token* const pToken);
uint64_t pushStrLiteral        (const char*       vLine, struct Token* const pToken, const struct Line iLineInfo);
uint64_t pushStrOperator       (const char* const vLine, struct Token* const pToken);
uint64_t pushStrUnknown        (const char* const vLine, struct Token* const pToken) ;
uint64_t pushStrInstructionEnd (const char* const vLine, struct Token* const pToken) ;

struct Token* tokenize(struct Line* const vLines, const char* const iFileName);