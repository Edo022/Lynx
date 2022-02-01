#pragma once
#define _DEFAULT_SOURCE // Required for non standard functions
#include "Files.h"





#define MAX_ERR         4100100
#define MAX_CODE_LEN    800100100	//FIXME use dynamic reallocations
#define MAX_CODE_LINES  4100100		//FIXME use dynamic reallocations or pass the size
#define MAX_TOKENS      8100100		//FIXME use dynamic reallocations



static const char *nBlk = "\033[0;30m", *bBlk = "\033[1;30m", *uBlk = "\033[4;30m";
static const char *nRed = "\033[0;31m", *bRed = "\033[1;31m", *uRed = "\033[4;31m";
static const char *nGrn = "\033[0;32m", *bGrn = "\033[1;32m", *uGrn = "\033[4;32m";
static const char *nYel = "\033[0;33m", *bYel = "\033[1;33m", *uYel = "\033[4;33m";
static const char *nBlu = "\033[0;34m", *bBlu = "\033[1;34m", *uBlu = "\033[4;34m";
static const char *nMag = "\033[0;35m", *bMag = "\033[1;35m", *uMag = "\033[4;35m";
static const char *nCyn = "\033[0;36m", *bCyn = "\033[1;36m", *uCyn = "\033[4;36m";
static const char *nWht = "\033[0;37m", *bWht = "\033[1;37m", *uWht = "\033[4;37m";



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
	return ret * !!value;
}

// Bit scanf forward //TODO add to Lynx Engine
// Returns 0 if value is 0
static inline uint64_t bsfz(uint64_t value){
	uint64_t ret;
	__asm__("bsf %1, %0" : "=r"(ret) : "r"(value));
	return ret * !!value;
}


uint64_t countChar(const char* const vLine, const char vChar);
void* reallocPow2(void* pAlloc, uint64_t vSize, uint64_t vOldNum);
double bstrtolf(const char* vStr, const int32_t vBase);
char* readFile(const char* vFilePath, uint64_t vTabSize);

const char* strchrn(const char* vSrc, const char vChar, const uint32_t vIndex);
char* strtokn(const char* vSrc, const char vChar, const uint32_t vIndex);

void printError(const char* vFormat, ...);
void printSyntaxError(const struct Line iLineInfo, const char* const vFormat, ...);
//TODO add a printsyntaxerror that takes Token* instead of a Line or replace this one