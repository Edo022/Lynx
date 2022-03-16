#include "Utils.h"
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
//TODO rewrite these weird and confusing comments



/**
 * @brief Returns the number of characters chr at the beginning of the string
 * @param str The string in which to search the characters
 * @param chr The characters to count
 * @return the number of characters chr at the beginning of the string
 *     Empty strings return 0
 */
uint64_t count_chars(const char* const str, const char chr){
	const char* s = str;
	for(; *s == chr; ++s);
	return s - str;
}





/**
 * @brief Reallocates ptr to (size) * (the smallest power of 2 that can contain (cur_num + 1) elements)
 *     //TODO
 * @param ptr The memory block to reallocate
 *     If the number of elements allocated in ptr is smaller than cur_num, it must be a power of 2
 * @param size The size of each element
 * @param cur_num The number of elements that are currently allocated //TODO
 * @return The address of the new memory block. This is the same as ptr if it has enough free space
 */
void* relloc_pow2(void* ptr, uint64_t size, uint64_t cur_num){
	uint64_t step = 0b10 << bsrz(cur_num);
	// if(cur_num + 1 == step) ptr = realloc(ptr, size * step);
	return realloc(ptr, size * step);
	// return ptr;
}






/**
 * @brief //TODOvv
 * @param str //TODO
 * @param base min 2, max 36 //TODO
 * @return //TODO
 */
double bstrtolf(const char* str, const int32_t base){
    double res = 0, div = 1;
    for(int32_t dec = 0; *str; ++str) {
        if(*str == '.') dec = 1;
        else {
			char digit = tolower(*str);
            double digit_val = digit - ((digit >= 'a') ? 'a' - 10 : '0');
            if(dec) { div *= base; res += digit_val / div; }
            else    { res *= base; res += digit_val; }
        }
    }
    return res;
}




/**
 * @brief Reads all the contents of the file path
 *     Removes any '\r' and '\v' character
 *     Tabs are replaced with spaces
 * @param path The path of the file
 * @param tab_size The length of tab characters. Max 8
 * @return A null terminated memory block containing the data
 */
char* read_sanitized_file(const char* path, uint64_t tab_size){
	if(tab_size > 8) tab_size = 8;
	FILE* const f = fopen(path, "r");
	fseek(f, 0, SEEK_END);
	const uint64_t size = ftell(f);
	rewind(f);

	char* const data = malloc(tab_size * size + 1);
	uint64_t i, j, line_i, newlines = 0;
	for(i = j = line_i = 0; i < size; ++i) {
		const char c = fgetc(f);
		switch(c){
			case '\r': break;
			case '\v': break;
			case '\t': {
				const uint64_t n = tab_size - line_i % tab_size;
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
 * @brief Returns the address of the index-th occurrence of chr in the str string
 * @param str The source string
 * @param chr The character to find
 * @param index The index of the occurrence to find
 * @return The address of the index-th occurrence of chr
 */
const char* strchrn(const char* str, const char chr, const uint32_t index){
	uint32_t n = 0;
	for(const char* c = str;; ++c) {
		if(*c == '\0') return NULL;
		else if(*c == chr) ++n;
		if(n == index + 1) return c;
	}
}
//TODO add start_from parameter


/**
 * @brief Splits str based on chr and returns the index-th string as a null terminated char*
 *     The original string is not modified
 *     The returned string must be freed
 * @param str The source string
 * @param chr The character used to split the string
 * @param index The index of the resulting string to return
 * @return The index-th resulting string, or NULL if it does not exist
 */
char* strtokn(const char* str, const char chr, const uint32_t index){
	const char* const a = index ? strchrn(str, chr, index - 1) : str;
	const char* const b = strchr(a + !!index, chr);
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
 * @param format The format string
 * @param ... A list of arguments for the format string
 */
void print_generic_error(const char* format, ...){
	va_list args; va_start(args, 0);
	char str[MAX_ERR];
	vsnprintf(str, MAX_ERR, format, args);

	printf(col_bred "\nILSL: Error:\n%s" col_nwht, str);
	exit(1);
}


/**
 * @brief Prints an error using the printf format specifiers, specifying the line and file in which the error occurred
 *     This function does not return
 * @param line_info additional line informations
 * @param format The format string
 * @param ... A list of arguments for the format string
 */
void print_syntax_error(const struct Line line_info, const char* const format, ...){
	va_list args; va_start(args, format);
	char str[MAX_ERR];
	vsnprintf(str, MAX_ERR, format, args);

	printf(col_bred "\nILSL: Syntax error" col_nwht " on line %s:%d", line_info.parent_file->abs_path, line_info.loc_line + 1);
	for(struct File* f = line_info.parent_file; f->parent_file; f = f->parent_file){
		printf("\n                Included from %s:%d", f->parent_file->abs_path, f->parent_line + 1);
	}
	printf(col_bred "\n%s" col_nwht "\n    %s\n\nCompilation stopped", str, line_info.str_val);

	va_end(args);
	exit(2);
}

