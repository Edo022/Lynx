#pragma once
#include <inttypes.h>
#include <stdio.h>




const char *nBlk = "\033[0;30m", *bBlk = "\033[1;30m", *uBlk = "\033[4;30m";
const char *nRed = "\033[0;31m", *bRed = "\033[1;31m", *uRed = "\033[4;31m";
const char *nGrn = "\033[0;32m", *bGrn = "\033[1;32m", *uGrn = "\033[4;32m";
const char *nYel = "\033[0;33m", *bYel = "\033[1;33m", *uYel = "\033[4;33m";
const char *nBlu = "\033[0;34m", *bBlu = "\033[1;34m", *uBlu = "\033[4;34m";
const char *nMag = "\033[0;35m", *bMag = "\033[1;35m", *uMag = "\033[4;35m";
const char *nCyn = "\033[0;36m", *bCyn = "\033[1;36m", *uCyn = "\033[4;36m";
const char *nWht = "\033[0;37m", *bWht = "\033[1;37m", *uWht = "\033[4;37m";



// Bit scan reverse //TODO add to Lynx Engine
// value cannot be 0
uint64_t bsr(uint64_t value){
	uint64_t ret;
	__asm__("bsr %1, %0" : "=r"(ret) : "r"(value));
	return ret;
}

// Bit scanf forward //TODO add to Lynx Engine
// value cannot be 0
uint64_t bsf(uint64_t value){
	uint64_t ret;
	__asm__("bsf %1, %0" : "=r"(ret) : "r"(value));
	return ret;
}

// Bit scan reverse //TODO add to Lynx Engine
// Returns 0 if value is 0
uint64_t bsrz(uint64_t value){
	uint64_t ret;
	__asm__("bsr %1, %0" : "=r"(ret) : "r"(value));
	return ret * !!value;
}

// Bit scanf forward //TODO add to Lynx Engine
// Returns 0 if value is 0
uint64_t bsfz(uint64_t value){
	uint64_t ret;
	__asm__("bsf %1, %0" : "=r"(ret) : "r"(value));
	return ret * !!value;
}




/**
 * @brief Reallocates pAlloc to (vSize) * (the closest power of 2 that can contain (vOldNum + 1) elements)
 *     //TODO
 * @param pAlloc The memory block to reallocate
 *     If the number of elements allocated in pAlloc is smaller than vOldNum, it must be a power of 2
 * @param vSize The size of each element
 * @param vOldNum The number of elements that are currently allocated //TODO
 * @return The address of the new memory block. This is the same as pAlloc if it has enough free space
 */
void* realloc2(void* pAlloc, uint64_t vSize, uint64_t vOldNum){
	uint64_t step = 0b10 << bsrz(vOldNum);
	if(vOldNum + 1 >= step) pAlloc = realloc(pAlloc, vSize * step);
	return pAlloc;
}






/**
 * @brief //TODOvv
 * @param vStr //TODO
 * @param vBase min 2, max 36 //TODO
 * @return //TODO
 */
double bstrtolf(const char* vStr, const int32_t vBase){
    double res = 0, div = 1;
    for(int32_t dec = 0; *vStr; ++vStr) {
        if(*vStr == '.') dec = 1;
        else {
			char digit = tolower(*vStr);
            double digitVal = digit - ((digit >= 'a') ? 'a' - 10 : '0');
            if(dec) { div *= vBase; res += digitVal / div; }
            else    { res *= vBase; res += digitVal; }
        }
    }
    return res;
}




/**
 * @brief Reads all the contents of the file vFilePath
 *     Removes any '\r' and '\v' character
 *     Tabs are replaced with spaces
 * @param vFilePath The path of the file
 * @param vTabSize The length of tab characters. Max 8
 * @return A null terminated memory block containing the data
 */
