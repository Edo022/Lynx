#include "Utils.h"
#include <stdbool.h>
#include <stdlib.h>
#include "SyntaxAnalyzer/SyntaxAnalyzer.h"
//TODO add error codes and a detailed explanation for each error



/**
 * @brief Returns the number of tokens that are between the left and right delimiters. This includes the delimiters
 * @param vTokens An array of tokens. The first token must be a left delimiter
 *     If the first token is not a left delimiter, the return value is undefined
 * @param vTokenNum The total number of tokens
 * @param vLeft The left delimiter
 * @param vRight The right delimiter
 * @param iLines Line informations
 * @return The index of the right delimiter that matches the first left delimiter
 *     Prints an error if it is unmatched
 */
// uint64_t statTokGroup(const struct Token* const vTokens, const uint64_t vTokenNum, const enum TokenID vLeft, const enum TokenID vRight, const struct Line* const iLines){
uint64_t statTokGroup(const struct Token* const vTokens, const enum TokenID vLeft, const enum TokenID vRight, const struct Line* const iLines){
	uint64_t n = 1; // Skip first left delimiter
	for(uint64_t i = 1; vTokens[i].value; ++i) {
		if     (vTokens[i].id == vLeft) ++n;
		else if(vTokens[i].id == vRight) {
			if(!--n) return i + 1;
		}
	}
	printSyntaxError(iLines[vTokens->absLine], "Unmatched delimiter \"%s\"", vTokens->value);
}

/**
 * @brief Returns the index of the first occurrence of a token
 * @param vTokens An array of tokens
 * @param vTokenNum The total number of tokens
 * @param vToken The token to find
 * @return The index of the first occurrence of a token, or UINT64_MAX if there are none
 */
// uint64_t statTok(const struct Token* const vTokens, const uint64_t vTokenNum, const enum TokenID vToken){
uint64_t statTok(const struct Token* const vTokens, const enum TokenID vToken){
	for(uint64_t i = 0;; ++i) if(vTokens[i].id == vToken) return i;
	return UINT64_MAX;
}










struct If* buildTreeIf(const struct Token* const vTokens){

}


struct For* buildTreeFor(const struct Token* const vTokens){

}


struct While* buildTreeWhile(const struct Token* const vTokens){

}

struct Expr* parseExpr(const struct Token* const vTokens){

}







//TODO DONT SAVE WHITESPACE
//TODO add semantic analyzer
//TODO graphic output of the syntax tree
/**
 * @brief Creates an abstract syntax tree from an array of tokens
 * @param vParent The address of the parent of this scope, or NULL if the scope is the global scope
 * @param vTokens An array of tokens that contain the scope
 *     If the scope is not the global scope, the first token must be its opening bracket
 * @param iLines Line informations
 * @param pScope the address of a pointer where to store the scope struct
 * @return The index of the closing bracket of the scope
 */
