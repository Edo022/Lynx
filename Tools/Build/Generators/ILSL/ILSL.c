//TODO change output name
// > from Lynx/Tools/Build/Generators/ILSL
//exec          gcc ILSL.c Utils.c Preprocessor/Preprocessor.c SyntaxAnalyzer/SyntaxAnalyzer.c Tokenizer/Tokenizer.c Preprocessor/Files.c -I. -std=c11 -o ../GenGlsl
//exec          gcc ILSL.c Utils.c Preprocessor/Preprocessor.c SyntaxAnalyzer/SyntaxAnalyzer.c Tokenizer/Tokenizer.c Preprocessor/Files.c -I. -std=c11 -ggdb3 -g3 -O0 -o ../GenGlsl
// > from Lynx
//run           Tools/Build/Generators/GenGlsl src/Lynx/shaders/Volume.ilsl ../.engine/.tmp/glsl-Volume.ilsl.comp

//Filter includes
// cd Lynx/Tools/Build/Generators/ILSL; gcc -Werror ILSL.c Utils.c  Preprocessor/Preprocessor.c SyntaxAnalyzer/SyntaxAnalyzer.c Tokenizer/Tokenizer.c Preprocessor/Files.c -I. -std=c11 -ggdb3 -g3 -O0 -o ../GenGlsl



//TODO MOVE TOOLS SOURCE FILES TO /src/Tools


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




//FIXME GLSL doesnt support bool and integer matrices
//TODO check returns values
//TODO add macros
//TODO add include lists













//TODO add token infos to printSyntaxError
//TODO add PrintSemanticError










// Output assembler -------------------------------------------------------------------------------------------------------------------------//








char* translate(const struct Token* vTokens, const uint64_t vTokensNum){
	char* const ret = malloc(MAX_CODE_LEN);
	uint64_t j = 0;
	for(uint64_t i = 0; i < vTokensNum; ++i){
		const struct Token* const curTok = vTokens + i;
		if(curTok->leading_ws){
			strcpy(ret + j, curTok->leading_ws);
			j += strlen(curTok->leading_ws);
		}

		if(curTok->id == e_literal){
			struct LiteralData_t* const tokData = curTok->data;
			char strValue[64];
			if(tokData->type == t_u32) snprintf(strValue, 64, "%d", *(uint32_t*)tokData->value);
			else                       snprintf(strValue, 64, "%lf",  *(double*)tokData->value);
			strcpy(ret + j, strValue);
			j += strlen(strValue);
		}
		else if(isType(curTok->id)){
			strcpy(ret + j, ((struct TypeData_t*)(curTok->data))->glslType);
			j +=     strlen(((struct TypeData_t*)(curTok->data))->glslType);
		}
		else if(isKeyword(curTok->id)){
			strcpy(ret + j, keywordValues[curTok->id - k_start]);
			j +=     strlen(keywordValues[curTok->id - k_start]);
		}
		else if(isOperator(curTok->id)){
			strcpy(ret + j, operatorValues[curTok->id - o_start]);
			j +=     strlen(operatorValues[curTok->id - o_start]);
		}
		else{
			strcpy(ret + j, curTok->value);
			j += curTok->len;
		}
	}
	ret[j + 1] = '\0';
	return ret;
}








// Main -------------------------------------------------------------------------------------------------------------------------------------//








void run(const char* const vSrc, const char* const vOut){
	files = malloc(sizeof(struct File) * 2);

	//Read input file
	const char* const src = realpath(vSrc, NULL); //Resolve symbolic links
	if(access(src, F_OK) != 0) printError("\"%s\": No such file or directory", vSrc);

	//Add hard coded version statement and parse the code
	uint64_t outputLinesNum;
	struct Line* const outputLines = include(vSrc, UINT64_MAX, NULL, &outputLinesNum);
	clear(outputLines, outputLinesNum);

	// Tokenize the code
	uint64_t outputTokensNum;
	struct Token* const outputTokens = tokenize(outputLines, outputLinesNum, &outputTokensNum, vSrc);

	// Check the syntax and write the GLSL code
	struct Scope* scope = buildSyntaxTree(outputTokens, outputTokensNum, outputLines);
	// char* const outputStr = translate(outputTokens, outputTokensNum);

	//TODO REMOVE or something, idk
	char* const outputStr = malloc(MAX_CODE_LEN); outputStr[0] = '\0';
	for(uint64_t i = 0; i < scope->varNum; ++i) strcat(outputStr, scope->varArr[i].name); strcat(outputStr, "\n");
	for(uint64_t i = 0; i < scope->funNum; ++i) strcat(outputStr, scope->funArr[i].name); strcat(outputStr, "\n");
	for(uint64_t i = 0; i < scope->strNum; ++i) strcat(outputStr, scope->strArr[i].name); strcat(outputStr, "\n");
	//TODO REMOVE


	//Write output file
	FILE* ofile = fopen(vOut, "w");
	fprintf(ofile, "#version 450\n");
	fprintf(ofile, "%s", outputStr);
	fclose(ofile);
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