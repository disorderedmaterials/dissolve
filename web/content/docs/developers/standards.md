---
title: Coding Standards
description: Code and style standards employed by the project
---

## Introduction

### Purpose
This document describes the coding standards and stylistic choices for the Dissolve project.

### Scope
This document is essential reading for any developer or power-user who wishes to contribute to the Dissolve codebase.

## Overview

Dissolve is written entirely in C++. The following standards and style requirements apply specifically to C++ source files.

## Style

Note that the format of contributed code is strictly imposed - pull requests are subject to code quality checks, and will not be merged until all such tests have passed.

### General
- Indentation shall be made using spaces, not tabs.
- Class names and namespaces shall be written in upper camel case (e.g. `ThisIsAClass`).
- Public and private member variables and functions shall be written in lower camel case (e.g. `thisIsAFunction()`).
- clang-format should be used to ensure correct formatting of the code (a suitable `.clang-format` file is provided).

### Classes and Namespaces
- There should be only one class or namespace declared in each header file, except where it makes sense to group multiple closely-related, small classes together.
- There should be only one class or namespace defined per body file (unless classes are closely related as described above). All definitions for that class/namespace should be contained in one file, except for larger classes where definitions may be grouped into separate files in order to reduce single-file size.
- Data members should be `private` or `protected` and suffixed with an underscore (e.g. `aPrivateInteger_`) in order to differentiate them from public / local function variables.
- Access to data from other classes should only be through `protected` or `public` methods.
- All constructors for a class must initialise all its member variables that do not have a default constructor (including primitive and pointer types).
- The use of `friend` should be avoided and its use requires justification.
- Unions and bitfields should only be used where essential.

### Functions and Variables
- Variables, functions parameters, and function return values must have explicit types.
- A function declaration should not use `void` to indicate an empty parameter list.
- Parameters in function prototypes should include names, not just their types. For example, use `void eraseRange(int nFirst, int nLast);` rather than `void eraseRange(int, int);` as this improves self-documentation.
- Non-static member functions should be declared `const` if logically they do not alter the state of the class instance.
- Operators should be overloaded sparingly. Operator overloads should behave in accordance with the semantics of the operator as applied to primitive types, or according to established conventions (e.g. `<<` and `>>` for serialisation).
- ‘Magic numbers’ must not be used in the code. Constants and enumerations must be used instead.

### Expressions and Statements
- Integers should not be cast to Booleans. For example, prefer `if (x != 0)` rather than `if(x)`
- C-style type casts should not be used. If casting up or down an inheritance hierarchy, use `dynamic_cast` (which performs a run-time type check) rather than `static_cast`.
- Function calls with side effects, and the `++`/`--`/assignment operators, should only be called as a standalone statement rather than embedded inside an expression.
- It is permissible, although discouraged, to have a function call with side effects as the right-operand of `&&` or `||`. Any such instances must be commented in detail to alert other developers to the fact that the function is not always called.
- A `for` loop should only have one control variable, and should not modify it in the body.
- `switch` statements must include a `default` clause, even if only to catch errors.
- Each case of a `switch` statement must either end with a `break`/`return`, or contain a clear comment to alert other developers to the fact that execution will fall through to the next case. Multiple `case` labels (with no code between them) are, however, permitted for the same block of code.
- `goto` must be avoided. When there is a need to break out of two or more nested loops in one go, the loops should be moved to a separate function where `return` can be used instead.

## Comments
- Comments must be indented to the same level as the code to which they refer.
- The language used in comments must be professional and to the point. Humor is permitted in small measure.
- The collection of comments in a function must, on its own, be sufficient that a competent C++ developer can pick up the function for subsequent modification / correction.
- Comments on a single line should use `//` rather than `/* ... */`.
- No code block should exceed 20 statement lines without a comment of some sort. In general all code should contain 15% comment lines.
- The style of the comments is not mandated here. However the following are general recommendations:
	- Comments should always be used to describe potential “difficult” sections of code - e.g. those implementing specific mathematical operations.
	- Comments should be used in particular to explain branch conditions in `if ... else` and `switch { case ...`-like statements.
	- Comments should be written at a higher level of abstraction than the code to which they pertain, rather than merely restating it.
- ‘Dead code’ (e.g. large commented blocks) must not be kept in the source.

### External Dependencies
- Dependence on external libraries should be kept to an absolute minimum, only being used where significant functional or maintainability benefits are gained.
- Classes from external libraries relevant to the GUI must **not** be used outside of the GUI-related aspects of the code, as doing so taints the non-GUI build of the code with unnecessary dependencies.
