//TODO change output name
// > from Lynx/Tools/Build/Generators/ILSL
//exec          gcc ILSL.c Utils.c Preprocessor/Preprocessor.c SyntaxAnalyzer/SyntaxAnalyzer.c Tokenizer/Tokenizer.c Preprocessor/Files.c -I. -std=c11 -o ../GenGlsl
//exec          gcc ILSL.c Utils.c Preprocessor/Preprocessor.c SyntaxAnalyzer/SyntaxAnalyzer.c Tokenizer/Tokenizer.c Preprocessor/Files.c -I. -std=c11 -ggdb3 -g3 -O0 -o ../GenGlsl
// > from Lynx
//run           Tools/Build/Generators/GenGlsl src/Lynx/shaders/Volume.ilsl ../.engine/.tmp/glsl-Volume.ilsl.comp

// //Filter includes
// // cd Lynx/Tools/Build/Generators/ILSL; make CMAKE_C_FLAGS="-Werror"
// // doesnt work



//TODO MOVE TOOLS SOURCE FILES TO /src/Tools


//FIXME USE NULL TERMINATING TOKEN INSTEAD OF PASSING THE SIZE IN EVERY FUNCTION

#include "Utils.h"

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "Tokenizer/Operators.h"
#include "Tokenizer/Types.h"
#include "Tokenizer/Keywords.h"
#include "Tokenizer/Literals.h"

#include "Preprocessor/Preprocessor.h"
#include "Tokenizer/Tokenizer.h"
#include "SyntaxAnalyzer/SyntaxAnalyzer.h"
#include "Translator.h"




//FIXME GLSL doesnt support bool and integer matrices
//TODO check returns values
//TODO add macros
//TODO add include lists













//TODO add token infos to printSyntaxError
//TODO add PrintSemanticError







// Main -------------------------------------------------------------------------------------------------------------------------------------//








void run(const char* const src_name, const char* const out_name){
	source_files_arr = malloc(sizeof(struct File) * 2);

	//Read input file
	const char* const src = realpath(src_name, NULL); //Resolve symbolic links
	if(access(src, F_OK) != 0) print_generic_error("\"%s\": No such file or directory", src_name);


	// Process the code
	struct Line*  out_lines;  include_file(src_name, UINT64_MAX, NULL,             &out_lines);		//Add hard coded version statement and parse the code
	struct Token* out_tokens; tokenize(out_lines, src_name,                        &out_tokens);	// Tokenize the code
	struct Scope* out_scope;  build_scope_syntax_tree(NULL, out_tokens, out_lines, &out_scope);		// Check the syntax and write the GLSL code


	//Write output file
	FILE* ofile = fopen(out_name, "w");
	fprintf(ofile, "#version 450"); //TODO USE 460
	translate_scope(out_scope, ofile);
}






int32_t main(int32_t argc, char* argv[]){
	if(argc != 3){
		printf("GenGlsl: Wrong number of arguments");
		return 1;
	}

	run(argv[1], argv[2]);
	printf(argv[2]);
	return 0;
}