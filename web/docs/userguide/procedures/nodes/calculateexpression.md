---
title: CalculateExpression (Node)
linkTitle: CalculateExpression
description: Calculate distance between two sites
---

{{< htable >}}
| | |
|-|-|
|Context|Analysis|
|Name Required?|Yes|
|Branches|--|
{{< /htable >}}

## Overview

The `CalculateExpression` node calculates the result of an arbitrary mathematical [expression]({{< ref expressions >}}).

## Description

`CalculateExpression` is one of several calculation nodes that determine and store a numerical result, ready for retrieval and use by other nodes in a procedure.

`CalculateExpression` returns the result of evaluating its defined expression.

## Options

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Expression`|`<expression>`|--|The expression to evaluate.|
