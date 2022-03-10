#include "Translator.h"
#include "Tokenizer/Types.h"




static void translate_var(struct Var* const var, FILE* const f){
	if(var->is_const) fprintf(f, "const ");
	fprintf(f, "\n%s %s", typeData[var->type].glslType, var->name);
	if(var->init) fprintf(f, " = %s", "\"Unknown expression\"");
	fprintf(f, ";");
}




static void translate_fun(struct Fun* const fun, FILE* const f){
	fprintf(f, "\n%s %s(", typeData[fun->type].glslType, fun->name);
	for(uint64_t j = 0; j < fun->paramNum; ++j){
		// strcat(outputStr, );
	}
	fprintf(f, "\"Unknown argument list\""); //TODO actually read the arguments
	fprintf(f, ") {");
	translate_scope(fun->scope, f);
	fprintf(f, "\n}");
}




static void translate_instruction(struct Instruction* const instruction, FILE* const f){
	switch(instruction->type){
		case inst_if: {
			// fprintf(f, s->instructionArr[i].data._if->condition); //TODO
			// fprintf(f, s->instructionArr[i].data._if->falseBody); //TODO
			// fprintf(f, s->instructionArr[i].data._if->trueBody); //TODO
			fprintf(f, "if(%s){ %s }", "\"Unknown condition\"", "\"Unknown expression\"");
			break;
		}
		case inst_for:  {
			// fprintf(f, s->instructionArr[i].data._for->init); //TODO
			// fprintf(f, s->instructionArr[i].data._for->inc); //TODO
			// fprintf(f, s->instructionArr[i].data._for->condition); //TODO
			// fprintf(f, s->instructionArr[i].data._for->body); //TODO
			fprintf(f, "for(%s;%s;%s){ %s }", "\"Unknown expression\"", "\"Unknown condition\"", "\"Unknown expression\"", "\"Unknown expression\"");
			break;
		}
		case inst_while:  {
			// fprintf(f, s->instructionArr[i].data._while->condition); //TODO
			// fprintf(f, s->instructionArr[i].data._while->body); //TODO
			fprintf(f, "while(%s){ %s }", "\"Unknown condition\"", "\"Unknown expression\"");
			break;
		}

		case inst_expr: {
			// fprintf(f, s->instructionArr[i].data.expr); //TODO
			fprintf(f, "%s;", "\"Unknown expression\"");
			break;
		}

		case inst_continue: fprintf(f, "continue;"); break;
		case inst_break:    fprintf(f, "break;");    break;
		case inst_return: {
			// fprintf(f, s->instructionArr[i].data.expr); //TODO
			fprintf(f, "return %s;", "\"Unknown expression\"");
			break;
		}
	}
}





void translate_scope(struct Scope* const s, FILE* f){
    // Structs
	fprintf(f, "\n");
	for(uint64_t i = 0; i < s->strNum; ++i) {
		fprintf(f, "\nstruct %s{};", s->strArr[i].name);
        //TODO nested structs
	}

    // Variables
	fprintf(f, "\n");
	for(uint64_t i = 0; i < s->varNum; ++i) {
		translate_var(&s->varArr[i], f);
	}

    // Functions
	fprintf(f, "\n");
	for(uint64_t i = 0; i < s->funNum; ++i) {
		translate_fun(&s->funArr[i], f);
	}

	// Instructions
	if(s->parent) for(uint64_t i = 0; i < s->instructionNum; ++i){
		fprintf(f, "\n");
		translate_instruction(&s->instructionArr[i], f);
	}
}