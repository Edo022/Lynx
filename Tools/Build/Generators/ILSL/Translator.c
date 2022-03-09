#include "Translator.h"
#include "Tokenizer/Types.h"





void translate(struct Scope* const s, FILE* f){
    // Structs
	fprintf(f, "\n");
	for(uint64_t i = 0; i < s->strNum; ++i) {
		fprintf(f, "\nstruct %s{};", s->strArr[i].name);
        //TODO nested structs
	}


    // Variables
	fprintf(f, "\n");
	for(uint64_t i = 0; i < s->varNum; ++i) {
		fprintf(f, "\n%s%s %s;", s->varArr[i].is_const ? "const " : "", typeData[s->varArr[i].type].glslType, s->varArr[i].name);
	}


    // Functions
	fprintf(f, "\n");
	for(uint64_t i = 0; i < s->funNum; ++i) {
		fprintf(f, "\n%s %s(", typeData[s->funArr[i].type].glslType, s->funArr[i].name);
		for(uint64_t j = 0; j < s->funArr[i].paramNum; ++j){
			// strcat(outputStr, );
		}
		fprintf(f, ") {");
        translate(&s->funArr[i].scope, f);
		fprintf(f, "\n}");
	}
}