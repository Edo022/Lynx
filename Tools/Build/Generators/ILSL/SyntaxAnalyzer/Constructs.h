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
	uint64_t str_num;		// The numer of structures declared in the scope
	uint64_t fun_num;		// The numer of functions  declared in the scope
	uint64_t var_num;		// The numer of variables  declared in the scope
	uint64_t scp_num;		// The numer of scopes     declared in the scope
	struct Str* str_arr;		// An array  of structures declared in the scope
	struct Fun* fun_arr;		// An array  of functions  declared in the scope
	struct Var* var_arr;		// An array  of variables  declared in the scope
	struct Scope* scp_arr;	// An array  of scopes     declared in the scope //TODO rename to Scp

	uint64_t            inst_num;	// The number of runtime Instructions
	struct Instruction* inst_arr;	// An array   of runtime Instructions
};

static void init_scope(struct Scope* scope) {
	scope->parent = NULL;
	scope->str_num = 0;
	scope->fun_num = 0;
	scope->var_num = 0;
	scope->scp_num = 0;
	scope->str_arr = NULL;
	scope->fun_arr = NULL;
	scope->var_arr = NULL;
	scope->scp_arr = NULL;

	scope->inst_num = 0;
	scope->inst_arr = NULL;
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
	struct Var* member_arr;		// An array of members
	uint64_t member_num;			// The number of members
};


struct Fun {
	// struct Scope* parent;		// The parent scope of the function //TODO prob useless. use the parent of the own scope
	struct Scope* scope;			// The scope of the function //TODO rename as "body" //FIXME make this a pointer
	enum TokenID type;			// The type of the function
	const char* name;			// The name of the function
	// struct Var* paramv;			// An array of parameters //TODO remove. save as variables in the function body
	uint64_t param_num;			// The number of parameters. The parameters are saved in the first param_num elements of the scope's variable array
};



void scope_add_str(struct Scope* const scope, const struct Str* const str);
void scope_add_fun(struct Scope* const scope, const struct Fun* const fun);
void scope_add_var(struct Scope* const scope, const struct Var* const var);
void scope_add_scp(struct Scope* const scope, const struct Scope* const scp);









//TODO ----------------------------------------------------------------------------------------------
//TODO MOVE TO FlowControl.h


struct ExprElm;
/**
 * @brief An instruction composed of function calls, operators, literals and variables
 */
struct Expr {
	struct ExprElm* elm_arr;
	uint64_t        elm_num;
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
void scope_add_inst_if   (struct Scope* const scope, struct If*    const inst);
void scope_add_inst_while(struct Scope* const scope, struct While* const inst);
void scope_add_inst_for  (struct Scope* const scope, struct For*   const inst);
void scope_add_inst_expr (struct Scope* const scope, struct Expr*  const inst);







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
	struct Scope* true_body;		// Instructions in the if   body
	struct Scope* false_body;	// Instructions in the else body
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



