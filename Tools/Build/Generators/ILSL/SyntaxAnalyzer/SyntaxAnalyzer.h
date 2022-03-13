#pragma once
#include "SyntaxAnalyzer/Constructs.h"



uint64_t stat_tok_group(const struct Token* const tokens, const enum TokenID left_del, const enum TokenID right_del, const struct Line* const line_info);
uint64_t stat_tok      (const struct Token* const tokens, const enum TokenID tok);



uint64_t build_statement_if   (struct Scope* const parent, const struct Token* const tokens, struct If*    const dst, const struct Line* const line_info);
uint64_t build_statement_for  (struct Scope* const parent, const struct Token* const tokens, struct For*   const dst, const struct Line* const line_info);
uint64_t build_statement_while(struct Scope* const parent, const struct Token* const tokens, struct While* const dst, const struct Line* const line_info);
uint64_t parse_expr           (const struct Token* const tokens, struct Expr*  const dst, const struct Line* const line_info);


uint64_t build_scope_syntax_tree(struct Scope* const parent, const struct Token* const tokens, const struct Line* const line_info, struct Scope** const out_scope);