uint64_t buildScopeSyntaxTree(struct Scope* const vParent, const struct Token* const vTokens, const struct Line* const iLines, struct Scope** const pScope){
	// Allocate the current scope
	*pScope = malloc(sizeof(struct Scope));
	initScope(*pScope); (*pScope)->parent = vParent;

	// Skip own '{'
	uint64_t i = !!vParent;

	//! Whitespace is not saved as tokens //TODO REMOVE
	// while(vTokens[i].id != o_rscope){ /*TODO checking .value is useless if the null terminator doesnt have o_rscope ID*/
	while(true){ /*TODO checking .value is useless if the null terminator doesnt have o_rscope ID*/
		if(!vTokens[i].value) {
			if(vParent) printSyntaxError(iLines[vTokens->absLine], "Unmatched scope delimiter \"%s\"", vTokens->value);
			else return i;
		}
		else if(vTokens[i].id == o_rscope) {
			// if(vParent && !--scopeDepth) return s;
			++i; // Skip own '}'
			// return s;
			return i;
		}

		// printf("line %d | token \"%s\" | ID %d\n", curLine, vTokens[i].value, vTokens[i].id); fflush(stdout); //TODO REMOVE

		// Scope delimiters
		if(vTokens[i].id == o_lscope) { //TODO print error if in global scope
			if(vParent){
				// uint64_t subScopeLen = statTokGroup(vTokens + i, vTokenNum - i, o_lscope, o_rscope, iLines);
				// uint64_t subScopeLen = statTokGroup(vTokens + i, o_lscope, o_rscope, iLines);
				struct Scope* subScope;
				uint64_t subScopeSize = buildScopeSyntaxTree(*pScope, vTokens + i, iLines, &subScope);
				addScp(*pScope, subScope);
				i += subScopeSize; // Skip subscope
			}
			else printSyntaxError(iLines[vTokens[i].absLine], "Unnamed scopes can only be used inside function definitions");
			//FIXME instruction analysis is skipped
		}

		// Variable or function definition //FIXME add const keyword
		else if(isType(vTokens[i].id)){
			// printf("line %d | token \"%s\" | ID %d\n", vTokens[i].locLine + 1, vTokens[i].value, vTokens[i].id); fflush(stdout); //TODO REMOVE
			const struct Token* constructType = &vTokens[i++];
			if(vTokens[i].id == e_user_defined) {
				const struct Token* constructName = &vTokens[i++];
				// printf("A\n"); fflush(stdout); //TODO REMOVE
				// printf("line %d | token \"%s\" | ID %d\n", vTokens[i].locLine + 1, vTokens[i].value, vTokens[i].id); fflush(stdout); //TODO REMOVE
				if(vTokens[i].id == o_lgroup){
					//FIXME actually read the argument list
					struct Fun fun = {
						.name = constructName->value,
						.type = constructType->id,
						.paramNum = 0, //TODO
						.scope = NULL,
						// .exec = NULL, //TODO
						.parent = *pScope
					};
					i += statTokGroup(vTokens + i, o_lgroup, o_rgroup, iLines);
					// fun.paramNum = 0; //TODO
					//FIXME ACTUALLY READ THE ARGUMENTS AND SAVE THEM AS VARIABLES
					// ++i; //Skip ')'

					// Analyze the function definition
					// uint64_t funScopeLen = statTokGroup(vTokens + i, o_lscope, o_rscope, iLines);
					uint64_t funScopeLen = buildScopeSyntaxTree(*pScope, vTokens + i, iLines, &fun.scope);
					i += funScopeLen; //skip nested scope

					// Save the function
					addFun(*pScope, &fun);
				}
				else {
					struct Var var = {
						.is_const = false,
						.name = constructName->value,
						.type = constructType->id,
						.parent = *pScope
					};
					if(vTokens[i].id == e_instruction_end){ //FIXME check variables declared as void
						var.init = NULL;
						++i; //Skip ;
					}
					else if(vTokens[i].id == o_set){ //TODO ADD INLINE INITIALIZATION TO VARIABLES //FIXME check variables declared as void
						++i; // Skip =
						uint64_t exprLen = statTok(vTokens + i, e_instruction_end);
						i += exprLen + 1; // len + ';'
						var.init = 1; //FIXME ACTUALLY READ THE EXPRESSION
					}
					else printSyntaxError(iLines[vTokens[i].absLine], "Expected a function argument list or a variable definition after global identifier \"%s\"", constructName->value);
					addVar(*pScope, &var);
				}
			}
			//FIXME check multiple definitions
			//FIXME
			//FIXME Multiple definitions of identifier "uwu"
			//FIXME Definition 0 at myproject/file:54
			//FIXME     int uwu;
			//FIXME Definition 1 at myproject/file:993
			//FIXME     void uwu(int a){
			//TODO print a more detailed error if the identifier is a language keyword
			//TODO <keyword used as identifier>
			else if(isKeyword(vTokens[i].id)) printSyntaxError(iLines[vTokens[i].absLine], "Keyword \"%s\" used as identifier", vTokens[i].value);
			else printSyntaxError(iLines[vTokens[i].absLine], "Expected identifier after type \"%s\"", constructType->value);
		}

		// // Newline tokens
		// else if(vTokens[i].id == e_newline){
		// 	++curLine; ++i;
		// }

		// Preprocessor directives //TODO
		else if(vTokens[i].id == e_preprocessor) {
			//FIXME actually check the syntax and save the arguments
			while(vTokens[i].absLine == vTokens[i + 1].absLine) ++i;
			++i;
		}

		// Flow control



		//FIXME MERGE
		//FIXME MERGE
		//FIXME MERGE
		else if(vParent && vTokens[i].id == c_if){ //TODO replace with nested if else and a len variable
			//TODO MOVE CONDITION PARSING TO buildTreeIf
			if(vTokens[++i].id != o_lgroup) printSyntaxError(iLines[vTokens[i].absLine], "Expected '(' after flow control construct \"if\""); //FIXME make scope operators optional with single instruction bodies
			uint64_t ifSExprLen = statTokGroup(vTokens + i, o_lgroup, o_rgroup, iLines);
			i += ifSExprLen;

			if(vTokens[i].id != o_lscope) printSyntaxError(iLines[vTokens[i].absLine], "Expected '{' or an expression after condition of flow control construct \"if\""); //FIXME make scope operators optional with single instruction bodies
			uint64_t ifScopeLen = statTokGroup(vTokens + i, o_lscope, o_rscope, iLines);
			addInstructionIf(*pScope, buildTreeIf(vTokens + i)); //TODO save this thing //FIXME implement the function
			i += ifScopeLen;
			//FIXME read elif and else constructs
		}
		else if(vParent && vTokens[i].id == c_elif){ //TODO replace with nested if else and a len variable
			if(vTokens[++i].id != o_lgroup) printSyntaxError(iLines[vTokens[i].absLine], "Expected '(' after flow control construct \"elif\""); //FIXME make scope operators optional with single instruction bodies
			uint64_t elifSExprLen = statTokGroup(vTokens + i, o_lgroup, o_rgroup, iLines);
			//FIXME actually read the expression
			i += elifSExprLen;

			if(vTokens[i].id != o_lscope) printSyntaxError(iLines[vTokens[i].absLine], "Expected '{' or an expression after condition of flow control construct \"elif\""); //FIXME make scope operators optional with single instruction bodies
			uint64_t elifScopeLen = statTokGroup(vTokens + i, o_lscope, o_rscope, iLines);
			// buildTreeIf(vTokens + i, elifScopeLen); //FIXME implement the function
			//addInstructionIf //TODO
			i += elifScopeLen;
			//FIXME read elif and else constructs
		}
		else if(vParent && vTokens[i].id == c_else){ //TODO replace with nested if else and a len variable
			if(vTokens[++i].id != o_lscope) printSyntaxError(iLines[vTokens[i].absLine], "Expected '{' or an expression after condition of flow control construct \"else\""); //FIXME make scope operators optional with single instruction bodies
			uint64_t elseScopeLen = statTokGroup(vTokens + i, o_lscope, o_rscope, iLines);
			// buildTreeIf(vTokens + i, elseScopeLen); //FIXME implement the function
			//addInstructionIf //TODO
			i += elseScopeLen;
			//FIXME read elif and else constructs
		}
		//FIXME MERGE
		//FIXME MERGE
		//FIXME MERGE







		else if(vParent && vTokens[i].id == c_while){ //TODO replace with nested if else and a len variable
			//TODO MOVE CONDITION PARSING TO buildTreeWhile
			if(vTokens[++i].id != o_lgroup) printSyntaxError(iLines[vTokens[i].absLine], "Expected '(' after flow control construct \"while\""); //FIXME make scope operators optional with single instruction bodies
			uint64_t whileExprLen = statTokGroup(vTokens + i, o_lgroup, o_rgroup, iLines);
			//FIXME actually read the expression
			i += whileExprLen;

			if(vTokens[i].id != o_lscope) printSyntaxError(iLines[vTokens[i].absLine], "Expected '{' or an expression after condition of flow control construct \"while\""); //FIXME make scope operators optional with single instruction bodies
			uint64_t whileScopeLen = statTokGroup(vTokens + i, o_lscope, o_rscope, iLines);
			addInstructionWhile(*pScope, buildTreeWhile(vTokens + i)); //FIXME implement the function
			i += whileScopeLen;
		}
		else if(vParent && vTokens[i].id == c_for){ //TODO replace with nested if else and a len variable
			//TODO MOVE CONDITION PARSING TO buildTreeFor
			if(vTokens[++i].id != o_lgroup) printSyntaxError(iLines[vTokens[i].absLine], "Expected '(' after flow control construct \"for\""); //FIXME make scope operators optional with single instruction bodies
			uint64_t forExprLen = statTokGroup(vTokens + i, o_lgroup, o_rgroup, iLines);
			//FIXME actually read the expressions
			i += forExprLen;

			if(vTokens[i].id != o_lscope) printSyntaxError(iLines[vTokens[i].absLine], "Expected '{' or an expression after condition of flow control construct \"for\""); //FIXME make scope operators optional with single instruction bodies
			uint64_t forScopeLen = statTokGroup(vTokens + i, o_lscope, o_rscope, iLines);
			addInstructionFor(*pScope, buildTreeFor(vTokens + i)); //FIXME implement the function
			i += forScopeLen;
		}
		else if(vParent && (vTokens[i].id == e_literal || vTokens[i].id == e_user_defined || vTokens[i].id == o_sub  || vTokens[i].id == e_instruction_end)){ //TODO add struct and base types constructors analysis
			uint64_t exprLen = statTok(vTokens + i, e_instruction_end);
			addInstructionExpr(*pScope, parseExpr(vTokens + i));
			i += exprLen + 1; // len + ';'
		}

		// Anything else
		else printSyntaxError(iLines[vTokens[i].absLine], "Unexpected token \"%s\"", vTokens[i].value);
	}
	// if(vParent) printSyntaxError(iLines[vTokens[i].absLine], "Unmatched scope");
	// //! Unmatched scope '{' are checked in statTokGroup
	// return i + 1; //len + own '}'
}


