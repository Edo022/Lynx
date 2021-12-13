 
# Intermediate Lynx Shading Language

## Table of contents

//TODO add links

- 1\. Overview
- 2\. Syntax
  - 2.1\. Identifiers
  - 2.2\. Variables
  - 2.4\. Scope
  - 2.5\. Functions
- 3\. Flow control
  - 3.1\. Logical operators
  - 3.2\. Selection statements
  - 3.4\. Ternary operator
  - 3.5\. While and for loops
  - 3.6\. Continue and break statements
  - 3.7\. Do-while
  - 3.8\. For-else
  - 3.9\. Return
- 4\. Types
  - 4.1\. Scalar types
  - 4.2\. Vectors
  - 4.3\. Matrices
  - 2.4\. Arithmetic and binary operators
  - 4.5\. Structures
  - 4.6\. Input/output arrays
  - 4.7\. Local and extern qualifiers
- 5\. Built-in variables
- 6\. Built-in functions
- 7\. Preprocessor
  - 7.1\. Comments
  - 7.2\. Headers
  - 7.3\. Macros
  - 7.4\. Selection directives
  - 7.5\. Inline GLSL
- 8\. Compilation
  - 8.1\. C++ interface
  - 8.2\. ILSL/C++ data compatibility

<br><br><br><br>

## 1. Overview

ILSL is a high level compiled shading language based on GLSL 4.5 and the C language syntax.<br>
A shader defines the appearence of a render object and is composed of one or more source files.<br>
This language is part of the Lynx SDK.

//TODO

<br><br><br><br>

## 2. Syntax

The syntax is almost identical to the C language syntax, except for some additional operators and constructs.

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

