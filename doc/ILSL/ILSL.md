 
# Intermediate Lynx Shading Language

## Table of contents

//TODO add links

//TODO matrices and 2d arrays are the same thing. remove the matrices
//TODO vectors only exist to call .x, .y, .z and .w on them




//TODO
free expression
enclosed expression

syntactic structure
e.g.
if(expression) {
  instructions
} else {
  instructions
}

for(instruction; expression; instruction) {
  instructions
}





//TODO
array [0]        = 0;
array [0, 2, n]  = 0;
array [0, 2..60] = 0;

array [0]        = 0;
array [0, 2, n]  = { n - 1, 5, 5 };
array [0, 2..60] = array[102..161];
array [0, 2..60] = array[102..161];







- 1\. Overview
- 2\. Preprocessor directives
  - 2.1\. Include statements
  - 2.2\. Macros
  - 2.3\. Conditionals
  - 2.4\. Line continuation token
- 3\. Language constructs
  - 3.1\. Variables
  - 3.2\. Functions
  - 3.3\. Const qualifier
  - 3.4\. Local and extern qualifiers
  - 3.5\. Temporary variables
- 4\. Types
  - 4.1\. Scalar types
  - 4.2\. Structs
  - 4.3\. Arrays
  - 4.4\. Vectors
  - 4.5\. Type constructors
- 5\. Operators
  - 5.1\. Arithmetic operators
  - 5.2\. Binary operators
  - 5.2\. Logical operators
  - 5.2\. Ternary operator
  - 5.3\. Implicit conversions
- 6\. Runtime instructions
  - 6.1\. Expressions
    - 6.1.1\. Operators
    - 6.1.2\. Literals
    - 6.1.3\. Grouping
    - 6.1.4\. Function calls
    - 6.1.5\. Array subscript
    - 6.1.6\. Ranges
  - 6.2\. Selection
    - 6.2.1\. If statement
    - 6.2.2\. Select statement
    - 6.2.3\. Fallthrough statement
  - 6.3\. Iteration
    - 6.3.1\. For loop
    - 6.3.2\. While loop
    - 6.3.3\. Do-while loop
    - 6.3.4\. Continue and break statements
    - 6.3.5\. Then statement
  - 6.4\. Else statement
  - 6.5\. Return statement
- 7\. Built-in functions
- 8\. Compilation
  - 8.1\. C++ interface //TODO REMOVE
  - 8.2\. ILSL/C++ data compatibility //TODO change to lynx language compatibility
  - 8.3\. Compiler optimizations


<br><br><br><br>

## 1. Overview

ILSL is a <b>high level compiled shading language</b> based on <b>GLSL 4.6</b>, designed to allow developers to efficiently write shaders without going through the laborious work of manually managing arrays and interfaces.

The language features <b>C-like syntax</b> and semantics.<br>
Additional constructs and keywords allow for better abstraction on data types and a more direct approach to flow control and management of multidimensional data.

Shaders control the graphical output of a class of objects.<br>
Each shader is composed of a single source file (.ilsl) which can include one or more header files (.ilsh).<br>
Header files can also include other header files.

Althought files extensions are only a convention and can be changed or omitted, it is still recommended to use the correct ones, as some softwares may use them to determine the language of the source file.

ILSL is part of the <b>Lynx SDK</b>.

<br><br><br><br>

## 2. Syntax

### 2.1. Identifiers

Identifiers are used to name variables, functions, structures, fields and macros.<br>
Any sequence of alphanumeric characters or underscores that starts with an alphabetic character or an underscore is considered an identifier.<br>
Identifiers cannot contain special characters, whitespace or start with digits.

Certain identifiers cannot be used as they are reserved for keywords or built-int types, variables and functions:

