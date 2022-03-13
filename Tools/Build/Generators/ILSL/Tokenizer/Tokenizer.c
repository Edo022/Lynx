#include "Utils.h"
#include "Tokenizer/Tokenizer.h"

#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "Tokenizer/Operators.h"
#include "Tokenizer/Types.h"
#include "Tokenizer/Keywords.h"
#include "Tokenizer/Literals.h"





uint64_t push_str_preprocessor(const char* const line, struct Token* const out_tok){
	if(*line == '#'){
		out_tok->value = strdup("#");
		out_tok->len   = 1;
		out_tok->id    = e_preprocessor;
		out_tok->data  = NULL;
		return 1;
	}
	return 0;
}




uint64_t push_str_identifier(const char* const line, struct Token* const out_tok){
	if(isalpha(line[0]) || line[0] == '_'){
		uint64_t i;
		for(i = 1; isalnum(line[i]) || line[i] == '_'; ++i);	// Get the length
		out_tok->value = strndup(line, i);						// Save the identifier
		out_tok->len   = i;										// Save the length

		// Types
		for(uint64_t t = t_start; t <= t_end; ++t){				// For each hard coded type
			if(!strcmp(out_tok->value, type_to_str(t))){	// If it matches the current identifier
				out_tok->id   = t;										// Set token id to the corresponding type id
				out_tok->data = type_get_data(t);					// Set token data to the hard coded data of the corresponding type
				return i;												// Return the length
			}
		}

		// Keywords
		for(uint64_t k = k_start; k <= k_end; ++k){				// For each hard coded keyword
			if(!strcmp(out_tok->value, keyword_to_str(k))){	// If it matches the current identifier
				out_tok->id   = k;										// Set token id to the corresponding identifier id
				out_tok->data = NULL;									// Set token data to NULL
				return i;												// Return the length
			}
		}

		// User defined
		out_tok->id    = e_user_defined;							// Set token id to user defined identifier
		out_tok->data  = NULL;									// Set token data to NULL
		return i;												// Return the length
	}
	return 0;
}




//TODO comment
uint64_t push_str_literal(const char* line, struct Token* const out_tok, const struct Line line_info){
	if(isdigit(line[0])){
		out_tok->data = malloc(sizeof(struct LiteralData_t));			// Allocate a block for the data

		uint64_t is_valid = 1, i, base;									// 1 if the literal is valid, loop index and literal length, literal base
		uint64_t offset = (line[0] == '0' && isalpha(line[1])) ? 2 : 0;	// 0 or 2 if the literal has a prefix
		const char* fancy_base_name;											// The complete name of the base

		// Get literal base and complete base name
		if     (line[0] == '0' && line[1] == 'b') { base =  2; fancy_base_name = "binary";      }
		else if(line[0] == '0' && line[1] == 'o') { base =  8; fancy_base_name = "octal";       }
		else if(line[0] == '0' && line[1] == 'x') { base = 16; fancy_base_name = "hexadecimal"; }
		else                                      { base = 10; fancy_base_name = "decimal";     }

		// Get literal length and type and check if it's valid
		enum TokenID* literal_type = &((struct LiteralData_t*)out_tok->data)->type;	// Cache the address of the type
		*literal_type = t_u32;														// Default type is u32, set to f64 if the loop finds a '.'
		for(i = offset; isalnum(line[i]) || line[i] == '.'; ++i){
			if(line[i] != '.') is_valid &= !!(base == 16 ? isxdigit(line[i]) : line[i] >= '0' && line[i] <= '0' + base);
			else if(*literal_type == t_u32) *literal_type = t_f64; else is_valid = 0;
		}

		// Get and save the string value of the literal and print an error if it's not valid
		out_tok->value = strndup(line, i);
		if(!is_valid) print_syntax_error(line_info, "Invalid %s literal \"%s\"", fancy_base_name, out_tok->value);

		// Convert the literal to an unsigned or a double and save it in the value of the allocated data
		if(*literal_type == t_u32) *(uint32_t*)(&((struct LiteralData_t*)out_tok->data)->value) =  strtoul(out_tok->value + offset, NULL, base);
		else                        *(double*)(&((struct LiteralData_t*)out_tok->data)->value) = bstrtolf(out_tok->value + offset,       base);

		out_tok->len = i;			// Save the length
		out_tok->id  = e_literal;	// Set token id to literal
		return i;					// Return the length
	}
	return 0;
}
//TODO check empty literals like 0x, 0d, 0o
//TODO check decimal literals starting with .




