#pragma once
#include "SyntaxAnalyzer/Constructs.h"



uint64_t stat_tok_group(const struct Token* const tokens, const enum TokenID left_del, const enum TokenID right_del, const struct Line* const line_info);
uint64_t stat_tok      (const struct Token* const tokens, const enum TokenID tok);



struct If*    build_tree_if   (const struct Token* const tokens);
struct For*   build_tree_for  (const struct Token* const tokens);
struct While* build_tree_while(const struct Token* const tokens);


uint64_t build_scope_syntax_tree(struct Scope* const parent, const struct Token* const tokens, const struct Line* const line_info, struct Scope** const out_scope);
