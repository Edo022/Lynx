#include "SyntaxAnalyzer/Constructs.h"
#include "Utils.h"




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






void scope_add_inst_if(struct Scope* const scope, struct If* const inst){ //TODO remove if not used
	scope->inst_arr = relloc_pow2(scope->inst_arr, sizeof(struct Instruction), scope->inst_num);
	scope->inst_arr[scope->inst_num].type = inst_if;
	scope->inst_arr[scope->inst_num].data._if = inst;
	scope->inst_num++;
}
void scope_add_inst_while(struct Scope* const scope, struct While* const inst){ //TODO remove if not used
	scope->inst_arr = relloc_pow2(scope->inst_arr, sizeof(struct Instruction), scope->inst_num);
	scope->inst_arr[scope->inst_num].type = inst_while;
	scope->inst_arr[scope->inst_num].data._while = inst;
	scope->inst_num++;
}
void scope_add_inst_for(struct Scope* const scope, struct For* const inst){ //TODO remove if not used
	scope->inst_arr = relloc_pow2(scope->inst_arr, sizeof(struct Instruction), scope->inst_num);
	scope->inst_arr[scope->inst_num].type = inst_for;
	scope->inst_arr[scope->inst_num].data._for = inst;
	scope->inst_num++;
}
void scope_add_inst_expr(struct Scope* const scope, struct Expr* const inst){ //TODO remove if not used
	scope->inst_arr = relloc_pow2(scope->inst_arr, sizeof(struct Instruction), scope->inst_num);
	scope->inst_arr[scope->inst_num].type = inst_expr;
	scope->inst_arr[scope->inst_num].data.expr = inst;
	scope->inst_num++;
}
