#include "Utils.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "Tokenizer/Tokenizer.h"
#include "Data/Types.h"
#include "Data/Keywords.h"
#include "Data/Literals.h"
#include "Data/Operators.h"





uint64_t findSpaces(const char* const vLine){
	for(uint64_t i = 0;; ++i) {
		if(vLine[i] != ' ') return i;
	}
}




uint64_t getPreprocessor(const char* const vLine, struct Token* const pToken){
	if(*vLine == '#'){
		pToken->value = strdup("#");
		pToken->len   = 1;
		pToken->id    = e_preprocessor;
		pToken->data  = NULL;
		return 1;
	}
	return 0;
}




uint64_t getIdentifier(const char* const vLine, struct Token* const pToken){
	if(isalpha(vLine[0]) || vLine[0] == '_'){
		uint64_t i;
		for(i = 1; isalnum(vLine[i]) || vLine[i] == '_'; ++i);	// Get the length
		pToken->value = strndup(vLine, i);						// Save the identifier
		pToken->len   = i;										// Save the length

		// Types
		for(uint64_t t = t_start; t < t_end; ++t){				// For each hard coded type
			if(!strcmp(pToken->value, typeValues[t - t_start])){	// If it matches the current identifier
				pToken->id   = t;										// Set token id to the corresponding type id
				pToken->data = &typeData[t - t_start];					// Set token data to the hard coded data of the corresponding type
				return i;												// Return the length
			}
		}

		// Keywords
		for(uint64_t k = k_start; k < k_end; ++k){				// For each hard coded keyword
			if(!strcmp(pToken->value, keywordValues[k - k_start])){	// If it matches the current identifier
				pToken->id   = k;										// Set token id to the corresponding identifier id
				pToken->data = NULL;									// Set token data to NULL
				return i;												// Return the length
			}
		}

		// User defined
		pToken->id    = e_user_defined;							// Set token id to user defined identifier
		pToken->data  = NULL;									// Set token data to NULL
		return i;												// Return the length
	}
	return 0;
}




//TODO comment
uint64_t getLiteral(const char* vLine, struct Token* const pToken, const struct Line iLineInfo){
	if(isdigit(vLine[0])){
		pToken->data = malloc(sizeof(struct LiteralData_t));			// Allocate a block for the data

		uint64_t isValid = 1, i, base;									// 1 if the literal is valid, loop index and literal length, literal base
		uint64_t offset = (vLine[0] == '0' && isalpha(vLine[1])) ? 2 : 0;	// 0 or 2 if the literal has a prefix
		const char* baseName;											// The complete name of the base

		// Get literal base and complete base name
		if     (vLine[0] == '0' && vLine[1] == 'b') { base =  2; baseName = "binary";      }
		else if(vLine[0] == '0' && vLine[1] == 'o') { base =  8; baseName = "octal";       }
		else if(vLine[0] == '0' && vLine[1] == 'x') { base = 16; baseName = "hexadecimal"; }
		else                                        { base = 10; baseName = "decimal";     }

		// Get literal length and type and check if it's valid
		enum TokenID* literalType = &((struct LiteralData_t*)pToken->data)->type;	// Cache the address of the type
		*literalType = t_u32;														// Default type is u32, set to f64 if the loop finds a '.'
		for(i = offset; isalnum(vLine[i]) || vLine[i] == '.'; ++i){
			if(vLine[i] != '.') isValid &= !!(base == 16 ? isxdigit(vLine[i]) : vLine[i] >= '0' && vLine[i] <= '0' + base);
			else if(*literalType == t_u32) *literalType = t_f64; else isValid = 0;
		}

		// Get and save the string value of the literal and print an error if it's not valid
		pToken->value = strndup(vLine, i);
		if(!isValid) printSyntaxError(iLineInfo, "Invalid %s literal \"%s\"", baseName, pToken->value);

		// Convert the literal to an unsigned or a double and save it in the value of the allocated data
		if(*literalType == t_u32) *(uint32_t*)(&((struct LiteralData_t*)pToken->data)->value) =  strtoul(pToken->value + offset, NULL, base);
		else                        *(double*)(&((struct LiteralData_t*)pToken->data)->value) = bstrtolf(pToken->value + offset,       base);

		pToken->len = i;			// Save the length
		pToken->id  = e_literal;	// Set token id to literal
		return i;					// Return the length
	}
	return 0;
}
//TODO check empty literals like 0x, 0d, 0o
//TODO check decimal literals starting with .




