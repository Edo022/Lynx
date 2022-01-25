#pragma once
#include <ILSL/Tokens.h>




struct Var;
struct Function;
struct Struct;

struct Scope{
    struct Scope* parent;		// The parent scope of the scope
    struct Function* functionv;	// An array  of functions declared in the scope
    uint64_t functionc;			// The numer of functions declared in the scope
    struct Var* varv;			// An array  of variables declared in the scope
    uint64_t varc;				// The numer of variables declared in the scope
    struct Struct* structv;		// An array  of structures declared in the scope
    uint64_t strctc;			// The numer of structures declared in the scope
};

struct Var{
    struct Scope* parent;		// The parent scope of the variable
    enum TokenID type;			// THe type of the variable
    bool is_const;				// True if the function has const type, false otherwise
    char* name;					// The name of the function
};

struct Struct{
    struct Scope* parent;		// The parent scope of the struct
    char* name;					// The name of the struct
    struct Var* memberv;		// An array of members
    uint64_t memberc;			// The number of members
};


struct Function{
    struct Scope* parent;		// The parent scope of the function
    enum TokenID type;			// The type of the function
    char* name;					// The name of the function
    // struct Var* paramv;			// An array of parameters //TODO same the variables in the function's scope
    // uint64_t paramc;			// The number of parameters
    struct Scope scope;			// The scope of the function
    struct Token* exec; 		//runtime lines as a list of tokens
};

