#pragma once
#define _DEFAULT_SOURCE // Required for non standard functions
#include "Preprocessor/Files.h"





#define MAX_ERR         4100100
#define MAX_CODE_LEN    800100100	//FIXME use dynamic reallocations
#define MAX_CODE_LINES  4100100		//FIXME use dynamic reallocations or pass the size
#define MAX_TOKENS      8100100		//FIXME use dynamic reallocations


//TODO use defines in lux engine
#define col_nblk "\033[0;30m"
#define col_nred "\033[0;31m"
#define col_ngrn "\033[0;32m"
#define col_nyel "\033[0;33m"
#define col_nblu "\033[0;34m"
#define col_nmag "\033[0;35m"
#define col_ncyn "\033[0;36m"
#define col_nwht "\033[0;37m"

#define col_bblk "\033[1;30m"
#define col_bred "\033[1;31m"
#define col_bgrn "\033[1;32m"
#define col_byel "\033[1;33m"
#define col_bblu "\033[1;34m"
#define col_bmag "\033[1;35m"
#define col_bcyn "\033[1;36m"
#define col_bwht "\033[1;37m"

#define col_ublk "\033[4;30m";
#define col_ured "\033[4;31m";
#define col_ugrn "\033[4;32m";
#define col_uyel "\033[4;33m";
#define col_ublu "\033[4;34m";
#define col_umag "\033[4;35m";
#define col_ucyn "\033[4;36m";
#define col_uwht "\033[4;37m";



// Bit scan reverse //TODO add to Lynx Engine
// value cannot be 0
static inline uint64_t bsr(uint64_t value){
	uint64_t ret;
	__asm__("bsr %1, %0" : "=r"(ret) : "r"(value));
	return ret;
}

// Bit scanf forward //TODO add to Lynx Engine
// value cannot be 0
static inline uint64_t bsf(uint64_t value){
	uint64_t ret;
	__asm__("bsf %1, %0" : "=r"(ret) : "r"(value));
	return ret;
}

// Bit scan reverse //TODO add to Lynx Engine
// Returns 0 if value is 0
static inline uint64_t bsrz(uint64_t value){
	uint64_t ret;
	__asm__("bsr %1, %0" : "=r"(ret) : "r"(value));
	return value ? ret : 0;
}

// Bit scanf forward //TODO add to Lynx Engine
// Returns 0 if value is 0
static inline uint64_t bsfz(uint64_t value){
	uint64_t ret;
	__asm__("bsf %1, %0" : "=r"(ret) : "r"(value));
	return value ? ret : 0;
}


uint64_t count_chars(const char* const str, const char chr);
void*    relloc_pow2(void* ptr, uint64_t size, uint64_t cur_num);
double   bstrtolf(const char* str, const int32_t base);
char*    read_sanitized_file(const char* path, uint64_t tab_size);

const char* strchrn(const char* str, const char chr, const uint32_t index);
char*       strtokn(const char* str, const char chr, const uint32_t index);

void print_generic_error(const char* format, ...);
void print_syntax_error(const struct Line line_info, const char* const format, ...);
//TODO add a printsyntaxerror that takes Token* instead of a Line or replace this one