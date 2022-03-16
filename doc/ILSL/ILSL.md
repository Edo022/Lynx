# Intermediate Lynx Shading Language

//TODO matrices and 2d arrays are the same thing. remove the matrices
//TODO vectors only exist to call .x, .y, .z and .w on them

//TODO

Values are divided in named and unnamed values

named                 unnamed
struct fields         struct constructors
array elements        array constructors
variables             literals
temporary variables   function return values



array [0]        = 0;
array [0, 2, n]  = 0;
array [0, 2..60] = 0;

array [0]        = 0;
array [0, 2, n]  = { n - 1, 5, 5 };
array [0, 2..60] = array[102..161];
array [0, 2..60] = array[102..161];

ILSL is a <b>high level compiled shading language</b> based on <b>GLSL 4.6</b>, designed to allow developers to efficiently write shaders without going through the laborious work of manually managing arrays and interfaces.

The language features <b>C-like syntax</b> and semantics.<br>
Additional constructs and keywords allow for better abstraction on data types and a more direct approach to flow control and management of multidimensional data.

Shaders control the graphical output of a class of objects.<br>
Each shader is composed of a single source file (.ilsl) which can include one or more header files (.ilsh).<br>
Header files can also include other header files.<br>
Code execution starts and ends in the `main` function.

Althought files extensions are only a convention and can be changed or omitted, it is still recommended to use the correct ones, as some softwares may use them to determine the language of the source file.

