#pragma once
#include <stdio.h>
#include "SyntaxAnalyzer/Constructs.h"


void translate_scope(struct Scope* const s, FILE* f);