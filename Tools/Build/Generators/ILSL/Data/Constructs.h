#pragma once
#include "Tokenizer/Tokens.h"




struct Var;
struct Fun;
struct Str;

struct Scope{
	struct Scope* parent;	// The parent scope of the scope
	uint64_t strNum;		// The numer of structures declared in the scope
	uint64_t funNum;		// The numer of functions declared in the scope
	uint64_t varNum;		// The numer of variables declared in the scope
	struct Str* strArr;		// An array  of structures declared in the scope
	struct Fun* funArr;		// An array  of functions declared in the scope
	struct Var* varArr;		// An array  of variables declared in the scope
};
static void initScope(struct Scope* pScope){
	pScope->parent = NULL;
	pScope->strNum = 0;
	pScope->funNum = 0;
	pScope->varNum = 0;
	pScope->strArr = NULL;
	pScope->funArr = NULL;
	pScope->varArr = NULL;
}


struct Var{
	struct Scope* parent;		// The parent scope of the variable
	enum TokenID type;			// THe type of the variable
	bool is_const;				// True if the function has const type, false otherwise
	const char* name;			// The name of the function
};

struct Str{
	struct Scope* parent;		// The parent scope of the struct
	const char* name;			// The name of the struct
	struct Var* memberArr;		// An array of members
	uint64_t memberNum;			// The number of members
};


struct Fun{
	struct Scope* parent;		// The parent scope of the function
	enum TokenID type;			// The type of the function
	const char* name;			// The name of the function
	// struct Var* paramv;			// An array of parameters //TODO same as the variables in the function's scope
	uint64_t paramNum;			// The number of parameters. The parameters are saved in the first paramNum elements of the scope's variable array
	struct Scope scope;			// The scope of the function
	struct Token* exec; 		//runtime lines as a list of tokens //FIXME write Instruction struct and use an array of struct Instruction
};



static void addStr(struct Scope* const pScope, const struct Str* const vStr){ //TODO make not static
	pScope->strArr = reallocPow2(pScope->strArr, sizeof(struct Str), pScope->strNum);
	pScope->strArr[pScope->strNum++] = *vStr;
}
static void addFun(struct Scope* const pScope, const struct Fun* const vFun){ //TODO make not static
	pScope->funArr = reallocPow2(pScope->funArr, sizeof(struct Fun), pScope->funNum);
	pScope->funArr[pScope->funNum++] = *vFun;
}
static void addVar(struct Scope* const pScope, const struct Var* const vVar){ //TODO make not static
	pScope->varArr = reallocPow2(pScope->varArr, sizeof(struct Var), pScope->varNum);
	pScope->varArr[pScope->varNum++] = *vVar;
}