```text
//TODO add built in functions
//TODO add glsl function
//TODO fix unsupported matrices
b        u32        i32        f32        f64
bv2      u32v2      i32v2      f32v2      f64v2
bv3      u32v3      i32v3      f32v3      f64v3
bv4      u32v4      i32v4      f32v4      f64v4
bm2      u32m2      i32m2      f32m2      f64m2
bm3      u32m3      i32m3      f32m3      f64m3
bm4      u32m4      i32m4      f32m4      f64m4
bm2x2    u32m2x2    i32m2x2    f32m2x2    f64m2x2
bm2x3    u32m2x3    i32m2x3    f32m2x3    f64m2x3
bm2x4    u32m2x4    i32m2x4    f32m2x4    f64m2x4
bm3x2    u32m3x2    i32m3x2    f32m3x2    f64m3x2
bm3x3    u32m3x3    i32m3x3    f32m3x3    f64m3x3
bm3x4    u32m3x4    i32m3x4    f32m3x4    f64m3x4
bm4x2    u32m4x2    i32m4x2    f32m4x2    f64m4x2
bm4x3    u32m4x3    i32m4x3    f32m4x3    f64m4x3
bm4x4    u32m4x4    i32m4x4    f32m4x4    f64m4x4
void

if          while        continue        switch
else        for          break           case
elif        do           return          default

highp       local        struct
medp        extern       preicison
lowp        const
```

### 2.2. Variables

//TODO

### 2.4. Scope

//TODO

### 2.5. Functions

//TODO

<br><br><br><br>

## 3. Flow control

//TODO

### 3.1. Logical operators

//TODO
! == != && || ^^ < > <= >=

### 3.2. Selection statements

//TODO
if-else
elif

### 3.4. Ternary operator

//TODO
bool ? if_true : if_false

### 3.5. While and for loops

//TODO

### 3.6. Continue and break statements

//TODO

### 3.7. Do-while

//TODO

### 3.8. For-else

//TODO
for(bool){
    ...
    if(something) break;
}
else something_if_break_was_reached();

### 3.9. Return

//TODO
cant return arrays
(?) multiple values

<br><br><br><br>

## 4. Types

//TODO

### 4.1. Scalar types

//TODO

### 4.2. Vectors

//TODO

### 4.3. Matrices

//TODO

### 2.4. Arithmetic and binary operators

//TODO
+  -  *  /  <<  >>  &  |  ^  %
+= -= *= /= <<= >>= &= |= ^= %=
~ = ++ --

### 4.5. Structures

//TODO
cant have array fields

### 4.6. Input/output arrays
can only be passed to other functions and not allocated or created
const allowed
//TODO

### 4.7. Local and extern qualifiers

//TODO
object-local data or extern allocations
only allowed in main arguments

<br><br><br><br>

## 5. Built-in variables

//TODO
this.x this.y this.z
^ coordinates in current workspace

<br><br><br><br>

## 6. Built-in functions

//TODO

<br><br><br><br>

## 7. Preprocessor

//TODO

### 7.1. Comments

//TODO
// single line
/* multiline */

### 7.2. Headers

//TODO
.ilsh files
headers are copy pasted

### 7.3. Macros

//TODO
same as C but without stringify operator

### 7.4. Selection directives

//TODO
#ifdef #else #endif

### 7.5. Inline GLSL

//TODO
glsl(
    glsl_code();
    idk;
)

<br><br><br><br>

## 8. Compilation

//TODO
glsl output and c++ interfaces are generated automatically
the user has to pass the .ilsl file to the compiler and include the interface
> link to details

### 8.1. C++ interface

//TODO
#include d from `"Generated/ShaderCore/<shader_name>"`
must be included inside the C++ struct before anything else.
the struct has to run `core.start(<extern allocations...>)` in the first instruction of each constructor

//TODO add an image that shows how the interface works

### 8.2. ILSL/C++ data compatibility

//TODO


//TODO cheat sheet

