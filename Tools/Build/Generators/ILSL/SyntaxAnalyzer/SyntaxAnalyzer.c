#include "Utils.h"
#include <stdbool.h>
#include <stdlib.h>
#include "SyntaxAnalyzer/SyntaxAnalyzer.h"




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
uint64_t statTokGroup(const struct Token* const vTokens, const uint64_t vTokenNum, const enum TokenID vLeft, const enum TokenID vRight, const struct Line* const iLines){
	uint64_t n = 1;
	for(uint64_t i = 1; i < vTokenNum; ++i) {
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
uint64_t statTok(const struct Token* const vTokens, const uint64_t vTokenNum, const enum TokenID vToken){
	for(uint64_t i = 0; i < vTokenNum; ++i) if(vTokens[i].id == vToken) return i;
	return UINT64_MAX;
}










struct OpNode* buildTreeIf(   const struct Token* const vTokens, const uint64_t vTokenNum){

}


struct OpNode* buildTreeFor(  const struct Token* const vTokens, const uint64_t vTokenNum){

}


struct OpNode* buildTreeWhile(const struct Token* const vTokens, const uint64_t vTokenNum){

}









//TODO DONT SAVE WHITESPACE
//TODO add semantic analyzer
//TODO graphic output of the syntax tree
/**
 * @brief Creates an abstract syntax tree from an array of tokens
 * @param vParent The address of the parent of this scope, or NULL if the scope is the global scope
 * @param vTokens An array of tokens that contain the scope
 *     If the scope is not the global scope, the first token must be its opening bracket
 * @param vTokenNum The maximum number of tokens to read
 *     Prints an error if the opening bracket isn't matched
 * @param iLines Line informations
 * @return A Scope struct containing the syntax tree of the scope
 */
struct Scope* buildScopeSyntaxTree(struct Scope* const vParent, const struct Token* const vTokens, const uint64_t vTokenNum, const struct Line* const iLines){
	// Current scope
	struct Scope* s = malloc(sizeof(struct Scope));
	initScope(s); s->parent = vParent;


	//! Whitespace is not saved as tokens //TODO REMOVE
	uint64_t scopeDepth = !vParent;
	for(uint64_t i = 0; i < vTokenNum;){
		// printf("line %d | token \"%s\" | ID %d\n", curLine, vTokens[i].value, vTokens[i].id); fflush(stdout); //TODO REMOVE

		// Scope delimiters
		if(vTokens[i].id == o_lscope) { //TODO print error if in global scope
			if(vParent){
				if(++scopeDepth > 1) {
					uint64_t subScopeLen = statTokGroup(vTokens + i, vTokenNum - i, o_lscope, o_rscope, iLines);
					addScp(s, buildScopeSyntaxTree(s, vTokens + i, subScopeLen, iLines));
					i += subScopeLen; // Skip subscope
				}
				else ++i; // Skip own '{'
			}
			else printSyntaxError(iLines[vTokens[i].absLine], "Unnamed scopes can only be used inside function definitions"); //FIXME error doesnt work
			//FIXME instruction analysis is skipped
		}
		else if(vTokens[i].id == o_rscope) {
			// if(vParent && !--scopeDepth) return s;
			++i; // Skip own '}'
			return s;
		}

		// Variable or function definition //FIXME add const keyword
		else if(isType(vTokens[i].id)){
			// printf("line %d | token \"%s\" | ID %d\n", vTokens[i].locLine + 1, vTokens[i].value, vTokens[i].id); fflush(stdout); //TODO REMOVE
			const struct Token* constructType = &vTokens[i++];
			if(i < vTokenNum && vTokens[i].id == e_user_defined) {
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
						.parent = s
					};
					i += statTokGroup(vTokens + i, vTokenNum - i, o_lgroup, o_rgroup, iLines);
					// ++i; //Skip ')'

					// Analyze the function definition
					uint64_t funScopeLen = statTokGroup(vTokens + i, vTokenNum - i, o_lscope, o_rscope, iLines);
					fun.scope = *buildScopeSyntaxTree(s, vTokens + i, funScopeLen, iLines);
					i += funScopeLen; //skip nested scope

					// Save the function
					addFun(s, &fun);
				}
				else if(vTokens[i].id == e_instruction_end){ //FIXME check variables declared as void
					struct Var var = {
						.is_const = false,
						.name = constructName->value,
						.type = constructType->id,
						.parent = s
					};
					addVar(s, &var);
					++i; //Skip ;
				}
				// else if(vTokens[i].id == o_log_eq){ //TODO ADD INLINE INITIALIZATION TO VARIABLES //FIXME check variables declared as void
				else printSyntaxError(iLines[vTokens[i].absLine], "Expected a function argument list or a variable definition after global identifier \"%s\"", constructName->value);
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

		// Preprocessor directives
		else if(vTokens[i].id == e_preprocessor) {
			//FIXME actually check the syntax and save the arguments
			while(i < vTokenNum && vTokens[i].absLine == vTokens[i + 1].absLine) ++i;
			++i;
		}

		// Flow control
		else if(vParent && vTokens[i].id == c_if){ //TODO replace with nested if else and a len variable
			buildTreeIf(vTokens + i, vTokenNum - i);
			//TODO move to parent scope

		}
		// else if(vParent && (vTokens[i].id == e_literal || vTokens[i].id == e_user_defined || vTokens[i].id == o_sub)){

		// Anything else
		else {
			printf("[%d]\n",vTokens[i].id);
			printSyntaxError(iLines[vTokens[i].absLine], "Unexpected token \"%s\"", vTokens[i].value);
		}
	}
	// if(vParent) printSyntaxError(iLines[vTokens[i].absLine], "Unmatched scope");
	//! Unmatched '{' are checked in statTokGroup
	return s;
}


//TODO REMOVE
struct Scope* buildSyntaxTree(const struct Token* const vTokens, const uint64_t vTokenNum, const struct Line* const iLines){
	struct Scope* g = buildScopeSyntaxTree(NULL, vTokens, vTokenNum, iLines);
}
