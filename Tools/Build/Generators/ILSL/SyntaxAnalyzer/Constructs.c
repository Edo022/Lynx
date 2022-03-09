#include "SyntaxAnalyzer/Constructs.h"
#include "Utils.h"




void addStr(struct Scope* const pScope, const struct Str* const vStr){ //TODO remove if not used
	pScope->strArr = reallocPow2(pScope->strArr, sizeof(struct Str), pScope->strNum);
	pScope->strArr[pScope->strNum++] = *vStr;
}
void addFun(struct Scope* const pScope, const struct Fun* const vFun){ //TODO remove if not used
	pScope->funArr = reallocPow2(pScope->funArr, sizeof(struct Fun), pScope->funNum);
	pScope->funArr[pScope->funNum++] = *vFun;
}
void addVar(struct Scope* const pScope, const struct Var* const vVar){ //TODO remove if not used
	pScope->varArr = reallocPow2(pScope->varArr, sizeof(struct Var), pScope->varNum);
	pScope->varArr[pScope->varNum++] = *vVar;
}
void addScp(struct Scope* const pScope, const struct Scope* const vScope){ //TODO remove if not used
	pScope->scpArr = reallocPow2(pScope->scpArr, sizeof(struct Scope), pScope->scpNum);
	pScope->scpArr[pScope->scpNum++] = *vScope;
}








void addInstructionIf(struct Scope* const pScope, struct If* const vInstruction){ //TODO remove if not used
	pScope->instructionArr = reallocPow2(pScope->instructionArr, sizeof(struct Instruction), pScope->instructionNum);
	pScope->instructionArr[pScope->instructionNum].type = inst_if;
	pScope->instructionArr[pScope->instructionNum].data._if = vInstruction;
	pScope->instructionNum++;
}
void addInstructionWhile(struct Scope* const pScope, struct While* const vInstruction){ //TODO remove if not used
	pScope->instructionArr = reallocPow2(pScope->instructionArr, sizeof(struct Instruction), pScope->instructionNum);
	pScope->instructionArr[pScope->instructionNum].type = inst_while;
	pScope->instructionArr[pScope->instructionNum].data._while = vInstruction;
	pScope->instructionNum++;
}
void addInstructionFor(struct Scope* const pScope, struct For* const vInstruction){ //TODO remove if not used
	pScope->instructionArr = reallocPow2(pScope->instructionArr, sizeof(struct Instruction), pScope->instructionNum);
	pScope->instructionArr[pScope->instructionNum].type = inst_for;
	pScope->instructionArr[pScope->instructionNum].data._for = vInstruction;
	pScope->instructionNum++;
}
void addInstructionExpr(struct Scope* const pScope, struct Expr* const vInstruction){ //TODO remove if not used
	pScope->instructionArr = reallocPow2(pScope->instructionArr, sizeof(struct Instruction), pScope->instructionNum);
	pScope->instructionArr[pScope->instructionNum].type = inst_expr;
	pScope->instructionArr[pScope->instructionNum].data.expr = vInstruction;
	pScope->instructionNum++;
}
