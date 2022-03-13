#include "SyntaxAnalyzer/Constructs.h"
#include "Utils.h"



//FIXME rename as scope_new_something
//FIXME make these functions return the new element

void scope_add_str(struct Scope* const scope, const struct Str* const str){ //TODO remove if not used
	scope->str_arr = relloc_pow2(scope->str_arr, sizeof(struct Str), scope->str_num);
	scope->str_arr[scope->str_num++] = *str;
}
void scope_add_fun(struct Scope* const scope, const struct Fun* const fun){ //TODO remove if not used
	scope->fun_arr = relloc_pow2(scope->fun_arr, sizeof(struct Fun), scope->fun_num);
	scope->fun_arr[scope->fun_num++] = *fun;
}
void scope_add_var(struct Scope* const scope, const struct Var* const var){ //TODO remove if not used
	scope->var_arr = relloc_pow2(scope->var_arr, sizeof(struct Var), scope->var_num);
	scope->var_arr[scope->var_num++] = *var;
}
void scope_add_scp(struct Scope* const scope, const struct Scope* const scp){ //TODO remove if not used
	scope->scp_arr = relloc_pow2(scope->scp_arr, sizeof(struct Scope), scope->scp_num);
	scope->scp_arr[scope->scp_num++] = *scp;
}





//FIXME these ones are ok

struct If* scope_new_statement_if(struct Scope* const scope){
	scope->inst_arr = relloc_pow2(scope->inst_arr, sizeof(struct Instruction), scope->inst_num);
	scope->inst_arr[scope->inst_num].type = inst_if;
	scope->inst_num++;
	return scope->inst_arr[scope->inst_num].data._if;
}
struct While* scope_new_statement_while(struct Scope* const scope){
	scope->inst_arr = relloc_pow2(scope->inst_arr, sizeof(struct Instruction), scope->inst_num);
	scope->inst_arr[scope->inst_num].type = inst_while;
	scope->inst_num++;
	return scope->inst_arr[scope->inst_num].data._while;
}
struct For* scope_new_statement_for(struct Scope* const scope){
	scope->inst_arr = relloc_pow2(scope->inst_arr, sizeof(struct Instruction), scope->inst_num);
	scope->inst_arr[scope->inst_num].type = inst_for;
	scope->inst_num++;
	return scope->inst_arr[scope->inst_num].data._for;
}
struct Expr* scope_new_statement_expr(struct Scope* const scope){
	scope->inst_arr = relloc_pow2(scope->inst_arr, sizeof(struct Instruction), scope->inst_num);
	scope->inst_arr[scope->inst_num].type = inst_expr;
	scope->inst_num++;
	return scope->inst_arr[scope->inst_num].data.expr;
}
