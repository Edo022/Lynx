#pragma once
#include <inttypes.h>
#include <stdio.h>



struct File {			// A struct containing informations about a file
	char* path;				// The path to the file
	struct File* from;		// The file from which this file was included, or 0 for source files
	uint64_t fromLine;		// The line from which this file was included, unset for source files
};
struct File* files;		// A list of all the compiled files. Reallocated dynamically
uint64_t filesNum = 0;		// The number of compiled files




struct Line {			// A struct containing informations about a line
	char* value;			// String value of the line
	uint64_t len;			// Length of the line, '\n' or '\0' excluded
	struct File* file;		// A pointer to the file containing this line
	uint32_t lineNum;		// The number of the line. Starts from 0
};
