#pragma once
#include <inttypes.h>
#include <stdio.h>



struct File {				// A struct containing informations about a file
	char* path;					// The path to the file
	struct File* parent_file;	// The file from which this file was included, or NULL   for non-header .ilsl files
	uint64_t     parent_line;	// The line from which this file was included, undefined for non-header .ilsl files
};
extern struct File* source_files_arr;	// A list of all the compiled files. Reallocated dynamically
extern uint64_t     source_files_num;	// The number of compiled files




struct Line {				// A struct containing informations about a line
	char*        str_val;		// String value of the line
	uint64_t     str_len;		// Length of the string value in bytes. This does not include the null terminator
	struct File* parent_file;	// A pointer to the file that contains this line
	uint32_t     loc_line;		// The local index of the line. Starts from 0
};
