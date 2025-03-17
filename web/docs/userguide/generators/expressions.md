---
title: Expressions
description: Mathematical expression parsing
---

## Overview

Many node keywords accept mathematical expressions as input, rather than plain numerical values, allowing more complicated values to be calculated. This includes being able to reference named variables declared in, for instance, a {{< node "Parameters" >}} node, as well as any parameters exposed by specific nodes. Provided a node is in scope at the point of use of the expression, available node parameters may be accessed by referencing the node name followed by a period (`.`) followed by the name of the parameter. For instance, [`Select`]({{< ref selectnode >}}) nodes expose a parameter containing the current number of sites selected, and which can be accessed (assuming the name of the node is "MySelect") as `MySelect.nSelected` in an expression.

## Type Conversion

The result of evaluating an expression is always either an integer or a floating-point value. If all of the operators and functions in the expression return integer types (or use only integer arguments) then the result of the expression will also be an integer. Otherwise, the result will be a floating-point value.

## Scope

As well as the operators and functions listed below, parentheses (`(` and `)`) can be used freely to enforce the correct precedence of operations in expressions.

### Unary Operators

|Operator|Description|
|:------:|-----------|
|`-`|Negate, e.g. `-4`, `-(8.4*10)`|

### Binary Operators

Listed in order of decreasing precedence.

|Operator|Description|
|:------:|-----------|
|`^`|Power (e.g. `4^2 = 16`, `3^3 = 27`)|
|`*`|Multiply|
|`/`|Divide|
|`+`|Add|
|`-`|Subtract|

### Functions

|Function|Arguments|Description|
|:------:|:-------:|-----------|
|`abs`|`x`|Return the absolute value of $x$.|
|`acos`|`x`|Return the arc cosine of $x$ (in degrees).|
|`asin`|`x`|Return the arc sine of $x$ (in degrees).|
|`atan`|`x`|Return the arc tangent of $x$ (in degrees).|
|`cos`|`x`|Return the cosine of $x$, where $x$ is given in degrees.|
|`exp`|`x`|Return the exponential function of $x$.|
|`ln`|`x`|Return the natural logarithm of $x$.|
|`log`|`x`|Return the base-10 logarithm of $x$.|
|`pi`|`--`|Return the the value of Pi.|
|`sin`|`x`|Return the sine of $x$, where $x$ is given in degrees.|
|`sqrt`|`x`|Return the square root of $x$.|
|`tan`|`x`|Return the tangent of $x$, where $x$ is given in degrees.|
|`twopi`|`--`|Return the the value of 2Pi.|
|`random`|`--`|Return a random value between 0.0 and 1.0 inclusive.|
|`randomi`|`N`|Return a random integer between 0 and `N-1` inclusive.|
