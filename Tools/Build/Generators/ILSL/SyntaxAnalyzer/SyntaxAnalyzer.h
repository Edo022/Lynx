#pragma once
#include "SyntaxAnalyzer/Constructs.h"



//TODO MOVE TO .c
uint64_t statTokGroup(const struct Token* const vTokens, const enum TokenID vLeft, const enum TokenID vRight, const struct Line* const iLines);
uint64_t statTok     (const struct Token* const vTokens, const enum TokenID vToken);



//TODO MOVE TO .c
struct If*    buildTreeIf   (const struct Token* const vTokens);
struct For*   buildTreeFor  (const struct Token* const vTokens);
struct While* buildTreeWhile(const struct Token* const vTokens);


uint64_t buildScopeSyntaxTree(struct Scope* const vParent, const struct Token* const vTokens, const struct Line* const iLines, struct Scope** const pScope);
