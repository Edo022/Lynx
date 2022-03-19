#include "Utils.h"
#include <stdbool.h>
#include <stdlib.h>
#include "SyntaxAnalyzer/SyntaxAnalyzer.h"

//TODO add error codes and a detailed explanation for each error
//TODO add semantic analyzer
//TODO graphic output of the syntax tree

//FIXME check multiple definitions
//FIXME
//FIXME Multiple definitions of identifier "uwu"
//FIXME Definition 0 at myproject/file:54
//FIXME     int uwu;
//FIXME Definition 1 at myproject/file:993
//FIXME     void uwu(int a){
//TODO print a more detailed error if the identifier is a language keyword
//TODO <keyword used as identifier>

//TODO arr[0..n]

/**
 * @brief Returns the number of tokens that are between the left and right delimiters. This includes the delimiters
 * @param vTokens An array of tokens. The first token must be a left delimiter
 *     The token list must start with the first token after the first left delimiter
 * @param vTokenNum The total number of tokens
 * @param vLeft The left delimiter
 * @param vRight The right delimiter
 * @param iLines Line informations
 * @return The index of the first token after the right delimiter that matches the first left delimiter
 *     Prints an error if it is unmatched
 */
uint64_t stat_tok_group(const struct Token* const tokens, const enum TokenID left_del, const enum TokenID right_del, const struct Line* const line_info){
	uint64_t n = 1; // Count first left delimiter
	for(uint64_t i = 0; tokens[i].value; ++i) {
		if     (tokens[i].id == left_del) ++n;
		else if(tokens[i].id == right_del) {
			if(!--n) return i + 1;
		}
	}
	print_syntax_error(line_info[tokens->abs_line], "Unmatched delimiter \"%s\"", tokens->value);
}

/**
 * @brief Returns the index of the first occurrence of a token
 * @param vTokens An array of tokens
 * @param vTokenNum The total number of tokens
 * @param vToken The token to find
 * @return The index of the first occurrence of a token, or UINT64_MAX if there are none
 */
uint64_t stat_tok(const struct Token* const tokens, const enum TokenID tok){
	for(uint64_t i = 0;; ++i) if(tokens[i].id == tok) return i;
	return UINT64_MAX;
}










uint64_t build_statement_if(struct Scope* const parent, const struct Token* const tokens, struct If* const dst, const struct Line* const line_info){
	uint64_t i = 0;

	// Condition
	if(tokens->id != o_lgroup) print_syntax_error(line_info[tokens->abs_line], "Expected '(' after \"%s\" statement, token \"%s\" found instead", tokens[-1].value, tokens[i].value);
	++i; // Skip ')'
	i += parse_expr(tokens + i, &dst->condition, line_info);
	if(!dst->condition) print_syntax_error(line_info[tokens[i].abs_line], "Condition of \"%s\" statement cannot be empty", tokens[-1].value);
	if(tokens[i].id != o_rgroup) print_syntax_error(line_info[tokens[i].abs_line], "Expected ')' in condition of \"%s\" statement, token \"%s\" found instead", tokens[-1].value, tokens[i].value);
	++i; // Skip '('

	// Body
	if(tokens[i].id != o_lscope) print_syntax_error(line_info[tokens[i].abs_line], "Expected '{' after condition of \"%s\" statement, token \"%s\" found instead", tokens[-1].value, tokens[i].value);
	++i; // Skip '{'
	i += build_scope_syntax_tree(parent, tokens + i, line_info, &dst->body);

	// Else
	if(tokens[i].id == c_else) {
		++i; // Skip 'else'
		if(tokens[i].id != o_lscope) print_syntax_error(line_info[tokens[i].abs_line], "Expected '{' after \"%s\" statement, token \"%s\" found instead", tokens[i - 1].value, tokens[i].value);
		++i; // Skip '{'
		i += build_scope_syntax_tree(parent, tokens + i, line_info, &dst->_else);
	}
	else dst->_else = NULL;

	return i;
}


