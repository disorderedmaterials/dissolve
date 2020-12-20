---
title: Expressions
description: Mathematical expression parsing
---

## Overview

Many node keywords accept mathematical expressions as input, rather than plain numerical values, allowing more complicated values to be calculated. This includes being able to reference named variables declared in, for instance, a [`Parameters`]({{< ref "parametersnode" >}}) node.

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
|`sin`|`x`|Return the sine of $x$, where $x$ is given in degrees.|
|`sqrt`|`x`|Return the square root of $x$.|
|`tan`|`x`|Return the tangent of $x$, where $x$ is given in degrees.|
