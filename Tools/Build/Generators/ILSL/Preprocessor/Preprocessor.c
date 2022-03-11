#include "Utils.h"
#include <unistd.h>         // access() function
#include <sys/stat.h>       // stat() function
#include <linux/limits.h>   // PATH_MAX macro
#include <stdlib.h>
#include <string.h>
#include "Preprocessor/Preprocessor.h"




/**
 * @brief Replaces multiline comments with a space + a number of '\n' corresponding to the number of newlines they contain
 *     Single line comments are replaced with one '\n' character
 * @param code The code to uncomment
 * @param file_path The path of the file containing the code. This is used to print syntax errors
 * @return The resulting string
 */ //FIXME WRITE THE RESULT IN THE SAME ALLOCATION AND DONT RETURN IT
char* uncomment(const char* code, struct File* const file_path){
	char* ret = malloc(MAX_CODE_LEN); ret[0] = '\0';
	const uint64_t code_len = strlen(code);
	uint64_t i = 0;


	//For each character
	while(i < code_len){

		//If the character is a double quote
		if(code[i] == '"'){
			//Check if the string ends
			const char* const str_begin = code + i;
			int ends = 1;
			for(++i; code[i] != '"'; ++i){
				if(!code[i] || code[i] == '\n') {
					ends = 0; break;
				}
			}

			// If it does, paste it and skip closing '"'
			if(ends) {
				strncat(ret, str_begin, i - (str_begin - code) + 1);
				++i;
			}
			// If it doesn't, print a syntax error
			else{
				uint64_t cur_line_num = 0; for(const char* c = code; c < str_begin; ++c) cur_line_num += *c == '\n';
				const char* error_line = cur_line_num ? strchrn(code, '\n', cur_line_num - 1) : code;
				const char* line_str_val = strndup(error_line, strchrn(code, '\n', cur_line_num) - error_line);

				struct Line cur_line_info;
				cur_line_info.str_len     = strlen(line_str_val);
				cur_line_info.str_val     = strdup(line_str_val);
				cur_line_info.parent_file = file_path;
				cur_line_info.loc_line    = cur_line_num;
				print_syntax_error(cur_line_info, "Unterminated string");
			}
		}

		// If it's not and there is enough space to start a comment
		else if(i < code_len - 1){
			if(code[i] == '/' && code[i + 1] == '/'){			//If the character is the beginning of a single line comment
				i += 2;												//Ignore "//"
				while(i < code_len && code[i] != '\n') ++i;		//Ignore the whole comment
				++i;												//Ignore '\n'
				strcat(ret, "\n"); 								// Write '\n'
			}
			else if(code[i] == '/' && code[i + 1] == '*'){	//If the character is the beginning of a multiline comment
				strcat(ret, " ");									//Add a space as token separator
				i += 2;												//Ignore "/*"
				while(i < code_len && !(code[i] == '*' && code[i + 1] == '/')){ //For each character of the comment
					if(code[i] == '\n'){								//If the character is a newline
						strcat(ret, "\n");									//Paste the newline
					}
					++i;												//Update the counter and ignore the other characters
				}
				i += 2;												//Ignore "*/"
			}
			else{												//Else
				strncat(ret, &code[i], 1);						//Paste the character
				++i;												//Update the counter
			}
		}

		// Else paste whatever character was read
		else{
			strncat(ret, &code[i], 1);
			++i;
		}
	}


	//Return the parsed code
	return ret;
}
//FIXME dont use strcat and strncat or just start from an index




/**
 * @brief Checks if an included path is valid
 *     Prints an error if it isn't
 * @param line_info The number of the line. Used to print errors
 * @param path The path to the included file
 */
void check_included_file(const struct Line line_info, const char* path){ //TODO check vLine type
	if(access(path, F_OK) == 0) {
		struct stat file_stat; stat(path, &file_stat);
		if(S_ISDIR(file_stat.st_mode)) {
			print_syntax_error(line_info, "\"%s\" is a directory", path);
		}
	}
	else {
		print_syntax_error(line_info, "No such file or directory");
	}
}




/**
 * @brief Checks if vLine is an include statement and returns the path of the included file
 * @param vLineValue The line to parse
 * @return The path of the included file, or NULL if the line is not an include statement
 */
