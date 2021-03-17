---
title: CalculateAxisAngle (Node)
linkTitle: CalculateAxisAngle
description: Calculate angle between axes on two oriented sites
---

{{< htable >}}
| | |
|-|-|
|Context|Analysis|
|Name Required?|Yes|
|Branches|--|
{{< /htable >}}

## Overview

The `CalculateAxisAngle` node calculates the angle, in degrees, between specific axes on two sites $i$ and $j$, which are referenced from [`Select`]({{< ref "selectnode" >}}) nodes.

## Description

`CalculateAxisAngle` is one of several calculation nodes that determine and store a numerical result, ready for retrieval and use by other nodes in a procedure.

`CalculateAxisAngle` determines the angle formed between specific axes on two oriented sites $i$ and $j$, mapping the origins of both axes to {0,0,0}. The resulting value is measured in degrees.

## Configuration

### Control Keywords

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`I`|`name`|--|Site that represents $i$ in the angle $i$-$j$-$k$, taken from the `name`d [`Select`]({{< ref "selectnode" >}}) node.|
|`J`|`name`|--|Site that represents $j$ in the angle $i$-$j$-$k$, taken from the `name`d [`Select`]({{< ref "selectnode" >}}) node.|
