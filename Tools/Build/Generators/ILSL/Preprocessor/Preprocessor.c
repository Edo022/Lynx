#include "Utils.h"
#include <unistd.h>         // access() function
#include <sys/stat.h>       // stat() function
#include <linux/limits.h>   // PATH_MAX macro
#include <stdlib.h>
#include <string.h>
#include "Preprocessor/Preprocessor.h"



/** //TODO prob useless, remove the function
 * @brief Removes the trailing whitespace of each line
 *     Consecutive newline characters are preserved
 * @param vLines An array of Line structs containing the lines to clear
 */
void clear(struct Line* vLines){
	for(uint64_t i = 0; vLines[i].value; ++i){
		struct Line* const l = &vLines[i];

		for(int64_t j = l->len - 1;; --j){
			//Lines containing only whitespace or empty lines
			if(j < 0){
				l->value[0] = '\0';
				l->len = 0;
				break;
			}
			// Lines with trailing whitespace or none
			else if(l->value[j] != ' ') {
			//!^ '\r' are removed, tabs are replaced with spaces and newlines are not part of the line value
				l->value[j + 1] = '\0';
				l->len = j;
				break;
			}
		}
	}
}




/**
 * @brief Replaces multiline comments with a space + a number of '\n' corresponding to the number of newlines they contain
 *     Single line comments are replaced with one '\n' character
 * @param vCode The code to uncomment
 * @param iFile The path of the file containing the code. This is used to print syntax errors
 * @return The resulting string
 */ //FIXME WRITE THE RESULT IN THE SAME ALLOCATION AND DONT RETURN IT
char* uncomment(const char* vCode, struct File* const iFile){
	char* code = malloc(MAX_CODE_LEN); code[0] = '\0';
	const uint64_t vCodeLen = strlen(vCode);
	uint64_t i = 0;


	//For each character
	while(i < vCodeLen){

		//If the character is a double quote
		if(vCode[i] == '"'){
			//Check if the string ends
			const char* const str_begin = vCode + i;
			int ends = 1;
			for(++i; vCode[i] != '"'; ++i){
				if(!vCode[i] || vCode[i] == '\n') {
					ends = 0; break;
				}
			}

			// If it does, paste it and skip closing '"'
			if(ends) {
				strncat(code, str_begin, i - (str_begin - vCode) + 1);
				++i;
			}
			// If it doesn't, print a syntax error
			else{
				uint64_t vLineN = 0; for(const char* c = vCode; c < str_begin; ++c) vLineN += *c == '\n';
				const char* errorLine = vLineN ? strchrn(vCode, '\n', vLineN - 1) : vCode;
				const char* iLineValue = strndup(errorLine, strchrn(vCode, '\n', vLineN) - errorLine);

				struct Line iLineInfo;
				iLineInfo.len     = strlen(iLineValue);
				iLineInfo.value   = strdup(iLineValue);
				iLineInfo.file    = iFile;
				iLineInfo.locLine = vLineN;
				printSyntaxError(iLineInfo, "Unterminated string");
			}
		}

		// If it's not and there is enough space to start a comment
		else if(i < vCodeLen - 1){
			if(vCode[i] == '/' && vCode[i + 1] == '/'){			//If the character is the beginning of a single line comment
				i += 2;												//Ignore "//"
				while(i < vCodeLen && vCode[i] != '\n') ++i;		//Ignore the whole comment
				++i;												//Ignore '\n'
				strcat(code, "\n"); 								// Write '\n'
			}
			else if(vCode[i] == '/' && vCode[i + 1] == '*'){	//If the character is the beginning of a multiline comment
				strcat(code, " ");									//Add a space as token separator
				i += 2;												//Ignore "/*"
				while(i < vCodeLen && !(vCode[i] == '*' && vCode[i + 1] == '/')){ //For each character of the comment
					if(vCode[i] == '\n'){								//If the character is a newline
						strcat(code, "\n");									//Paste the newline
					}
					++i;												//Update the counter and ignore the other characters
				}
				i += 2;												//Ignore "*/"
			}
			else{												//Else
				strncat(code, &vCode[i], 1);						//Paste the character
				++i;												//Update the counter
			}
		}

		// Else paste whatever character was read
		else{
			strncat(code, &vCode[i], 1);
			++i;
		}
	}


	//Return the parsed code
	return code;
}
//FIXME dont use strcat and strncat or just start from an index




/**
 * @brief Checks if an included path is valid
 *     Prints an error if it's not
 * @param iLineValue The line value. Used to print errors
 * @param iLineNum The number of the line. Used to print errors
 * @param iFile The file path. Used to print errors
 * @param vPath The path of the included file
 */