char* is_include(const struct Line line_info){
	// Check #
	uint64_t i = count_chars(line_info.str_val, ' ');
	if(line_info.str_val[i] != '#') return NULL;
	i += count_chars(line_info.str_val + ++i, ' ');

	// Check "include "
	const char* const include_value = "include ";
	const uint64_t include_len = strlen(include_value);
	if(memcmp(line_info.str_val + i, include_value, include_len)) return NULL;
	i += include_len;

	// Get included file path
	char* const ret = malloc(PATH_MAX);				// Allocate space for the included file path
	const char c = line_info.str_val[i] == '<' ? '>' : line_info.str_val[i] == '"' ? '"' : '\0';
	if(c){												// If either '<' or '"' are used
		++i;												// Ignore '<' or '"'
		const uint64_t line_len = strlen(line_info.str_val);		// Cache the length of the line
		for(uint64_t j = 0; i < line_len; ++i, ++j){		// Get the file path
			if(line_info.str_val[i] == c) {						//
				ret[j] = '\0';								//
				return ret;									// Return the path
			}
			ret[j] = line_info.str_val[i];
		}
	}
	print_syntax_error(line_info, "Invalid include statement");
}



//  * @param pLineNum The address of a uint64_t variable where to store the total number of lines

/** //TODO fix comment
 * @brief Creates a code with no includes by recursively pasting all the included files together
 *     Comments are not preserved
 * @param vCode The code containing the include statements
 * @param file_path The path of the file
 * @param vLineInfo The absolute line from which the file was included. UINT64_MAX if the file is not included
 * @return An array of Line structures of size *pNum containing the lines of all the included files
 */
uint64_t include_file(const char* const path, const uint64_t parent_line, struct File* parent_file, struct Line** const out_lines){
	//Reallocate file array
	source_files_arr = relloc_pow2(source_files_arr, sizeof(struct File), source_files_num);

	source_files_arr[source_files_num].path = strdup(path);
	source_files_arr[source_files_num].parent_line = parent_line;
	source_files_arr[source_files_num].parent_file = parent_file;
	struct File* cur_file = &source_files_arr[source_files_num];
	++source_files_num;



	char* code = read_sanitized_file(path, 4);									// Read the file
	code = uncomment(code, cur_file);											// Uncomment it
	*out_lines = malloc(sizeof(struct Line) * MAX_CODE_LINES);	// Allocate the return array


	char *line; uint64_t tot_line_num = 0;
	for(uint64_t i = 0; (line = strsep(&code, "\n")) != NULL; ++i){			// For each line of the code
		// const uint64_t lineNum = parent_line == UINT64_MAX ? i : parent_line;		// Get the number of the line from which the file was included

		struct Line tmp_isinclude_info; //TODO
		tmp_isinclude_info.parent_file = cur_file;
		tmp_isinclude_info.str_val = line;
		tmp_isinclude_info.str_len = strlen(line);
		tmp_isinclude_info.loc_line = i;
		char* const r = is_include(tmp_isinclude_info);							// Check the line
		if(r){																	// If the line is an include statement
			check_included_file(tmp_isinclude_info, r);								// Check the included file
			struct Line* included_lines;			// Get the lines of the included file
			uint64_t included_lines_num = include_file(r, i, cur_file, &included_lines);	//
			memcpy(*out_lines + tot_line_num, included_lines, sizeof(struct Line) * included_lines_num);	// Copy them in the return array
			free(included_lines);															// Free the saved lines
			tot_line_num += included_lines_num;												// Update the line counter
		}
		else{																	// If it's not
			(*out_lines)[tot_line_num].loc_line = i;											// Set the line numer
			(*out_lines)[tot_line_num].str_len     = strlen(line);									// Set the line length
			(*out_lines)[tot_line_num].str_val   = line;											// Set the line value
			(*out_lines)[tot_line_num].parent_file    = cur_file;										// Set the line file
			++tot_line_num;															// Update the line counter
		}
	}

	// Null terminator line //TODO MOVE TO Tokens.h
	(*out_lines)[tot_line_num].str_val = NULL;
	(*out_lines)[tot_line_num].parent_file = NULL;
	(*out_lines)[tot_line_num].str_len = 0;
	(*out_lines)[tot_line_num].loc_line = (uint32_t)-1;


	return tot_line_num;
}

