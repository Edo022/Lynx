#include "Utils.h"
#include <linux/limits.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>





/**
 * @brief Returns the number of characters vChar at the beginning of the string
 * @param vLine The string in which to search the characters
 * @param vChar The characters to count
 * @return the number of characters vChar at the beginning of the string
 *     Empty strings return 0
 */
uint64_t countChar(const char* const vLine, const char vChar){
	const char* s = vLine;
	for(; *s == vChar; ++s);
	return s - vLine;
}





/**
 * @brief Reallocates pAlloc to (vSize) * (the smallest power of 2 that can contain (vOldNum + 1) elements)
 *     //TODO
 * @param pAlloc The memory block to reallocate
 *     If the number of elements allocated in pAlloc is smaller than vOldNum, it must be a power of 2
 * @param vSize The size of each element
 * @param vOldNum The number of elements that are currently allocated //TODO
 * @return The address of the new memory block. This is the same as pAlloc if it has enough free space
 */
void* reallocPow2(void* pAlloc, uint64_t vSize, uint64_t vOldNum){
	uint64_t step = 0b10 << bsrz(vOldNum);
	// if(vOldNum + 1 == step) pAlloc = realloc(pAlloc, vSize * step);
	return realloc(pAlloc, vSize * step);
	// return pAlloc;
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


	printf("%s\nGenGlsl: Syntax error%s on line %s:%d", bRed, nWht, realpath(iLineInfo.file->path, NULL), iLineInfo.lineNum + 1);
	for(struct File* f = iLineInfo.file; f->from; f = f->from){
		printf("\n                Included from %s:%d", realpath(f->from->path, NULL), f->fromLine + 1);
	}
	printf("\n%s%s%s\n    %s\n\nCompilation stopped", bRed, vStr, nWht, iLineInfo.value);

	va_end(vArgs);
	exit(2);
}