uint64_t getOperator(const char* const vLine, struct Token* const pToken){
	for(uint64_t opLen, o = o_start; o < o_end; ++o){			// For each hard coded operator
		opLen = strlen(operatorValues[o - o_start]);			// Cache operator length
		if(!strncmp(vLine, operatorValues[o - o_start], opLen)){// If it matches the current operator
			pToken->value = operatorValues[o - o_start];			// Save the operator
			pToken->len   = opLen;									// Save the length
			pToken->id    = o;										// Set token id to the corresponding operator id
			pToken->data  = &operatorData[o - o_start];				// Set token data to the hard coded data of the corresponding operator
			return opLen;											// Return the length
		}
	}
	return 0;
}




uint64_t getUnknown(const char* const vLine, struct Token* const pToken) {
	pToken->value = strndup(vLine, 1);
	pToken->len   = 1;
	pToken->id    = e_unknown;
	pToken->data  = NULL;
	return 1;
}



uint64_t getInstructionEnd(const char* const vLine, struct Token* const pToken) {
	if(vLine[0] == ';'){
		pToken->value = strdup(";");
		pToken->len   = 1;
		pToken->id    = e_instruction_end;
		pToken->data  = NULL;
		return 1;
	}
	return 0;
}








//TODO replace tabs with spaces
struct Token* tokenize(struct Line* const vLines, const uint64_t vLineNum, uint64_t* pNum, const char* const iFileName){
	struct Token* const ret = malloc(sizeof(struct Token) * MAX_TOKENS);
	uint64_t tok_j = 0;
	for(uint64_t i = 0; i < vLineNum; ++i){
		char* const l = vLines[i].value;
		const uint64_t lLen = strlen(l);
		char* leading_ws = NULL;
		for(uint64_t j = 0; j < lLen; ++tok_j){
			struct Token* const curToken = ret + tok_j;	// Cache the address of the current token
			curToken->lineNum = i;						// Set the number of the line
			curToken->start   = j;						// Set the start index to j
			curToken->leading_ws = leading_ws ? strdup(leading_ws) : NULL; // Save leading whitespace


			// Get leading whitespace
			const uint64_t wsLen = findSpaces(l + j);
			if(wsLen){
				leading_ws = strndup(l + j, wsLen);
				j += wsLen; --tok_j; continue;
			}
			else leading_ws = NULL;

			// Find the first token, save into the array and update j
			uint64_t tokLen;
			if     (tokLen = getPreprocessor   (l + j, curToken)){}
			else if(tokLen = getInstructionEnd (l + j, curToken)){}
			else if(tokLen = getIdentifier     (l + j, curToken)){}
			else if(tokLen = getLiteral        (l + j, curToken, vLines[i])){}
			else if(tokLen = getOperator       (l + j, curToken)){}
			else    tokLen = getUnknown        (l + j, curToken);
			j += tokLen;
		}

		//Add newline token
		if(i < vLineNum - 1) {
			struct Token* const curToken = ret + tok_j;
			curToken->leading_ws = leading_ws ? strdup(leading_ws) : NULL;

			curToken->value   = strdup("\n");
			curToken->len     = 1;
			curToken->id      = e_newline;
			curToken->data    = NULL;
			curToken->lineNum = i;
			curToken->start   = 0;

			++tok_j;
		}
	}
	*pNum = tok_j;
	return ret;
}