| Operator                                                       | Precedence | Associativity | Class                                                                                  |
|----------------------------------------------------------------|:----------:|:-------------:|----------------------------------------------------------------------------------------|
| `(` `)`                                                        | 1          | -             | Grouping                                                                               |
| `[` `]`<br>`(` `)`<br>`.`<br>`++` `--`                         | 2          | Left to right | Array subscript<br>Function call<br>Structure field<br>Postfix increment and decrement |
| `++` `--`<br>`+` `-` `~`<br>`!`                                | 3          | Right to left | Prefix increment and decrement<br>Arithmetic unary<br>Logical not                      |
| `*` `/` `%`                                                    | 4          | Left to right | Multiplicative                                                                         |
| `+` `-`                                                        | 5          | Left to right | Additive                                                                               |
| `<<` `>>`                                                      | 6          | Left to right | Bitwise left and right shift                                                           |
| `<` `>` `<=` `>=`                                              | 7          | Left to right | Relational                                                                             |
| `==` `!=`                                                      | 8          | Left to right | Equality                                                                               |
| `&`                                                            | 9          | Left to right | Bitwise and                                                                            |
| `^`                                                            | 10         | Left to right | Bitwise xor                                                                            |
| `\|`                                                           | 11         | Left to right | Bitwise or                                                                             |
| `&&`                                                           | 12         | Left to right | Logical and                                                                            |
| `^^`                                                           | 13         | Left to right | Logical xor                                                                            |
| `\|\|`                                                         | 14         | Left to right | Logical or                                                                             |
| `?` `:`                                                        | 15         | Right to left | Ternary                                                                                |
| `=`<br>`+=` `-=` `*=` `/=` `%=`<br>`<<=` `>>=` `&=` `^=` `\|=` | 16         | Right to left | Assignment<br>Arithmetic assignment<br>Bitwise assignment                              |
| `,`                                                            | 17         | Left to right | List                                                                                   |


