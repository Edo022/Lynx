#pragma once
#include "SyntaxAnalyzer/Constructs.h"




uint64_t statTokGroup(const struct Token* const vTokens, const uint64_t vTokenNum, const enum TokenID vLeft, const enum TokenID vRight, const struct Line* const iLines);
uint64_t statTok     (const struct Token* const vTokens, const uint64_t vTokenNum, const enum TokenID vToken);




struct OpNode* buildTreeIf   (const struct Token* const vTokens, const uint64_t vTokenNum);
struct OpNode* buildTreeFor  (const struct Token* const vTokens, const uint64_t vTokenNum);
struct OpNode* buildTreeWhile(const struct Token* const vTokens, const uint64_t vTokenNum);


struct Scope* buildScopeSyntaxTree(struct Scope* const vParent, const struct Token* const vTokens, const uint64_t vTokenNum, const struct Line* const iLines);
struct Scope* buildSyntaxTree                                  (const struct Token* const vTokens, const uint64_t vTokenNum, const struct Line* const iLines); //TODO REMOVE
