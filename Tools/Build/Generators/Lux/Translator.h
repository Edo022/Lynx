#pragma once
#include <stdio.h>
#include "SyntaxAnalyzer/Constructs.h"


void translate_scope(const uint64_t depth, struct Scope* const s, FILE* f);