uint64_t build_statement_for(struct Scope* const parent, const struct Token* const tokens, struct For* const dst, const struct Line* const line_info){
	uint64_t i = 0;

	// Condition
	if(tokens->id != o_lgroup) print_syntax_error(line_info[tokens->abs_line], "Expected '(' after \"%s\" loop, token \"%s\" found instead", tokens[-1].value, tokens[i].value);
	++i; // Skip ')'
	i += parse_expr(tokens + i, &dst->init, line_info); //FIXME replace with var declaration parsing
	if(tokens[i].id != e_instruction_end) print_syntax_error(line_info[tokens[i].abs_line], "Expected ';' after initializer statement of \"%s\" loop, token \"%s\" found instead", tokens[-1].value, tokens[i].value);
	++i; // Skip ';'

	i += parse_expr(tokens + i, &dst->condition, line_info);
	if(!dst->condition) print_syntax_error(line_info[tokens[i].abs_line], "Condition of \"%s\" statement cannot be empty", tokens[-1].value);
	if(tokens[i].id != e_instruction_end) print_syntax_error(line_info[tokens[i].abs_line], "Expected ';' after condition of \"%s\" loop, token \"%s\" found instead", tokens[-1].value, tokens[i].value);
	++i; // Skip ';'

	i += parse_expr(tokens + i, &dst->last, line_info);
	if(tokens[i].id != o_rgroup) print_syntax_error(line_info[tokens[i].abs_line], "Expected ')' after iterated statement of \"%s\" loop, token \"%s\" found instead", tokens[-1].value, tokens[i].value);
	++i; // Skip '('

	// Body
	if(tokens[i].id != o_lscope) print_syntax_error(line_info[tokens[i].abs_line], "Expected '{' after arguments of \"%s\" loop, token \"%s\" found instead", tokens[-1].value, tokens[i].value);
	++i; // Skip '{'
	i += build_scope_syntax_tree(parent, tokens + i, line_info, &dst->body);

	// Then
	if(tokens[i].id == c_then) {
		++i; // Skip "then"
		if(tokens[i].id != o_lscope) print_syntax_error(line_info[tokens[i].abs_line], "Expected '{' after \"%s\" statement, token \"%s\" found instead", tokens[i - 1].value, tokens[i].value);
		++i; // Skip '{'
		i += build_scope_syntax_tree(parent, tokens + i, line_info, &dst->_then);
	}
	else dst->_then = NULL;

	// Else
	if(tokens[i].id == c_else) {
		++i; // Skip "else"
		if(tokens[i].id != o_lscope) print_syntax_error(line_info[tokens[i].abs_line], "Expected '{' after \"%s\" statement, token \"%s\" found instead", tokens[i - 1].value, tokens[i].value);
		++i; // Skip '{'
		i += build_scope_syntax_tree(parent, tokens + i, line_info, &dst->_else);
	}
	else dst->_else = NULL;

	return i;
}


uint64_t build_statement_while(struct Scope* const parent, const struct Token* const tokens, struct While* const dst, const struct Line* const line_info){
	uint64_t i = 0;

	// Condition
	if(tokens->id != o_lgroup) print_syntax_error(line_info[tokens->abs_line], "Expected '(' after \"%s\" loop, token \"%s\" found instead", tokens[-1].value, tokens[i].value);
	++i; // Skip ')'
	uint64_t arg_len = parse_expr(tokens + i, &dst->condition, line_info);
	i += arg_len;
	if(!arg_len) print_syntax_error(line_info[tokens[i].abs_line], "Condition of \"%s\" statement cannot be empty", tokens[-1].value);
	if(tokens[i].id != o_rgroup) print_syntax_error(line_info[tokens[i].abs_line], "Expected ')' in condition of \"%s\" loop, token \"%s\" found instead", tokens[-1].value, tokens[i].value);
	++i; // Skip '('

	// Body
	if(tokens[i].id != o_lscope) print_syntax_error(line_info[tokens[i].abs_line], "Expected '{' after condition of \"%s\" loop, token \"%s\" found instead", tokens[-1].value, tokens[i].value);
	++i; // Skip '{'
	i += build_scope_syntax_tree(parent, tokens + i, line_info, &dst->body);

	// Then
	if(tokens[i].id == c_then) {
		++i; // Skip "then"
		if(tokens[i].id != o_lscope) print_syntax_error(line_info[tokens[i].abs_line], "Expected '{' after \"%s\" statement, token \"%s\" found instead", tokens[i - 1].value, tokens[i].value);
		++i; // Skip '{'
		i += build_scope_syntax_tree(parent, tokens + i, line_info, &dst->_then);
	}
	else dst->_then = NULL;

	// Else
	if(tokens[i].id == c_else) {
		++i; // Skip "else"
		if(tokens[i].id != o_lscope) print_syntax_error(line_info[tokens[i].abs_line], "Expected '{' after \"%s\" statement, token \"%s\" found instead", tokens[i - 1].value, tokens[i].value);
		++i; // Skip '{'
		i += build_scope_syntax_tree(parent, tokens + i, line_info, &dst->_else);
	}
	else dst->_else = NULL;

	return i;
}