char* readFile(const char* vFilePath, uint64_t vTabSize){
	if(vTabSize > 8) vTabSize = 8;
	FILE* const f = fopen(vFilePath, "r");
	fseek(f, 0, SEEK_END);
	const uint64_t size = ftell(f);
	rewind(f);

	char* const data = malloc(vTabSize * size + 1);
	uint64_t i, j, line_i, newlines = 0;
	for(i = j = line_i = 0; i < size; ++i) {
		const char c = fgetc(f);
		switch(c){
			case '\r': break;
			case '\v': break;
			case '\t': {
				const uint64_t n = vTabSize - line_i % vTabSize;
				strncpy(data + j, "        ", n);
				j += n;
				line_i += n;
				break;
			}
			case '\n': {
				if(j && data[j - 1] == '\\') {
					--j;
					++newlines;
				}
				else {
					data[j++] = c;
					for(; newlines; --newlines) data[j++] = '\n';
					line_i = 0;
				}
				break;
			}
			default: {
				data[j++] = c;
				++line_i;
				break;
			}
		}
	}
	data[j] = '\0';

	fclose(f);
	return data;
}
//TODO add newline continuation



/**
 * @brief Returns the address of the vIndex-th occurrence of vChar in the vSrc string
 * @param vSrc The source string
 * @param vChar The character to find
 * @param vIndex The index of the occurrence to find
 * @return The address of the vIndex-th occurrence of vChar
 */
const char* strchrn(const char* vSrc, const char vChar, const uint32_t vIndex){
	uint32_t n = 0;
	for(const char* c = vSrc;; ++c) {
		if(*c == '\0') return NULL;
		else if(*c == vChar) ++n;
		if(n == vIndex + 1) return c;
	}
}
//TODO add start_from parameter


/**
 * @brief Splits vSrc based on vChar and returns the vIndex-th string as a null terminated char*
 *     The original string is not modified
 *     The returned string must be freed
 * @param vSrc The source string
 * @param vChar The character used to split the string
 * @param vIndex The index of the resulting string to return
 * @return The vIndex-th resulting string, or NULL if it does not exist
 */
char* strtokn(const char* vSrc, const char vChar, const uint32_t vIndex){
	const char* const a = vIndex ? strchrn(vSrc, vChar, vIndex - 1) : vSrc;
	const char* const b = strchr(a + !!vIndex, vChar);
	const uint64_t len =  b ? b - a : strlen(a);
	char* const ret = malloc(len);
	memcpy(ret, a + 1, len - !!len);
	ret[len] = '\0';
	return ret;
}
//TODO add start from parameter




/**
 * @brief Prints an error using the printf format specifiers
 *     This function does not return
 * @param vFormat The format string
 * @param ... A list of arguments for the format string
 */
void printError(const char* vFormat, ...){
	va_list vArgs; va_start(vArgs, 0);
	char vStr[MAX_ERR];
	vsnprintf(vStr, MAX_ERR, vFormat, vArgs);

	printf("\n%sGenGlsl: Error:\n%s%s", bRed, vStr, nWht);
	exit(1);
}


/**
 * @brief Prints an error using the printf format specifiers, specifying the line and file in which the error occurred
 *     This function does not return
 * @param iLineN The number of the line. Used to print additional informations
 * @param iLine The line value. Used to print additional informations
 * @param iFile The name of the file. Used to print additional informations
 * @param vFormat The format string
 * @param ... A list of arguments for the format string
 */
void printSyntaxError(const struct Line iLineInfo, const char* const vFormat, ...){
	va_list vArgs; va_start(vArgs, vFormat);
	char vStr[MAX_ERR];
	vsnprintf(vStr, MAX_ERR, vFormat, vArgs);


	printf("%s\nGenGlsl: Syntax error on line %s:%d", bRed, realpath(iLineInfo.file->path, NULL), iLineInfo.lineNum + 1);
	for(struct File* f = iLineInfo.file; f->from; f = f->from){
		printf("\n                Included from %s:%d", realpath(f->from->path, NULL), f->fromLine + 1);
	}
	printf("\n%s%s\n    %s\n\nCompilation stopped", vStr, nWht, iLineInfo.value);

	va_end(vArgs);
	exit(2);
}