void checkIncludeFile(const struct Line iLineInfo, const char* vPath){ //TODO check vLine type
	if(access(vPath, F_OK) == 0) {
		struct stat fileStat; stat(vPath, &fileStat);
		if(S_ISDIR(fileStat.st_mode)) {
			printSyntaxError(iLineInfo, "\"%s\" is a directory", vPath);
		}
	}
	else {
		printSyntaxError(iLineInfo, "No such file or directory");
	}
}




/**
 * @brief Checks if vLine is an include statement and returns the path of the included file
 * @param vLineValue The line to parse
 * @return The path of the included file, or NULL if the line is not an include statement
 */
char* isInclude(const struct Line iLineInfo){
	// Check #
	uint64_t i = countChar(iLineInfo.value, ' ');
	if(iLineInfo.value[i] != '#') return NULL;
	i += countChar(iLineInfo.value + ++i, ' ');

	// Check "include "
	const char* const include_value = "include ";
	const uint64_t include_len = strlen(include_value);
	if(memcmp(iLineInfo.value + i, include_value, include_len)) return NULL;
	i += include_len;

	// Get included file path
	char* const ret = malloc(PATH_MAX);				// Allocate space for the included file path
	const char c = iLineInfo.value[i] == '<' ? '>' : iLineInfo.value[i] == '"' ? '"' : '\0';
	if(c){												// If either '<' or '"' are used
		++i;												// Ignore '<' or '"'
		const uint64_t line_len = strlen(iLineInfo.value);		// Cache the length of the line
		for(uint64_t j = 0; i < line_len; ++i, ++j){		// Get the file path
			if(iLineInfo.value[i] == c) {						//
				ret[j] = '\0';								//
				return ret;									// Return the path
			}
			ret[j] = iLineInfo.value[i];
		}
	}
	printSyntaxError(iLineInfo, "Invalid include statement");
}



//  * @param pLineNum The address of a uint64_t variable where to store the total number of lines

/** //TODO fix comment
 * @brief Creates a code with no includes by recursively pasting all the included files together
 *     Comments are not preserved
 * @param vCode The code containing the include statements
 * @param vFile The path of the file
 * @param vLineInfo The absolute line from which the file was included. UINT64_MAX if the file is not included
 * @return An array of Line structures of size *pNum containing the lines of all the included files
 */
// struct Line* include(const char* const vFile, const uint64_t vFromLine, struct File* vFromFile, uint64_t* const pLineNum){
uint64_t include(const char* const vFile, const uint64_t vFromLine, struct File* vFromFile, struct Line** const pLines){
	//Reallocate file array
	files = reallocPow2(files, sizeof(struct File), filesNum);

	files[filesNum].path = strdup(vFile);
	files[filesNum].fromLine = vFromLine;
	files[filesNum].from = vFromFile;
	struct File* curFile = &files[filesNum];
	++filesNum;



	char* code = readFile(vFile, 4);									// Read the file
	code = uncomment(code, curFile);											// Uncomment it
	*pLines = malloc(sizeof(struct Line) * MAX_CODE_LINES);	// Allocate the return array


	char *line; uint64_t totLineNum = 0;
	for(uint64_t i = 0; (line = strsep(&code, "\n")) != NULL; ++i){			// For each line of the code
		// const uint64_t lineNum = vFromLine == UINT64_MAX ? i : vFromLine;		// Get the number of the line from which the file was included

		struct Line tmp_isinclude_info; //TODO
		tmp_isinclude_info.file = curFile;
		tmp_isinclude_info.value = line;
		tmp_isinclude_info.len = strlen(line);
		tmp_isinclude_info.locLine = i;
		char* const r = isInclude(tmp_isinclude_info);							// Check the line
		if(r){																	// If the line is an include statement
			checkIncludeFile(tmp_isinclude_info, r);								// Check the included file
			struct Line* included;			// Get the lines of the included file
			uint64_t includedLen = include(r, i, curFile, &included);	//
			memcpy(*pLines + totLineNum, included, sizeof(struct Line) * includedLen);	// Copy them in the return array
			free(included);															// Free the saved lines
			totLineNum += includedLen;												// Update the line counter
		}
		else{																	// If it's not
			(*pLines)[totLineNum].locLine = i;											// Set the line numer
			(*pLines)[totLineNum].len     = strlen(line);									// Set the line length
			(*pLines)[totLineNum].value   = line;											// Set the line value
			(*pLines)[totLineNum].file    = curFile;										// Set the line file
			++totLineNum;															// Update the line counter
		}
	}

	// Null terminator line //TODO MOVE TO Tokens.h
	(*pLines)[totLineNum].value = NULL;
	(*pLines)[totLineNum].file = NULL;
	(*pLines)[totLineNum].len = 0;
	(*pLines)[totLineNum].locLine = (uint32_t)-1;


	return totLineNum;
}

