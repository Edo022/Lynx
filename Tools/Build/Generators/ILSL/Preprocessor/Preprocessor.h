#pragma once
#include <inttypes.h>
#include "Files.h"



void clear(struct Line* vLines);
char* uncomment(const char* vCode, struct File* const iFile);

void checkIncludeFile(const struct Line iLineInfo, const char* vPath);
char* isInclude(const struct Line iLineInfo);
// struct Line* include(const char* const vFile, const uint64_t vFromLine, struct File* vFromFile);
uint64_t include(const char* const vFile, const uint64_t vFromLine, struct File* vFromFile, struct Line** const pLines);