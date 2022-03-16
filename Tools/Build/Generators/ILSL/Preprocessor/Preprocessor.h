#pragma once
#include <inttypes.h>
#include <stdbool.h>
#include "Files.h"



char* uncomment(const char* code, struct File* const file_info);

bool check_included_file(const struct Line line_info, const char* abs_path);
char* is_include(const struct Line line_info);
uint64_t include_file(const char* const abs_path, const uint64_t parent_line, struct File* parent_file, struct Line** const out_lines);