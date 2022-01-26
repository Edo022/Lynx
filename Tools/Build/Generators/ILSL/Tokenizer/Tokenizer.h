#pragma once
#include "ILSL/Tokenizer/Tokens.h"




uint64_t        findSpaces(const char* const vLine);
uint64_t   getPreprocessor(const char* const vLine, struct Token* const pToken);
uint64_t     getIdentifier(const char* const vLine, struct Token* const pToken);
uint64_t        getLiteral(const char* vLine, struct Token* const pToken, const struct Line iLineInfo);
uint64_t       getOperator(const char* const vLine, struct Token* const pToken);
uint64_t        getUnknown(const char* const vLine, struct Token* const pToken) ;
uint64_t getInstructionEnd(const char* const vLine, struct Token* const pToken) ;

struct Token* tokenize(struct Line* const vLines, const uint64_t vLineNum, uint64_t* pNum, const char* const iFileName);