ILSL is part of the <b>Lynx SDK</b>.<br>
[Quick reference tabs](#quick-reference-tabs)

<br><br><br><br>

## 1. Preprocessor

+ [1.1. Comments](#comments)
+ [1.2. Include statement](#include-statement)
+ [1.3. Macros](#macros) //TODO
+ [1.4. Conditionals](#conditionals) //TODO
+ [1.5. Line continuation token](#line-continuation-token)

<br>

## 2. Language components

+ [2.1. Tokens and identifiers](#tokens-and-identifiers)
+ [2.2. Scopes](#scopes)
+ [2.3. Namespaces](#namespaces)
+ [2.4. Types](#types)
  + [2.4.1. Primitive types](#primitive-types)
  + [2.4.2. Structs](#structs)
  + [2.4.3. Arrays](#arrays)
  + [2.4.4. Implicit conversions](#implicit-conversions)
  + [2.4.5. Type aliases](#type-aliases)
  + [2.4.6. Typeof operator](#typeof-operator)
  + [2.4.6. Baseof operator](#baseof-operator)
  + [2.4.7. Auto specifier](#auto-specifier)
+ [2.6. Functions](#functions)
  + [2.6.1. Template functions](#template-functions)
  + [2.6.2. Type classes](#type-classes)
+ [2.7. Const qualifier](#const-qualifier)

<br>

## 3. Runtime statements

+ [3.1. Values](#Values)
  + [3.1.1. Literals](#literals)
  + [3.1.2. Variables](#Variables)
  + [3.1.3. Temporary variables](#temporary-variables)
  + [3.1.4. Array subscript](#array-subscript)
  + [3.1.5. Struct fields](#struct-fields)
  + [3.1.6. Function calls](#struct-fields)
  + [3.1.7. Constructors](#constructors)
+ [3.2. Operators](#operators)
  + [3.2.1 Arithmetic operators](#arithmetic-operators)
  + [3.2.2 Binary operators](#binary-operators)
  + [3.2.3 Logical operators](#logical-operators)
  + [3.2.4 Ternary operator](#ternary-operator)
  + [3.2.5 Grouping](#grouping)
  + [3.2.6 Array operations](#array-operations)
  + [3.2.7 Sequence operator](#sequence-operator) //TODO
+ [3.3. Selection](#selection)
  + [3.3.1. If statement](#if-statement)
  + [3.3.2. Select statement](#select-statement)
+ [3.4. Iteration](#iteration)
  + [3.4.1. For loops](#for-loops)
  + [3.4.2. While loops](#while-loops)
  + [3.4.3. Continue and break statements](#continue-and-break-statements)
  + [3.4.4. Once statement](#once-statement)
  + [3.4.5. Then statement](#then-statement)
+ [3.5. Else statement](#else-statement)
+ [3.6. Return statement](#return-statement)

<br>

## 4. Standard modules

+ [4.1. Built-in functions](#built-in-functions) //TODO

<br>

## 5. Compilation

+ [C++ interface //TODO REMOVE]
+ [ILSL/C++ data compatibility //TODO change to lynx language compatibility]
+ [5.1. Compilation phases and error types](#compilation-phases-and-error-types)
+ [5.2. Branching performance](#branching-performance)
+ [5.3. Compiler optimizations](#compiler-optimizations)

<br><br><br><br>

---

<br><br><br><br>

# Preprocessor

As in the C language, the preprocessing phase is performed <b>before</b> compiling the code.<br>
It removes comments and evaluates preprocessor directives and line continuation tokens.

Preprocessor directives are special instructions that allow the code to be programmatically modified.<br>
All the preprocessor directives are identified by the `#` character followed by their name and definition.<br>
Each directive spans over a single, complete line of code.<br>
Preprocessor directives are always <b>case sensitive</b>.

<br>

## Comments

Comments are portions of code that are purposely ignored.<br>
They can be used to provide informations or temporarily exclude lines of code.<br>
Comments are allowed to contain any sequence of unicode characters of any length.<br>
//TODO

Single line comments start with `//` and end at the first newline character<br>
Multiline comments start with `/*` and end with `*/`

```c
// Single line comment

/* Multi
   line 
   comment */
```

<br>

## Include statements

Include statements are used to <b>paste code</b> from other files.<br>
Any existent file can be included, regardless of its contents or extension.<br>
The path to the included file must be enclosed in `""` or `<>` and can only contain alphanumeric characters, dots and slashes. //TODO <br>
Both <b>absolute</b> and <b>relative</b> paths are supported.

Syntax:<br>
`#include "abs/path/file"`<br>
`#include "./rel/path/file"`<br>
`#include <abs/path/file>`<br>
`#include <./rel/path/file>`

```c
// Ok
#include <file.ilsh>
#include <./file.ilsh>
#include </home/Lynx/ILSL/file.ilsh>
#include "./file.ilsh"

// Error
//#include ./file.ilsh
//#include "./non_existent_file"
//#include ""
```

<br>

## Macros

<br>

## Conditionals

<br>

## Line continuation token

The line continuation token can be used to merge multiple lines and allow single line comments, preprocessor directives and multi character tokens to be written across multiple lines.<br>
It consists of a `\` <b>immediatly</b> followed by a <b>newline character</b>.

These are all allowed:

```c
// single line comment\
   on two lines

// #include "path/to/file"
#include "pat\
h/to/file\
"

// void function() {
//   var += b;
// }
vo\
id fun\
ction() {
  v\
ar +\
= b;
}

```

<br><br><br><br>

# Language components

<br>

## Tokens and identifiers

Any sequence of one or more characters that is left after the preprocessing phase and is recognizable according to the grammar of the language is called a token.<br>
Comments and whitespace characters can be freely written between two tokens without altering the GLSL output.<br>
Tokens are always <b>case sensitive</b>.<br>
Comments, newline characters, whitespace characters and preprocessor directives are <b>not tokens</b>.

Each token can be classified as one of the following:

| Type                                | Pattern                                                                                        | Example                                            |
|-------------------------------------|------------------------------------------------------------------------------------------------|----------------------------------------------------|
| Identifier                          | Any sequence of `alphanumeric characters`, `$` and `_` that doesn't start with a decimal digit | `Var_1` `function` `$0` `MACRO_NAME_$_1`           |
| Literal                             | Any sequence of `alphanumeric characters` and `.` that start withs a decimal digit             | `0x12a4bc` `266250` `0b1010001` `0invalid_literal` |
| Keywords                            | Predefined alphabetic identifiers                                                              | `for` `if` `namespace` `then`                      |
| Operator                            | Predefined sequences of symbols                                                                |  `=` `<<=` `+` `~`                                 |
| Scope delimiters                    | `{` `}`                                                                                        |                                                    |
| Function arguments delimiters       | `(` `)`                                                                                        |                                                    |
| Array declaration                   | `[` `]`                                                                                        |                                                    |
| Expression separator                | `,`                                                                                            |                                                    |
| Statement separator                 | `;`                                                                                            |                                                    |
| Reference token                     | `&`                                                                                            |                                                    |

<br>

Identifiers are used to name functions, variables, structs, aliases, type classes and namespaces.<br>
They must be unique to the current scope and not overlap with keywords.<br>
Temporary variables are the only identifiers that are allowed to start with a `$`.

See [Literals](#literals), [Operators](#operators), [Scopes](#scopes)

<br>

## Scopes

<br>

## Variables

Variables are <b>named memory locations</b> that can contain values of the specified type.<br>
Each variable <b>must be declared</b> before it is used. Variables <b>cannot be redeclared</b> and their <b>type cannot be changed</b> after the declaration.

A variable declaration consists of the type, the name and an optional `=` followed by an expression of a compatible type that is used to <b>inizialize it</b>.<br>
If an initializer expression is not specified, the value of the variable is <b>undefined</b>.<br>
<b>Evaluating</b> an expression that can result in an <b>undefined value</b> is considered a <b>logical error</b> and will stop the compilation process.<br>
Variables with undefined value can still be passed to reference arguments of functions or be assigned a value.

Syntax:<br>
`type identifier;`<br>
`type identifier = expression;`<br>

```c
u32 variable = 0;       // Ok, variable has value 0
f32 nya = 5 + variable; // Ok, nya ha value 5.0
b var$;                 // Ok, var$ has undefined value
//b var_2 = var$;       // Error. The syntax is correct, but undefined values (var$) cannot be used inside expressions

//f64 variable;         // Error, the identifier "variable" is already declared
//f64 foo               // Error, ';' is not optional
//b $var;               // Error, invalid identifier
//b 2var;               // Error, invalid identifier
```

See [Types](#types), [Expressions](#expressions), [Functions](#functions)

<br>

## Temporary variables

Temporary variables are a special set of identifiers that have dynamic type and don't need to be declared.<br>
These identifiers are meant to replace local variables whose only purpose is holding values that don't need to be used more than a few times.<br>
A temporary variable is identified by the `$` character followed by an arbitrary number of decimal digits, which denote its name.<br>

They are always <b>function-level scoped</b> and cannot be used outside of function definitions.<br>
There is no limit to the number of temporary variables a function can use.<br>

The <b>type</b> of a temporary variable can only be changed through direct assignments (`=`) and it must be <b>unambiguously determinable</b> in order for the variable to be evaulated.
This means that reading the value of a temporary variable after one or more branches that may have assigned it <b>different types</b> is not allowed and will result in a <b>semantic error</b>.<br>
Using temporary variables whose type is undefined will also result in a semantic error.

They act as normal variables for anything else.

See [Types](#types), [Selection](#selection), [Functions](#functions), [Expressions](#expressions)

```c
u32v3 fun1(b arg) {
    $0 = 1;             // Ok, $0 has type u32 and value 1
    //$2 = $1;          // Error, $1 has undefined type

    u32v3 vec = { 0, 1, 2 };
    $2 = true;          // Ok, $2 has type b and value true
    $2 = vec;           // Ok, $2 has type u32v3 and value { 0, 1, 2 }
}

void fun2(u32v3 vec) {
    u32 var = $2;       // Error, $2 has undefined type. fun2::$2 has nothing to do with fun1::$2
}

u32 var = $9;           // Error, temporary variables cannot be used outside of functions
```

```c
void fun3(b arg, u32v2 &ref) {
    if(arg) $0 = 1;             // $0 has type u32
    else    $0 = { 0, 1 };      // $0 has type u32v2
    //ref = $0;                 // Error, cannot determine the type of $0
    $0 = ref;                   // Ok, $0 has type u32v2 and the same value as ref

    if(arg) $0 = { 0, 1 };      // $0 has type u32v2
    else    $0 = { 2, 3 };      // $0 has type u32v2
    ref = $0;                   // Ok, $0 has type u32v2
}
```

<br>

## Functions

Functions are the <b>main component</b> of the language. They contain the statements that will be executed during run time and manage inputs and outputs of the shader.<br>
Similarly to variables, each function <b>must be defined</b> in order to be called, but their <b>position</b> in the code is <b>irrelevant</b>. Functions <b>cannot be redefined</b>.

A function definition consists of its type and name followed by a list of arguments and its statements enclosed in `{` `}` delimiters.<br>
Functions are allowed to take no arguments.

The <b>type</b> of the function defines the type of the <b>value</b> that is <b>returned</b> by the function when called.<br>
Values are returned using the `return` statement. When this happens, all the remaining statements in the function are skipped and the code execution returns to the caller function.<br>
Functions that <b>don't return</b> a value must be defined using the <b>`void` type</b>.<br>
Void functions are allowed to contain no statements. <b>Non-void</b> functions must <b>always return</b> a value.

Syntax:

```.txt
type name(type1 arg1, type2 arg2, ...) {
    statement1;
    statement2;
    ...
    return expression;
}
```

See [Runtime statements](#runtime-statements), [Types](#types)

```c
void empty_function_1() { return; }   // Ok
void empty_function_2() {}            // Ok

u32 empty_function_3() { return 0; }  // Ok
//u32 empty_function_4() {}           // Error, non-void functions must return a value

u32 f(u32 n) { return n * 2; }        // Ok
//void f() { }                        // Error, f is already defined

void main(){
    u32 n = empty_function_3();       // Ok
    n = f(f(n));                      // Ok
    //f32 f;                          // Error, f is already defined as a function
}
```

<br>

A function can take an <b>arbitrary number</b> of <b>parameters</b> of any type. Parameters are declared like variables and behave in the same exact way.<br>
The expressions used to call the function are called arguments.<br>
Each argument <b>must be implicitly convertible</b> to the type of the parameter.

By <b>default</b>, the arguments are passed <b>by value</b>, which means that when the function is called, the value of each argument is <b>copied</b> in the corresponding parameter and the function <b>cannot access</b> the original variable.<br>
If a parameter name is preceeded by a <b>`&`</b> token, instead, the argument is passed <b>by reference</b>, allowing the function to <b>access and modify</b> its value

Declaring a reference parameter as const is a syntax error and passing an unnamed value as argument of a reference parameter is a semantic error.<br>
Notice that in this case, the `&` token is not part of the type nor part of the name, but a language feature that is used exclusively within parameter declarations.

see [Const qualifier](#const-qualifier), [Literals and unnamed values](#literals-and-unnamed-values)

```c
void fun1(u32 arg1, u32 &arg2) {
    arg1 = 0;     // This doesn't modify main::a, but the local argument in which its value was copied
    arg2 = 0;     // This modifies main::b
}

void main(){
    u32 a, b;
    fun1(a, b);   // a keeps an undefined value, b is set to 0
    //fun1(a, 5); // Error, cannot pass literals to reference parameters
}

//void fun2(const i32 &arg1){} // Error, references cannot be const
```

<br>

The `main` function is where the <b>code execution starts and ends</b>.<br>
Every shader <b>must define</b> the <b>main</b> as a <b>void</b> function. As such, it cannot return values.<br>

<br>

## Const qualifier

The `const` qualifier can be used to <b>prevent</b> named values to be <b>modified</b> after their declaration.<br>
Reference const parameters are not allowed, as well as const functions and const struct members.<br>

As opposed to C and GLSL, const identifiers can be initialized using non-const expressions.<br>
The qualifier doesn't affect compiler optimizations or the GLSL output in any way.

Passing a const identifier as argument of reference parameters is not allowed and will result in a semantic error.

see [Compiler optimizations](#compiler-optimizations)

```c
struct s {
    u32 a;            // Ok, non-const member
    //u32 const b;    // Error, struct members cannot be const
} const var;          // Ok, const global variable

//f64 const fun(){}                 // Error, functions cannot be const
//f64 fun(f64 const &arg){}         // Error, references cannot be const

f64 fun(f64 const arg) {            // Ok, const parameter
    u64 const a, const b = arg, c;  // Ok, uninitialized const, inizialized const and uninitialized variables;
    //a = 2;                        // Error, cannot assign values to const variables
    //a = b;                        // Error, cannot assign values to const variables
    c = a + b;                      // Ok
}
```

<br>

## Namespaces

<br><br><br><br>

# Types

<br>

## Scalar types

<br>

## Structs

<br>

## Arrays

<br>

## Vectors

<br>

## Type constructors

<br><br><br><br>

# Runtime statements

<br>

## Expressions

### Literals and unnamed values

//TODO constructors are also literals
//TODO literals don't have to be compile time

### Function calls and variables

### Array subscript

### Struct fields

### Implicit conversions

### Operators

#### Arithmetic operators

#### Binary operators

#### Logical operators

#### Ternary operator

#### Array operations

#### Sequence operator

### Grouping

<br>

## Selection

### If statement

### Select statement

### Fallthrough statement

<br>

## Iteration

### For loop

### While loop

### Continue and break statements

### Once statement

### Then statement

<br>

## Else statement

<br>

## Return statement

<br><br><br><br>

# Built-in functions

<br><br><br><br>

# Compilation

<br>

## 7.1 C++ interface //TODO REMOVE

<br>

## 7.2 ILSL/C++ data compatibility //TODO change to lynx language compatibility

<br>

## Compilation phases and error types

<br>

## Selection directives performance

<br>

## Compiler optimizations

<br><br><br><br>

# Quick reference tabs

//TODO
this.x this.y this.z
^ coordinates in current workspace