/**
 * @brief Parses an expression and //TODO
 *     The parsing stops at the first ';', unmatched ')', ']' or '}' or unenclosed ','
 * @param tokens
 * @param dst
 * @param line_info
 * @return The index of the last character of the expression
 */
uint64_t parse_expr(const struct Token* const tokens, struct Expr** const dst, const struct Line* const line_info){
	struct Expr* tmp_expr; //FIXME REMOVE, actually save the output expression;

	uint64_t i = 0;
	bool is_expr_end(const enum TokenID id) {
		return id == o_rgroup ||  id == o_rscope || id == e_instruction_end || id == e_list;
	}

	*dst = is_expr_end(tokens[i].id) ? NULL : malloc(sizeof(struct Expr));
	while(!is_expr_end(tokens[i].id)){
		if(!tokens[i].value) print_syntax_error(line_info[tokens[i].abs_line], "File ended unexpectedly while reading an expression");

		//FIXME grouping
		//TODO array subscript
		//TODO array ranges
		//TODO constructors
		//TODO range constructors
		if(tokens[i].id == o_lgroup){ // Grouping
			//TODO save
			++i; // Skip '('
			i += parse_expr(tokens + i, &tmp_expr, line_info); //FIXME
			if(tokens[i].id != o_rgroup) print_syntax_error(line_info[tokens[i].abs_line], "Expected ')' after expression, token \"%s\" found instead", tokens[i].value);
			++i; // Skip ')'
		}
		else if(tokens[i].id == e_user_defined){
			if(tokens[i + 1].id == o_lgroup){ // Functions
				//TODO save
				++i; // Skip function name
				while(tokens[i].id != o_rgroup) {
					++i; // Skip '(' or ','
					i += parse_expr(tokens + i, &tmp_expr, line_info); //FIXME
					if(tokens[i].id != e_list && tokens[i].id != o_rgroup) print_syntax_error(line_info[tokens[i].abs_line], "Expected ',' or ')' after expression list, token \"%s\" found instead", tokens[i].value);
				}
				++i; // Skip ')'
			}
			else { //Variables
				//TODO save
				++i;
			}
		}
		else if(tok_is_operator(tokens[i].id) || tokens[i].id == e_literal){ //TODO
			//TODO check the syntax
			//TODO save
			++i;
		}
		else print_syntax_error(line_info[tokens[i].abs_line], "Unexpected token \"%s\" inside an expression", tokens[i].value);
	}

	return i;
}







/**
 * @brief Creates an abstract syntax tree from an array of tokens
 * @param parent The address of the parent of this scope, or NULL if the scope is the global scope
 * @param vTokens An array of tokens that contain the scope
 *     If the scope is not the global scope, the first token must be its opening bracket
 * @param line_info Line informations
 * @param pScope the address of a pointer where to store the scope struct
 * @return The index of the first token after the scope
 */