| Type                                                                                              | Size (bytes)               | Name                                                                                                                                                                   |                                          |
|---------------------------------------------------------------------------------------------------|:--------------------------:|------------------------------------------------------------------------------------------------------------------------------------------------------------------------|:----------------------------------------:|
| `b`          <br>`u32`            <br>`i32`            <br>`f32`            <br>`f64`             | 4<br>4<br>4<br>4<br>8      | Boolean scalar<br>32-bit unsigned integer scalar<br>32-bit signed integer scalar<br>32-bit float scalar<br>64-bit float scalar<br>                                     | ▅                                        |
| `bv2`        <br>`u32v2`          <br>`i32v2`          <br>`f32v2`          <br>`f64v2`           | 8<br>8<br>8<br>8<br>16     | Boolean 2D vector<br>32-bit unsigned integer 2D vector<br>32-bit signed integer 2D vector<br>32-bit float 2D vector<br>64-bit float 2D vector                          | ▅ ▅                                      |
| `bv3`        <br>`u32v3`          <br>`i32v3`          <br>`f32v3`          <br>`f64v3`           | 16<br>16<br>16<br>16<br>32 | Boolean 3D vector<br>32-bit unsigned integer 3D vector<br>32-bit signed integer 3D vector<br>32-bit float 3D vector<br>64-bit float 3D vector                          | ▅ ▅ ▅                                    |
| `bv4`        <br>`u32v4`          <br>`i32v4`          <br>`f32v4`          <br>`f64v4`           | 16<br>16<br>16<br>16<br>32 | Boolean 4D vector<br>32-bit unsigned integer 4D vector<br>32-bit signed integer 4D vector<br>32-bit float 4D vector<br>64-bit float 4D vector                          | ▅ ▅ ▅ ▅                                  |
| `bm2x2` `bm2`<br>`u32m2x2` `u32m2`<br>`i32m2x2` `i32m2`<br>`f32m2x2` `f32m2`<br>`f64m2x2` `f64m2` | 8<br>8<br>8<br>8<br>16     | 2x2 matrix of booleans<br>2x2 matrix of 32-bit unsigned integers<br>2x2 matrix of 32-bit signed integers<br>2x2 matrix of 32-bit floats<br>2x2 matrix of 64-bit floats | ▅ ▅<br>▅ ▅                               |
| `bm2x3`      <br>`u32m2x3`        <br>`i32m2x3`        <br>`f32m2x3`        <br>`f64m2x3`         | 8<br>8<br>8<br>8<br>16     | 2x3 matrix of booleans<br>2x3 matrix of 32-bit unsigned integers<br>2x3 matrix of 32-bit signed integers<br>2x3 matrix of 32-bit floats<br>2x3 matrix of 64-bit floats | ▅ ▅ ▅<br>▅ ▅ ▅                           |
| `bm2x4`      <br>`u32m2x4`        <br>`i32m2x4`        <br>`f32m2x4`        <br>`f64m2x4`         | 8<br>8<br>8<br>8<br>16     | 2x4 matrix of booleans<br>2x4 matrix of 32-bit unsigned integers<br>2x4 matrix of 32-bit signed integers<br>2x4 matrix of 32-bit floats<br>2x4 matrix of 64-bit floats | ▅ ▅ ▅ ▅<br>▅ ▅ ▅ ▅                       |
| `bm3x2`      <br>`u32m3x2`        <br>`i32m3x2`        <br>`f32m3x2`        <br>`f64m3x2`         | 16<br>16<br>16<br>16<br>32 | 3x2 matrix of booleans<br>3x2 matrix of 32-bit unsigned integers<br>3x2 matrix of 32-bit signed integers<br>3x2 matrix of 32-bit floats<br>3x2 matrix of 64-bit floats | ▅ ▅<br>▅ ▅<br>▅ ▅                        |
| `bm3x3` `bm3`<br>`u32m3x3` `u32m3`<br>`i32m3x3` `i32m3`<br>`f32m3x3` `f32m3`<br>`f64m3x3` `f64m3` | 16<br>16<br>16<br>16<br>32 | 3x3 matrix of booleans<br>3x3 matrix of 32-bit unsigned integers<br>3x3 matrix of 32-bit signed integers<br>3x3 matrix of 32-bit floats<br>3x3 matrix of 64-bit floats | ▅ ▅ ▅<br>▅ ▅ ▅<br>▅ ▅ ▅                  |
| `bm3x4`      <br>`u32m3x4`        <br>`i32m3x4`        <br>`f32m3x4`        <br>`f64m3x4`         | 16<br>16<br>16<br>16<br>32 | 3x4 matrix of booleans<br>3x4 matrix of 32-bit unsigned integers<br>3x4 matrix of 32-bit signed integers<br>3x4 matrix of 32-bit floats<br>3x4 matrix of 64-bit floats | ▅ ▅ ▅ ▅<br>▅ ▅ ▅ ▅<br>▅ ▅ ▅ ▅            |
| `bm4x2`      <br>`u32m4x2`        <br>`i32m4x2`        <br>`f32m4x2`        <br>`f64m4x2`         | 16<br>16<br>16<br>16<br>32 | 4x2 matrix of booleans<br>4x2 matrix of 32-bit unsigned integers<br>4x2 matrix of 32-bit signed integers<br>4x2 matrix of 32-bit floats<br>4x2 matrix of 64-bit floats | ▅ ▅<br>▅ ▅<br>▅ ▅<br>▅ ▅                 |
| `bm4x3`      <br>`u32m4x3`        <br>`i32m4x3`        <br>`f32m4x3`        <br>`f64m4x3`         | 16<br>16<br>16<br>16<br>32 | 4x3 matrix of booleans<br>4x3 matrix of 32-bit unsigned integers<br>4x3 matrix of 32-bit signed integers<br>4x3 matrix of 32-bit floats<br>4x3 matrix of 64-bit floats | ▅ ▅ ▅<br>▅ ▅ ▅<br>▅ ▅ ▅<br>▅ ▅ ▅         |
| `bm4x4` `bm4`<br>`u32m4x4` `u32m4`<br>`i32m4x4` `i32m4`<br>`f32m4x4` `f32m4`<br>`f64m4x4` `f64m4` | 16<br>16<br>16<br>16<br>32 | 4x4 matrix of booleans<br>4x4 matrix of 32-bit unsigned integers<br>4x4 matrix of 32-bit signed integers<br>4x4 matrix of 32-bit floats<br>4x4 matrix of 64-bit floats | ▅ ▅ ▅ ▅<br>▅ ▅ ▅ ▅<br>▅ ▅ ▅ ▅<br>▅ ▅ ▅ ▅ |
| `void`                                                                                            | -                          | Void                                                                                                                                                                   | -                                        |
//TODO add scope