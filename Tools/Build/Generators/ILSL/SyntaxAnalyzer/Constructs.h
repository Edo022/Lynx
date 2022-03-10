#pragma once
#include <stdbool.h>
#include "Tokenizer/Tokens.h"
#include "Tokenizer/Operators.h"



//TODO use Expr alias for OpNode*


struct Var;
struct Fun;
struct Str;
struct Instruction;

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

	uint64_t            instructionNum;	// The number of runtime Instructions
	struct Instruction* instructionArr;	// An array   of runtime Instructions
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

	pScope->instructionNum = 0;
	pScope->instructionArr = NULL;
}



struct Expr;
struct Var {
	struct Scope* parent;		// The parent scope of the variable
	enum TokenID type;			// THe type of the variable
	bool is_const;				// True if the function has const type, false otherwise
	const char* name;			// The name of the function
	struct Expr* init;			// Initializer value
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



void addStr(struct Scope* const pScope, const struct Str* const vStr);
void addFun(struct Scope* const pScope, const struct Fun* const vFun);
void addVar(struct Scope* const pScope, const struct Var* const vVar);
void addScp(struct Scope* const pScope, const struct Scope* const vScope);









//TODO ----------------------------------------------------------------------------------------------
//TODO MOVE TO FlowControl.h


struct ExprElm;
/**
 * @brief An instruction composed of function calls, operators, literals and variables
 */
struct Expr {
	struct ExprElm* elmArr;
	uint64_t        elmNum;
};


/**
 * @brief The type of a runtime instruction
 *     This defines the active value of its data member
 */
enum InstructionType {
	inst_if,		// If-else statement.                      Active data value: _if;
	inst_for,		// For loop statement.                     Active data value: _for;
	inst_while,		// While loop statement.                   Active data value: _while;

	inst_expr,		// An expression whose value is discarded. Active data value: expr;

	inst_continue,	// Continue statement.
	inst_break,		// Break statement.
	inst_return		// Return statement.                       Active data value: expr;
};

/**
 * @brief A runtime instruction
 *     This can be an expression or a flow control construct or directive
 */
struct Instruction {
	enum InstructionType type;
	union {
		struct If*    _if;
		struct For*   _for;
		struct While* _while;
		struct Expr*  expr;
	} data;
};
void addInstructionIf(struct Scope* const pScope, struct If* const vInstruction);
void addInstructionWhile(struct Scope* const pScope, struct While* const vInstruction);
void addInstructionFor(struct Scope* const pScope, struct For* const vInstruction);
void addInstructionExpr(struct Scope* const pScope, struct Expr* const vInstruction);







/**
 * @brief The type of an ExprElm struct
 *     This defines the active value of its data member
 */
enum ExprElmType {
	expr_operator,	// Operators.                          Active data value: operator
	expr_literal,	// Literal constants.                  Active data value: literal
	expr_variable,	// Variables. data contains its name   Active data value: name           Active data value: name
	expr_function	// Function calls. data contains its name. The subsequent elements are a literal representing the number of arguments and the list of arguments
};

/**
 * @brief An element of an expression
 */
struct ExprElm {
	enum ExprElmType type;
	union {
		struct OperatorData_t* operator;
		struct LiteralData_t*  literal;
		const char*            name;
	} data;
};




/**
 * @brief If flow control construct node
 */
struct If {
	struct Expr*  condition;	// The condition to check as an operator tree
	struct Scope* trueBody;		// Instructions in the if   body
	struct Scope* falseBody;	// Instructions in the else body
	//! elif constructs are saved as a series of nested if-else
};

//TODO add switch

/**
 * @brief While flow control construct node
 */
struct While{
	struct Expr*  condition;	// The condition to check as an operator tree
	struct Scope* body;			// The instructions in the loop body
}; //TODO add while - else


/**
 * @brief For flow control construct node
 */
struct For {
	struct Expr*  init; //TODO idk, prob useless
	struct Expr*  condition;	// The condition to check as an operator tree
	struct Expr*  inc; //TODO same
	struct Scope* body;			// The instructions in the loop body
};



