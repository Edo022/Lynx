#pragma once
#include "Tokenizer/Tokens.h"
#include "Tokenizer/Operators.h"



//TODO use Expr alias for OpNode*


struct Var;
struct Fun;
struct Str;
struct OpNode;

struct Scope {
	struct Scope* parent;	// The parent scope of the scope
	uint64_t strNum;		// The numer of structures declared in the scope
	uint64_t funNum;		// The numer of functions  declared in the scope
	uint64_t varNum;		// The numer of variables  declared in the scope
	uint64_t scpNum;		// The numer of scopes     declared in the scope
	struct Str* strArr;		// An array  of structures declared in the scope
	struct Fun* funArr;		// An array  of functions  declared in the scope
	struct Var* varArr;		// An array  of variables  declared in the scope
	struct Scope* scpArr;	// An array  of scopes     declared in the scope //TODO rename to Scp

	uint64_t        expNum;	// The number of runtime instructions
	struct OpNode** expArr;	// An array   of runtime instructions
};

static void initScope(struct Scope* pScope) {
	pScope->parent = NULL;
	pScope->strNum = 0;
	pScope->funNum = 0;
	pScope->varNum = 0;
	pScope->scpNum = 0;
	pScope->strArr = NULL;
	pScope->funArr = NULL;
	pScope->varArr = NULL;
	pScope->scpArr = NULL;

	pScope->expNum = 0;
	pScope->expArr = NULL;
}




struct Var {
	struct Scope* parent;		// The parent scope of the variable
	enum TokenID type;			// THe type of the variable
	bool is_const;				// True if the function has const type, false otherwise
	const char* name;			// The name of the function
};

struct Str {
	struct Scope* parent;		// The parent scope of the struct
	const char* name;			// The name of the struct
	struct Var* memberArr;		// An array of members
	uint64_t memberNum;			// The number of members
};


struct Fun {
	struct Scope* parent;		// The parent scope of the function
	enum TokenID type;			// The type of the function
	const char* name;			// The name of the function
	// struct Var* paramv;			// An array of parameters //TODO remove. save as variables in the function body
	uint64_t paramNum;			// The number of parameters. The parameters are saved in the first paramNum elements of the scope's variable array
	struct Scope scope;			// The scope of the function //TODO rename as "body" //FIXME make this a pointer
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
static void addScp(struct Scope* const pScope, const struct Scope* const vScope){ //TODO make not static
	pScope->scpArr = reallocPow2(pScope->scpArr, sizeof(struct Scope), pScope->scpNum);
	pScope->scpArr[pScope->scpNum++] = *vScope;
}








//TODO ----------------------------------------------------------------------------------------------
//TODO MOVE TO FlowControl.h

enum InstructionType {
	inst_if,		// If-else statement.                      data Points to a struct If
	inst_for,		// For loop statement.                     data Points to a struct For
	inst_while,		// While loop statement.                   data Points to a struct While
	inst_expr,		// An expression whose value is discarded. data Points to the root node of a struct Op tree
	inst_continue,	// Continue statement.                     data is NULL
	inst_break,		// Break statement.                        data is NULL
	inst_return		// Return statement.                       data Points to the root node of a struct Op tree
};

struct Instruction {
	enum InstructionType type;
	void* data;
};




enum OperandType {
	operand_op,			// Another operator.    The operand points to a struct OpNode
	operand_call,		// A function call.     The operand points to a struct Call
	operand_literal,	// A numeric literal.   The operand points to a struct LiteralData_t (from Tokenizer)
};



//TODO manage ternary operators
struct OpNode {
	enum TokenID id; 				// The ID of the operator. Redundant
	struct OperatorData_t data;		// The properties of the operator
	enum OperandType operandType_0;	// The type of the first  operand
	enum OperandType operandType_1;	// The type of the second operand
	void* operand_0;				// The address of the struct representing the first  operand
	void* operand_1;				// The address of the struct representing the second operand. NULL in unary operators
};


struct Call {
	uint64_t argNum;		// The number of arguments. //! This might be incorrect. Checked by the semantic analyzer
	struct Tree** argArr;	// An array of operator trees that are used as arguments
};






struct If {
	struct OpNode* condition;	// The condition to check as an operator tree
	struct Scope* trueBody;		// Instructions in the if   body
	struct Scope* falseBody;	// Instructions in the else body
	//! elif constructs are saved as a series of nested if-else
};

//TODO add switch

struct While{
	struct OpNode* condition;	// The condition to check as an operator tree
	struct Scope* body;			// The instructions in the loop body
}; //TODO add while - else


struct For {
	struct OpNode* init; //TODO idk, prob useless
	struct OpNode* condition;	// The condition to check as an operator tree
	struct OpNode* inc; //TODO same
	struct Scope* body;			// The instructions in the loop body
};