uint64_t push_str_operator(const char* const line, struct Token* const out_tok){
	for(uint64_t op_len, o = o_start; o <= o_end; ++o){			// For each hard coded operator
		op_len = strlen(operator_to_string(o));			// Cache operator length
		if(!strncmp(line, operator_to_string(o), op_len)){// If it matches the current operator
			out_tok->value = operator_to_string(o);			// Save the operator
			out_tok->len   = op_len;									// Save the length
			out_tok->id    = o;										// Set token id to the corresponding operator id
			out_tok->data  = operator_get_data(o);				// Set token data to the hard coded data of the corresponding operator
			return op_len;											// Return the length
		}
	}
	return 0;
}




uint64_t push_str_unknown(const char* const line, struct Token* const out_tok) {
	out_tok->value = strndup(line, 1);
	out_tok->len   = 1;
	out_tok->id    = e_unknown;
	out_tok->data  = NULL;
	return 1;
}



uint64_t push_str_list(const char* const line, struct Token* const out_tok) {
	if(line[0] == ','){
		out_tok->value = strdup(",");
		out_tok->len   = 1;
		out_tok->id    = e_list;
		out_tok->data  = NULL;
		return 1;
	}
	return 0;
}



uint64_t push_str_inst_end(const char* const line, struct Token* const out_tok) {
	if(line[0] == ';'){
		out_tok->value = strdup(";");
		out_tok->len   = 1;
		out_tok->id    = e_instruction_end;
		out_tok->data  = NULL;
		return 1;
	}
	return 0;
}








//TODO replace tabs with spaces
uint64_t tokenize(struct Line* const lines, const char* const file_name, struct Token** out_tokens){
	*out_tokens = malloc(sizeof(struct Token) * MAX_TOKENS); //TODO USE DYNAMIC ALLOCATION

	uint64_t i = 0, tok_j = 0;
	for(; lines[i].str_val; ++i){ //TODO use null terminator
		char* const l = lines[i].str_val;
		const uint64_t line_len = strlen(l);
		for(uint64_t j = 0; j < line_len; ++tok_j){
			struct Token* const cur_tok = *out_tokens + tok_j;	// Cache the address of the current token
			cur_tok->abs_line = i;						// Set the absolute index of the line
			cur_tok->loc_line = lines[i].loc_line;		// Set the local index of the line
			cur_tok->start   = j;						// Set the start index to j

			// Skip whitespace
			uint64_t whitespace_len = count_chars(l + j, ' ');
			if(whitespace_len){
				j += whitespace_len;
				--tok_j; continue;
			}

			// Find the first token, save it in the array and update j
			uint64_t tok_len;
			if     (tok_len = push_str_preprocessor(l + j, cur_tok)){}
			else if(tok_len = push_str_list        (l + j, cur_tok)){}
			else if(tok_len = push_str_inst_end    (l + j, cur_tok)){}
			else if(tok_len = push_str_identifier  (l + j, cur_tok)){}
			else if(tok_len = push_str_literal     (l + j, cur_tok, lines[i])){}
			else if(tok_len = push_str_operator    (l + j, cur_tok)){}
			else    tok_len = push_str_unknown     (l + j, cur_tok);
			j += tok_len;
		}
	}

	// Null terminator token //TODO MOVE TO Tokens.h
	(*out_tokens)[tok_j].value    = NULL;
	(*out_tokens)[tok_j].data     = NULL;
	(*out_tokens)[tok_j].id       = e_NULL;
	(*out_tokens)[tok_j].abs_line = (uint64_t)-1;
	(*out_tokens)[tok_j].loc_line = (uint64_t)-1;
	(*out_tokens)[tok_j].len      = 0;
	(*out_tokens)[tok_j].start    = 0;

	return i;
}