uint64_t build_scope_syntax_tree(struct Scope* const parent, const struct Token* const tokens, const struct Line* const line_info, struct Scope** const out_scope){
	// Allocate the current scope
	*out_scope = malloc(sizeof(struct Scope));
	init_scope(*out_scope); (*out_scope)->parent = parent;


	// For each construct or instruction
	// uint64_t i = !!parent; // Skip own '{'
	uint64_t i = 0;
	while(tokens[i].id != o_rscope){
		// Unmatched brackets
		if(!tokens[i].value) {
			if(parent) print_syntax_error(line_info[tokens->abs_line], "Unmatched scope delimiter \"%s\"", tokens->value);
			else return i;
		}

		// Sub scopes
		if(tokens[i].id == o_lscope) { //TODO print error if in global scope
			if(parent){
				++i; // Skip subscope '{'
				struct Scope* sub_scope;
				uint64_t sub_scope_len = build_scope_syntax_tree(*out_scope, tokens + i, line_info, &sub_scope);
				scope_add_scp(*out_scope, sub_scope);
				i += sub_scope_len; // Skip subscope
			}
			else print_syntax_error(line_info[tokens[i].abs_line], "Unnamed scopes can only be used inside function definitions");
			//FIXME instruction analysis is skipped
		}

		// Variable or function definition //FIXME add const keyword
		else if(tok_is_type(tokens[i].id)){
			const struct Token* construct_type = &tokens[i++];
			if(tokens[i].id == e_user_defined) {
				const struct Token* construct_name = &tokens[i++];
				if(tokens[i].id == o_lgroup){ // And Codics says, "Let it be legal"; and legal it is

					//FIXME actually read the argument list
					struct Fun fun = {
						.name = construct_name->value,
						.type = construct_type->id,
						.param_num = 0, //TODO
						.scope = NULL,
						// .exec = NULL, //TODO
						// .parent = *pScope
					};
					++i; //Skip '('
					i += stat_tok_group(tokens + i, o_lgroup, o_rgroup, line_info);
					// fun.param_num = 0; //TODO
					//FIXME ACTUALLY READ THE ARGUMENTS AND SAVE THEM AS VARIABLES

					if(tokens[i].id != o_lscope) print_syntax_error(line_info[tokens[i].abs_line], "Expected token '{' after parameter list of function \"%s\", token \"%s\" found instead", construct_name->value, tokens[i].value);
					++i; // Skip '{'

					// Analyze the function definition
					uint64_t fun_scope_len = build_scope_syntax_tree(*out_scope, tokens + i, line_info, &fun.scope);
					i += fun_scope_len; //skip nested scope

					// Save the function
					scope_add_fun(*out_scope, &fun);
				}
				else {
					struct Var var = {
						.is_const = false,
						.name = construct_name->value,
						.type = construct_type->id,
						.parent = *out_scope
					};
					if(tokens[i].id == e_instruction_end){ //FIXME check variables declared as void
						var.init = NULL;
						++i; //Skip ;
					}
					else if(tokens[i].id == o_set){ //TODO ADD INLINE INITIALIZATION TO VARIABLES //FIXME check variables declared as void
						++i; // Skip =
						uint64_t expr_len = stat_tok(tokens + i, e_instruction_end);
						i += expr_len + 1; // len + ';'
						var.init = 1; //FIXME ACTUALLY READ THE EXPRESSION
					}
					else print_syntax_error(line_info[tokens[i].abs_line], "Expected a function argument list or a variable definition after global identifier \"%s\"", construct_name->value);
					scope_add_var(*out_scope, &var);
				}
			}
			else if(tok_is_keyword(tokens[i].id)) print_syntax_error(line_info[tokens[i].abs_line], "Keyword \"%s\" used as identifier", tokens[i].value);
			else print_syntax_error(line_info[tokens[i].abs_line], "Expected identifier after type \"%s\"", construct_type->value);
		}

		// Preprocessor directives //TODO
		else if(tokens[i].id == e_preprocessor) {
			//FIXME actually check the syntax and save the arguments
			while(tokens[i].abs_line == tokens[i + 1].abs_line) ++i;
			++i;
		}

		// Flow control



		// //FIXME MERGE
		// //FIXME MERGE
		// //FIXME MERGE
		// else if(parent && tokens[i].id == c_if){ //TODO replace with nested if else and a len variable
		// 	//TODO MOVE CONDITION PARSING TO buildTreeIf
		// 	if(tokens[++i].id != o_lgroup) print_syntax_error(line_info[tokens[i].abs_line], "Expected '(' after flow control construct \"if\""); //FIXME make scope operators optional with single instruction bodies
		// 	uint64_t if_expr_len = stat_tok_group(tokens + i, o_lgroup, o_rgroup, line_info);
		// 	i += if_expr_len;

		// 	if(tokens[i].id != o_lscope) print_syntax_error(line_info[tokens[i].abs_line], "Expected '{' or an expression after condition of flow control construct \"if\""); //FIXME make scope operators optional with single instruction bodies
		// 	uint64_t if_scope_len = stat_tok_group(tokens + i, o_lscope, o_rscope, line_info);
		// 	scope_add_inst_if(*out_scope, build_tree_if(tokens + i)); //TODO save this thing //FIXME implement the function
		// 	i += if_scope_len;
		// 	//FIXME read elif and else constructs
		// }
		// else if(parent && tokens[i].id == c_elif){ //TODO replace with nested if else and a len variable
		// 	if(tokens[++i].id != o_lgroup) print_syntax_error(line_info[tokens[i].abs_line], "Expected '(' after flow control construct \"elif\""); //FIXME make scope operators optional with single instruction bodies
		// 	uint64_t elif_expr_len = stat_tok_group(tokens + i, o_lgroup, o_rgroup, line_info);
		// 	//FIXME actually read the expression
		// 	i += elif_expr_len;

		// 	if(tokens[i].id != o_lscope) print_syntax_error(line_info[tokens[i].abs_line], "Expected '{' or an expression after condition of flow control construct \"elif\""); //FIXME make scope operators optional with single instruction bodies
		// 	uint64_t elif_scope_len = stat_tok_group(tokens + i, o_lscope, o_rscope, line_info);
		// 	// buildTreeIf(vTokens + i, elifScopeLen); //FIXME implement the function
		// 	//scope_add_inst_if //TODO
		// 	i += elif_scope_len;
		// 	//FIXME read elif and else constructs
		// }
		// else if(parent && tokens[i].id == c_else){ //TODO replace with nested if else and a len variable
		// 	if(tokens[++i].id != o_lscope) print_syntax_error(line_info[tokens[i].abs_line], "Expected '{' or an expression after condition of flow control construct \"else\""); //FIXME make scope operators optional with single instruction bodies
		// 	uint64_t else_scope_len = stat_tok_group(tokens + i, o_lscope, o_rscope, line_info);
		// 	// buildTreeIf(vTokens + i, elseScopeLen); //FIXME implement the function
		// 	//scope_add_inst_if //TODO
		// 	i += else_scope_len;
		// 	//FIXME read elif and else constructs
		// }
		// //FIXME MERGE
		// //FIXME MERGE
		// //FIXME MERGE





		// else if(parent && tokens[i].id == c_while){ //TODO replace with nested if else and a len variable
		// 	//TODO MOVE CONDITION PARSING TO buildTreeWhile
		// 	if(tokens[++i].id != o_lgroup) print_syntax_error(line_info[tokens[i].abs_line], "Expected '(' after flow control construct \"while\""); //FIXME make scope operators optional with single instruction bodies
		// 	uint64_t while_scope_expr = stat_tok_group(tokens + i, o_lgroup, o_rgroup, line_info);
		// 	//FIXME actually read the expression
		// 	i += while_scope_expr;

		// 	if(tokens[i].id != o_lscope) print_syntax_error(line_info[tokens[i].abs_line], "Expected '{' or an expression after condition of flow control construct \"while\""); //FIXME make scope operators optional with single instruction bodies
		// 	uint64_t while_scope_len = stat_tok_group(tokens + i, o_lscope, o_rscope, line_info);
		// 	scope_add_inst_while(*out_scope, build_tree_while(tokens + i)); //FIXME implement the function
		// 	i += while_scope_len;
		// }
		// else if(parent && tokens[i].id == c_for){ //TODO replace with nested if else and a len variable
		// 	//TODO MOVE CONDITION PARSING TO buildTreeFor
		// 	if(tokens[++i].id != o_lgroup) print_syntax_error(line_info[tokens[i].abs_line], "Expected '(' after flow control construct \"for\""); //FIXME make scope operators optional with single instruction bodies
		// 	uint64_t for_expr_len = stat_tok_group(tokens + i, o_lgroup, o_rgroup, line_info);
		// 	//FIXME actually read the expressions
		// 	i += for_expr_len;

		// 	if(tokens[i].id != o_lscope) print_syntax_error(line_info[tokens[i].abs_line], "Expected '{' or an expression after condition of flow control construct \"for\""); //FIXME make scope operators optional with single instruction bodies
		// 	uint64_t for_scope_len = stat_tok_group(tokens + i, o_lscope, o_rscope, line_info);
		// 	scope_add_inst_for(*out_scope, build_tree_for(tokens + i)); //FIXME implement the function
		// 	i += for_scope_len;
		// }
		// else if(parent && (tokens[i].id == e_literal || tokens[i].id == e_user_defined || tokens[i].id == o_sub  || tokens[i].id == e_instruction_end)){ //TODO add struct and base types constructors analysis
		// 	uint64_t expr_len = stat_tok(tokens + i, e_instruction_end);
		// 	scope_add_inst_expr(*out_scope, parse_expr(tokens + i));
		// 	i += expr_len + 1; // len + ';'
		// }


		else if(parent) switch(tokens[i].id){
			case c_if: {
				++i; // Skip "if"
				i += build_statement_if(parent, tokens + i, scope_new_statement_if(*out_scope), line_info);
				break;
			}
			case c_while: {
				++i; // Skip "while"
				i += build_statement_while(parent, tokens + i, scope_new_statement_while(*out_scope), line_info);
				break;
			}
			case c_for: {
				++i; // Skip "for"
				i += build_statement_for(parent, tokens + i, scope_new_statement_for(*out_scope), line_info);
				break;
			}
			default: {
				i += parse_expr(tokens + i, scope_new_statement_expr(*out_scope), line_info);
				if(tokens[i].id != e_instruction_end) print_syntax_error(line_info[tokens[i].abs_line], "Expected ';' after expression statement, token \"%s\" found instead", tokens[i].value);
				++i; // Skip ';'
				break;
			}
		}



		// Anything else is a syntax error
		else {
			print_syntax_error(line_info[tokens[i].abs_line], "Unexpected token \"%s\"", tokens[i].value);
		}
	}


	// Successful parsing
	++i;		// Skip own '}'
	return i;	// Return the index of the first token after it
}


