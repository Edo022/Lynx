# Lux - Lynx shading language

<style>
    .h {
        color: gray;
}
</style>

Lux is a <b>high level compiled shading language</b> based on <b>GLSL 4.6</b>, written to allow developers to efficiently create shaders without going through the laborious work of manually managing arrays and interfaces.

The language features <b>C-like syntax</b> and semantics.<br>
Additional constructs and keywords allow for better abstraction on data types and a more direct approach to flow control and management of multidimensional data.

Lux is meant to be used alongside Umbra, the main programming language of the Lynx Engine, as it is highly compatible and interfacing shaders requires little to no effort.

Lux is part of the <b>Lynx SDK</b>.

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
+ [2.2. Scopes and symbols](#scopes-and-symbols)
+ [2.3. Namespaces](#namespaces)
+ [2.4. Types](#types)
  + [2.4.1. Primitive types](#primitive-types)
  + [2.4.2. Enums](#enums)
  + [2.4.3. Structs](#structs)
  + [2.4.4. Arrays](#arrays)
  + [2.4.6. Implicit conversions](#implicit-conversions)
  + [2.4.7. Auto specifier](#auto-specifier)
  + [2.4.8. Typeof and baseof specifiers](#typeof-and-baseof-specifiers)
+ [2.6. Functions](#functions)
  + [2.6.1. Parameters and references](#parameters-and-references)
  + [2.6.2. Return statement](#return-statement)
  + [2.6.3. Template functions](#template-functions)
  + [2.6.4. Type classes](#type-classes)
+ [2.7. Const qualifier](#const-qualifier)
+ [2.8. Symbol aliases](#symbol-aliases)

<br>

## 3. Runtime statements

+ [3.1. Values](#values)
  + [3.1.1. Left and right values](#variables)
  + [3.1.2. Literals](#literals)
  + [3.1.3. Temporary variables](#temporary-variables)
  + [2.4.5. Constructors](#constructors)
+ [3.2. Operators](#operators)
  + [3.2.1 Arithmetic operators](#arithmetic-operators)
  + [3.2.2 Binary operators](#binary-operators)
  + [3.2.3 Logical operators](#logical-operators)
  + [3.2.4 Ternary operator](#ternary-operator)
  + [3.2.5 Grouping](#grouping)
  + [3.2.6 Array operations](#array-operations)
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

<br>

## 4. Compilation

+ [5.1. Modules](#modules)
+ [5.1. Compilation phases and error types](#compilation-phases-and-error-types)
+ [5.2. Variables initialization order](#variable-initialization-order)
+ [5.3. Branching performance](#branching-performance)
+ [5.4. Compiler optimizations](#compiler-optimizations)

<br>

## 5. Standard modules

+ [4.1. Built-in functions](#built-in-functions) //TODO

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

<br><br>

## Comments

Comments are portions of code that are purposely ignored.<br>
They can be used to provide informations or temporarily exclude lines of code.<br>
Comments are allowed to contain any sequence of unicode characters of any length.<br>
//TODO

Single line comments start with `//` and end at the first newline character<br>
Multiline comments start with `/*` and end with `*/`

<h5>Syntax<h5>
<pre>//<kbd>Comment string</kbd><span class=h>↩</span></pre>
<pre>/*<kbd>Comment string</kbd>*/</pre>

<h5>Example<h5>

```c
// Single line comment

/* Multi
   line 
   comment */
```

<br><br>

## Include statements

Include statements are used to <b>paste code</b> from other files.<br>
Any existent file can be included, regardless of its contents or extension.<br>
The path to the included file must be enclosed in `"` `"` and can only contain alphanumeric characters, dots and slashes. //TODO <br>
Both <b>absolute</b> and <b>relative</b> paths are supported.

<h5>Syntax</h5>
<pre>#include<span class=h>·</span>"<kbd>absolute/path/to/file</kbd>"<span class=h>↩</span></pre>
<pre>#include<span class=h>·</span>"./<kbd>relative/path/to/file</kbd>"<span class=h>↩</span></pre>
<pre>#include<span class=h>·</span>"<kbd>relative/path/to/file</kbd>"<span class=h>↩</span></pre>

<h5>Example</h5>

```c
// Ok
#include "/home/Lynx/Lux/file.ilsh"
#include "./file.ilsh"
#include "file.ilsh"

// Error
//#include ./file.ilsh
//#include "./non_existent_file"
//#include ""
```

<br><br>

## Macros

<br><br>

## Conditionals

<br><br>

## Line continuation token

The line continuation token can be used to merge multiple lines and allow single line comments, preprocessor directives and multi character tokens to be written across multiple lines.<br>
It consists of a `\` <b>immediatly</b> followed by a <b>newline character</b>.

<h5>Syntax</h5>
<pre>\<span class=h>↩</span></pre>

<h5>These are all allowed</h5>

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

---

<br><br><br><br>

# Language components

<br><br><br><br>

## Tokens and identifiers

Any sequence of one or more characters that is left after the preprocessing phase and is recognizable according to the grammar of the language is called a token.<br>
Comments and whitespace characters can be freely written between two tokens without altering the GLSL output.<br>
Tokens are always <b>case sensitive</b>.<br>
Comments, newline characters, whitespace characters and preprocessor directives are <b>not tokens</b>.

Each token can be classified as one of the following:

| Type                                                 | Pattern                                                                                        | Example                                            |
|------------------------------------------------------|------------------------------------------------------------------------------------------------|----------------------------------------------------|
| Identifier                                           | Any sequence of `alphanumeric characters`, `$` and `_` that doesn't start with a decimal digit | `Var_1` `function` `$0` `NOT$A$MACRO`              |
| Literal                                              | Any sequence of `alphanumeric characters` and `.` that start withs a decimal digit             | `0x12a4bc` `266250` `0b1010001` `0invalid_literal` |
| Keywords                                             | Predefined alphabetic identifiers                                                              | `for` `if` `namespace` `then`                      |
| Operator                                             | Predefined sequences of symbols                                                                |  `=` `<<=` `+` `~`                                 |
| Function arguments, scope and constructor delimiters | `(` `)` `{` `}` `[` `]`                                                                        |                                                    |
| Expression, statement, type and scope separators     | `,` `;` `\|` `::`                                                                              |                                                    |
| Reference token                                      | `&`                                                                                            |                                                    |

<br>

Identifiers are used to name functions, variables, structs, aliases, type classes and namespaces.<br>
They must be unique to the current scope and not overlap with keywords.<br>
Temporary variables are the only identifiers that are allowed to start with a `$`.

See [Literals](#literals), [Operators](#operators), [Scopes](#scopes)

<br><br><br><br>

## Scopes and symbols

A symbol is any identifier that can be referenced unambiguously through its absolute name.

Scopes define which <b>symbols</b> the code can <b>reference</b> and the absolute and relative names of each of them.<br>
Any portion of code enclosed in `{` `}` defines a new scope. This includes `functions`, `structs`, `namespaces`, `unnamed scopes` and `runtime statements`.<br>
The only exception is the <b>module scope</b>. It contains all the symbols and scopes declared in the module and can be referenced using the `this` keyword.

`Namespaces`, `Enums`, `Structs` and `Symbol aliases` can be <b>declared</b> in <b>any scope</b>.<br>
Other identifiers are only allowed to be declared in certain scopes, following this table:

| Scope type        | Can contain                                                         | Can be declared in                  |
|-------------------|:-------------------------------------------------------------------:|:-----------------------------------:|
| Module            | `Functions`<br><i>`Variables`<br>`Type classes`</i>                 | -                                   |
| Namespace         | `Functions`<br><i>`Variables`<br>`Type classes`</i>                 | Any scope                           |
| Enum              | <i>`Enum elements`</i>                                              | Any scope                           |
| Struct            | <i>`Struct fields`</i>                                              | Any scope                           |
| Function          | `Runtime statements`<br><i>`Variables`<br>`Function parameters`</i> | Module scope                        |
| Runtime statement | `Runtime statements`<br><i>`Variables`<br></i>                      | `Functions`<br>`Runtime statements` |

<br>

Local names are used to reference identifiers declared in the current scope or one of the enclosing scopes.<br>
Relative names, instead, can reference any symbol declared in the module. They consist of the names of the enclosing scopes and the identifier of the symbol separated by `::` tokens.<br>
A relative name that starts from the module scope is called an absolute name.

Identifiers declared in unnamed scopes cannot be referenced from outside.<br>
Declaring a symbol with the same identifier as one in an enclosing scope will shadow the old identifier, and any use of the identifier in the current and sub scopes will refer to the new symbol.<br>
Declaring the same identifier more than once in the same scope is only allowed for function overloading. Anything else is a semantic error.<br>

<h5>Example</h5>

```c
// Module scope
// Local name: this


// Local name: s
// Relative names: this::s
struct s { u32 n; }

// Unnamed, cannot be referenced from outside
namespace {
    // Local name: fun
    void fun(u32 n) {}
}


// Local name: fun
// Relative names: this::fun
void fun(u32 n) {
    // Unnamed, cannot be referenced from outside
    if(n){
        // Local name: s2
        struct s2 { u32 n; };
    }

    // Local name: s, shadows this::s
    // Relative names: this::fun::s, fun::s
    struct s { u32 a, b; }
}
```

See [Namespaces](#namespaces), [Variables](#variables), [Functions](#functions), [Structs](#structs), [Enums](#enums), [Selection](#selection), [Iteration](#iteration), [Else statement](#else-statement), [Symbol aliases](#symbol-aliases)

<br><br><br><br>

## Namespaces

Namespaces are used to create named scopes which can contain variables, functions, enums, structs and symbol aliases.<br>
They don't affect the runtime behaviour and can be declared in any scope.<br>
Unnamed namespaces are allowed.

<h5>Syntax</h5>
<pre>
namespace<span class=h>·</span><kbd>namespace identifier</kbd> {
    <kbd>...</kbd>
}
</pre>
<pre>
namespace {
    <kbd>...</kbd>
}
</pre>

See [Variables](#variables), [Functions](#functions), [Structs](#structs), [Enums](#enums), [Symbol aliases](#symbol-aliases)

<br><br><br><br>

## Types

Data types define the way runtime values are interpreted and how they can be used within expressions.<br>
Lux features five primitives types, which can be used to define more complex types such as structs, arrays and enums.

Every value has an associated type, regardless of it being a right or a left value.<br>
Like any symbol, declearing a new type or other symbols will shadow the ones that were declared in the enclosing scopes using the same identifier.<br>
Primitive types can also be shadowed and they behave as if they were declared in the module namespace.

<br>

### Primite types

The primitive types are:

| Lux type  | Actual GLSL type | C equivalent | Description                      |
|-----------|------------------|--------------|----------------------------------|
| `u32`     | uint             | unsigned int | 32 bit unsigned integer          |
| `i32`     | int              | int          | 32 bit signed integer            |
| `f32`     | float            | float        | 32 bit floating point            |
| `f64`     | double           | double       | 64 bit floating point            |
| `b`       | bool             | -            | Boolean value. `true` or `false` |

<br>

As in most programming languages, the boolean `true` and `false` values correspond to the integer literals `1` and `0` and they can be used interchangeably.<br>
Unsigned integers can't hold negative values, but the maximum value is twice as high as their signed counterpart.<br>
There are no 64 bit integers as they aren't a standard type in GLSL.

See [Values](#values), [Literals](#literals), [Implicit conversions](#implicit-conversions)

<br>

### Enums

Enums are used to name and group literal constants of the same type.<br>
The base type of the enum can be specified after its name and it must be one of the primitive types.<br>
Elements are declared in the same way as variables, but neither the type nor the const qualifier are specified explicitly.<br>
The values of each variable are implicitly converted to the base type of the enum.

Elements of integer enums which are not initizlied will have the value of the preceeding element + 1, or 0 if they are the first.<br>
Elements of bool and float enums must always be initialized.<br>
Multiple elements can have the same value, but each element can only be defined once.<br>
Enums with no elements are not allowed.

Enums can be referenced before their definition.<br>
It is allowed to declare structures, enums and symbol aliases inside an enum.<br>
The name of the enum can be used as a type to prevent unknown values to be saved in it.

<h5>Syntax</h5>
<pre>
enum<span class=h>·</span><kbd>symbol identifier</kbd> : <kbd>base type</kbd> {
    <kbd>element identifier</kbd> = <kbd>(base type) expression</kbd>;
    <kbd>...</kbd>
}
</pre>

<h5>Example</h5>

```c
enum : u32 {                    // Ok, unnamed enum
    value_1,        value_2;    // Ok, value_1 = 0,    value_2 = 1
    value_3 = 9999, value_4;    // Ok, value_3 = 9999, value_4 = 10000

    struct s {
        u32 n1 = value_1;       // Ok
        u32 n3 = 5;             // Ok
        //e   n4 = 5;           // Error, unknown this::e value
    }

    value_5;                    // Ok, value_5 = 10001
}

enum c : f64 {
    pi          = 3.14159;      // Ok
    e           = 2.71828,      // Ok
    another_e,  = 2.71828;      // Ok
    another_e_2 = e;            // Ok
    another_e_3 = this::c::e;   // Ok, used absolute name
    //invalid_val;              // Error, uninitialized f64 enum value
}
```

<br>

When an enum doesn't have to be explicitly referenced more than once, unnamed enums can be used to define both the type and the symbol in the same construct.<br>
Similarly to the C syntax, the name is not specified and the enum definition is used as type of the symbol.

The elements and the enum can still be referenced through the `typeof` specifier.<br>
Defining an unnamed enum without declaring a symbol is allowed, but it will generate a warning as it its elements are unaccessible and the symbol hiding mechanic can be achieved through unnamed namespaces.

<h5>Syntax</h5>
<pre>
enum : <kbd>base type</kbd> {
    <kbd>element identifier</kbd> = <kbd>(base type) expression</kbd>;
    <kbd>...</kbd>
} <kbd>symbol identifier</kbd>;
</pre>

<h5>Example</h5>

```c
enum : b {
    on = true;
    off = false;
} grasses;

void main(){
    self.grasses = typeof(grasses)::on;
}
```

See [Typeof and baseof specifiers](#typeof-and-baseof-specifiers)

<br>

### Structs

Structs are used to group multiple values of different types.<br>
Elements are declared in the same way as variables, and they can be of any type, including enums, arrays and other structures.

Structs can be referenced before their definition.<br>
It is allowed to declare structures, enums and symbol aliases inside a struct.<br>
Struct fields are referenced using the `.` token. They cannot be const and cannot have an initializer value.<br>
Structs with no elements are not allowed.

<h5>Syntax</h5>
<pre>
struct<span class=h>·</span><kbd>symbol identifier</kbd> {
    <kbd>field type</kbd><span class=h>·</span><kbd>field identifier</kbd>;
    <kbd>...</kbd>
}
</pre>
<pre><kbd>symbol identifier</kbd>.<kbd>field identifier</kbd></pre>

<h5>Example</h5>

```c
struct idk {
    u32 a, b;
    f64 c;
}

void main(){
    idk var;
    var.a = 4;
    var.b = var.a * 4;
}
```

<br>

Like enums, structs can also be unnamed and their symbol and fields types can be referenced through `typeof`.<br>
Defining unnamed structs without a symbol declaration will also generate a warning.

<h5>Syntax</h5>

<pre>
struct {
    <kbd>field type</kbd><span class=h>·</span><kbd>field identifier</kbd>;
    <kbd>...</kbd>
} <kbd>symbol identifier</kbd>;
</pre>

<h5>Example</h5>

```c
struct {
    u32 a, b;
    enum {
        idk1;
        idk2;
    } c;
} var;

void main(){
    // Both are allowed
    var.c = typeof(var.c)::idk1;
    var.c = typeof(typeof(var)::c)::idk2;

    typeof(var.a) _a = 0;
}
```

See [Arrays](#arrays), [Typeof and baseof specifiers](#typeof-and-baseof-specifiers)

<br>

### Arrays

Arrays can aggregate values of the same type.<br>
Functions, parameters, right values, variables and struct fields can all be of an array type.

Arrays can use any base type, including structs, enums and other arrays.<br>
An array of arrays is called a multidimensional array. There is no limit to the number of dimensions an array can have.<br>

Array declarations are identical to the normal types, but `[` `]` tokens follow the symbol identifier, optionally enclosing an expression to specify the number of elements.<br>
Arrays can be initialized using any expression of a type that can be implicitly converted.<br>
If the `[` `]` are left empty, the number of elements is determined by the inizializer value, which becomes required and must be of an implicitly convertible array type.

There are no dynamic arrays.

<h5>Syntax</h5>
<pre><kbd>base type</kbd><span class=h>·</span><kbd>symbol identifier</kbd>[<kbd>(u32) expression</kbd>];</pre>
<pre><kbd>base type</kbd><span class=h>·</span><kbd>symbol identifier</kbd>[<kbd>(u32) expression</kbd>] = <kbd>(base type[]) expression</kbd>;</pre>
<pre><kbd>base type</kbd><span class=h>·</span><kbd>symbol identifier</kbd>[] = <kbd>(base type[]) expression</kbd>;</pre>

<h5>Example</h5>

```c
u32 arr1[4] = (1, 2, 3, 4);     // Ok, arr1 = (1, 2, 3, 4)
u32 arr2[2][2] = (1, 2, 3, 4);  // Ok, arr2 = ((1, 2), (3, 4)). Explained in Constructors
u32 arr3[2][2] = arr1;          // Ok, arr3 = ((1, 2), (3, 4)). Explained in Constructors
u32 arr4[5] = 1;                // Ok, arr4 = (1, 1, 1, 1, 1)
u32 arr5[] = (8, 7, 6);         // Ok, arr5 = (8,  7,  6)
u32 arr6[] = arr4 * 2;          // Ok, arr6 = (16, 14, 12)

//u32 arr1[4] = (1, 2, 3);      // Error, inizializer cannot convert as it doesn't have enough elements
//u32 arr1[]  = 1;              // Error, cannot determine the array size
```

<br>

<h5>Syntax</h5>
<h5>Example</h5>

See [Primitive types](#primitive-types), [Implicit conversions](#implicit-conversions), [Constructors](#constructors)

<br>

### Implicit conversions

Implicit conversions are performed when an expression of the incorrect type is used in place of the expected one.<br>
In function overloads and templates, the actual type of the value always takes precedence over conversions.

Primitive types are all implicitly convertible between them.<br>
The conversions follow these rules:

<table>
    <tr>
        <th>Expression type</th>
        <th>Expected type</th>
        <th>Behaviour</th>
    </tr>
    <tr>
        <td rowspan="3"><code>u32</code></td>
        <td><code>i32</code></td>
        <td>Values over <code>limits::i32::max</code> overflow to <code>limits::i32::min&nbsp;+&nbsp;n&nbsp;-&nbsp;1</code></td>
    </tr>
    <tr>
        <td><code>f32</code><code>f64</code></td>
        <td>No changes</td>
    </tr>
    <tr>
        <td><code>b</code></td>
        <td><code>1</code> is evaluated as <code>true</code>, any other value as <code>false</code></td>
    </tr>
    <tr>
        <td rowspan="3"><code>i32</code></td>
        <td><code>u32</code></td>
        <td>Negative values underflow to <code>limits::u32::max&nbsp;+&nbsp;n&nbsp;+&nbsp;1</code></td>
    </tr>
    <tr>
        <td><code>f32</code><code>f64</code></td>
        <td>No changes</td>
    </tr>
    <tr>
        <td><code>b</code></td>
        <td><code>1</code> is evaluated as <code>true</code>, any other value as <code>false</code></td>
    </tr>
    <tr>
        <td rowspan="4"><code>f32</code></td>
        <td><code>u32</code></td>
        <td>The decimal part is truncated. Negative values underflow to <code>limits::u32::max&nbsp;+&nbsp;n&nbsp;+&nbsp;1</code>.<br>
        If the value is not between <code>limits::u32::min</code> and <code>limits::u32::max</code>, the result is <b>undefined</b></td>
    </tr>
    <tr>
        <td><code>i32</code></td>
        <td>The decimal part is truncated.<br>
        If the value is not between <code>limits::i32::min</code> and <code>limits::i32::max</code>, the result is <b>undefined</b></td>
    </tr>
    <tr>
        <td><code>f64</code></td>
        <td>No changes</td>
    </tr>
    <tr>
        <td><code>b</code></td>
        <td><code>1.0</code> is evaluated as <code>true</code>, any other value as <code>false</code></td>
    </tr>
    <tr>
        <td rowspan="4"><code>f64</code></td>
        <td><code>u32</code></td>
        <td>The decimal part is truncated. Negative values underflow to <code>limits::u32::max&nbsp;+&nbsp;n&nbsp;+&nbsp;1</code>.<br>
        If the value is not between <code>limits::u32::min</code> and <code>limits::u32::max</code>, the result is <b>undefined</b></td>
    </tr>
    <tr>
        <td><code>i32</code></td>
        <td>The decimal part is truncated.<br>
        If the value is not between <code>limits::i32::min</code> and <code>limits::i32::max</code>, the result is <b>undefined</b></td>
    </tr>
    <tr>
        <td><code>f32</code></td>
        <td>The result is approximated using the maximum precision available</td>
    </tr>
    <tr>
        <td><code>b</code></td>
        <td><code>1.0</code> is evaluated as <code>true</code>, any other value as <code>false</code></td>
    </tr>
    <tr>
        <td rowspan="3"><code>b</code></td>
        <td><code>u32</code><code>i32</code></td>
        <td><code>true</code> is evaluated as <code>1</code>, <code>false</code> as <code>0</code></td>
    </tr>
    <tr>
        <td><code>f32</code><code>f64</code></td>
        <td><code>true</code> is evaluated as <code>1.0</code>, <code>false</code> as <code>0.0</code></td>
    </tr>
</table>

<br>

Additional conversions between arrays and primitive types are described in the following table,<br>
with `T` and `U` representing two different types of any kind:

| Expression type | Expected type | Behaviour                                                                                                               |
|-----------------|---------------|-------------------------------------------------------------------------------------------------------------------------|
| `T`             | `U[n]`        | Each element of the result is initialized with the value of the starting data, converted from `T` to `U`                |
| `T[n]`          | `U[n]`        | Each element of the result is initialized with the corresponding value of the starting array, converted from `T` to `U` |
| `T[n]`          | `b`           | Evaluated as `true` if all the elements of the initial array converted from `T` to `b` evaluate as `true`, `false` otherwise         |

<br>

Values cannot be explicitly casted as in C. To convert a value, constructors are used.<br>
Attempting to convert incompatible types will result in a semantic error.<br>
Structs are not convertible.

<h5>Example</h5>

```c
void fun(u32 a[5]) {}
void fun2(b a) {}

void main(){
    struct { u32 n; } var = (0);
    //fun(var);                               // Error, structs cannot be converted
    //fun(f32[2](1.0, 4.4));                  // Error, arrays have different lengths
    fun(2);                                   // Ok, a = (2, 2, 2, 2, 2)
    fun(f32[5](1.0, 4.4, 5.5, -33, 2.11));    // Ok, a = (1, 4, 5, 4294967263, 2)

    fun2(2);                                  // Ok, a = true
    fun2(f32[5](1.0, 4.4, 5.5, -33, 2.11));   // Ok, a = true
    fun2(f32[5](1.0, 4.4, 5.5, -33, 0   ));   // Ok, a = false

    u32 arr[1.2];                             // Ok, arr has 1 element
}
```

<br>

See [Arrays](#arrays), [Constructors](#constructors), [Standard modules](#standard-modules)

<br>

### Auto specifier



<br>

### Typeof and baseof specifiers

<br><br><br><br>

## Functions

<br>

### Parameters and references

<br>

### Return statement

<br>

### Template functions

<br>

### Type classes

<br><br><br><br>

## Const qualifier

<br><br><br><br>

## Symbol aliases

<br><br><br><br>

---

<br><br><br><br>

# 3. Runtime statements

<br><br><br><br>

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
type name(type arg_identifier, ...) {
    statement;
    ...
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

### Parameters and references

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

# Modules

Althought the file extension is only a convention and can be changed or omitted, it is still recommended to use the correct one, as some softwares may use it to determine the language of the source file.

<br>

## Compilation phases and error types

<br>

## Selection directives performance

<br>

## Compiler optimizations

<br><br><br><br>

# Quick reference tabs

//TODO
//this.x this.y this.z
this::pos.x
this::pos.y
this::pos.z
^ coordinates in current workspace




sequence function //TODO
sequence(start, end, step)