#include "Translator.h"
#include "Tokenizer/Types.h"

#define TAB_SIZE 4

void indent(const uint64_t depth, FILE* const f){
	fprintf(f, "\n%*s", TAB_SIZE * depth, "");
}


static void translate_var(const uint64_t depth, struct Var* const var, FILE* const f){
	indent(depth, f); fprintf(f, "");
	if(var->is_const) fprintf(f, "const ");
	fprintf(f, "%s %s", type_get_data(var->type)->glsl_type, var->name);
	if(var->init) fprintf(f, " = %s", "?");
	fprintf(f, ";");
}




static void translate_fun(const uint64_t depth, struct Fun* const fun, FILE* const f){
	indent(depth, f);
	fprintf(f, "%s %s(", type_get_data(fun->type)->glsl_type, fun->name);
	for(uint64_t j = 0; j < fun->param_num; ++j){
		// strcat(outputStr, );
	}
	fprintf(f, "?"); //TODO actually read the arguments
	fprintf(f, ") {");
	translate_scope(depth + 1, fun->scope, f);
	indent(depth, f); fprintf(f, "}");
}




static void translate_instruction(const uint64_t depth, struct Instruction* const instruction, FILE* const f){
	switch(instruction->type){
		case inst_if: {
			// fprintf(f, s->inst_arr[i].data._if->condition); //TODO
			indent(depth, f); fprintf(f, "if(%s){", "?");
			translate_scope(depth + 1, instruction->data._if->body, f);
			indent(depth, f); fprintf(f, "}");

			if(instruction->data._if->_else) {
				indent(depth, f); fprintf(f, "else {");
				translate_scope(depth + 1, instruction->data._if->_else, f);
				indent(depth, f); fprintf(f, "}");
			}

			break;
		}
		case inst_for:  {
			indent(depth, f); fprintf(f, "for(%s; %s;", instruction->data._for->init ? "?" : "", "?"); //TODO
			if(instruction->data._for->last) fprintf(f, " %s", "?"); //TODO
			fprintf(f, "){");
			translate_scope(depth + 1, instruction->data._for->body, f);
			indent(depth, f); fprintf(f, "}");

			if(instruction->data._for->_then) {
				indent(depth, f); fprintf(f, "then {");
				translate_scope(depth + 1, instruction->data._for->_then, f);
				indent(depth, f); fprintf(f, "}");
			}
			if(instruction->data._for->_else) {
				indent(depth, f); fprintf(f, "else {");
				translate_scope(depth + 1, instruction->data._for->_else, f);
				indent(depth, f); fprintf(f, "}");
			}

			break;
		}
		case inst_while:  {
			// fprintf(f, s->inst_arr[i].data._while->condition); //TODO
			indent(depth, f); fprintf(f, "while(%s){", "?");
			translate_scope(depth + 1, instruction->data._while->body, f);
			indent(depth, f); fprintf(f, "}");

			if(instruction->data._while->_then) {
				indent(depth, f); fprintf(f, "then {");
				translate_scope(depth + 1, instruction->data._while->_then, f);
				indent(depth, f); fprintf(f, "}");
			}
			if(instruction->data._while->_else) {
				indent(depth, f); fprintf(f, "else {");
				translate_scope(depth + 1, instruction->data._while->_else, f);
				indent(depth, f); fprintf(f, "}");
			}

			break;
		}

		case inst_expr: {
			// fprintf(f, s->inst_arr[i].data.expr); //TODO
			indent(depth, f); fprintf(f, "%s;", "?");
			break;
		}

		case inst_continue: {
			indent(depth, f); fprintf(f, "continue;"); break;
		}
		case inst_break:    {
			indent(depth, f); fprintf(f, "break;");    break;
		}
		case inst_return: {
			// fprintf(f, s->inst_arr[i].data.expr); //TODO
			indent(depth, f); fprintf(f, "return %s;", "?");
			break;
		}
	}
}





void translate_scope(const uint64_t depth, struct Scope* const s, FILE* f){
    // Structs
	for(uint64_t i = 0; i < s->str_num; ++i) {
		indent(depth, f); //TODO move to translate_struct
		fprintf(f, "\n");
		fprintf(f, "struct %s{};", s->str_arr[i].name); //TODO move to translate_struct
	}

    // Variables
	for(uint64_t i = 0; i < s->var_num; ++i) {
		translate_var(depth, &s->var_arr[i], f);
	}

    // Functions
	for(uint64_t i = 0; i < s->fun_num; ++i) {
		fprintf(f, "\n");
		translate_fun(depth, &s->fun_arr[i], f);
	}

	// Instructions
	if(s->parent) for(uint64_t i = 0; i < s->inst_num; ++i){
		translate_instruction(depth, &s->inst_arr[i], f);
	}
}