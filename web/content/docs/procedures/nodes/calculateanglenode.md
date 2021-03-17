---
title: CalculateAngle (Node)
linkTitle: CalculateAngle
description: Calculate angle between three sites
---

{{< htable >}}
| | |
|-|-|
|Context|Analysis|
|Name Required?|Yes|
|Branches|--|
{{< /htable >}}

## Overview

The `CalculateAngle` node calculates the angle, in degrees, between three provided sites $i$, $j$, and $k$, which are referenced from [`Select`]({{< ref "selectnode" >}}) nodes.

## Description

`CalculateAngle` is one of several calculation nodes that determine and store a numerical result, ready for retrieval and use by other nodes in a procedure.

`CalculateAngle` determines the angle $i$-$j$-$k$ between the selected sites $i$, $j$, and $k$, obeying minimum image distances if the encompassing box is periodic. The resulting value is measured in degrees.

## Configuration

### Control Keywords

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`I`|`name`|--|Site that represents $i$ in the angle $i$-$j$-$k$, taken from the `name`d [`Select`]({{< ref "selectnode" >}}) node.|
|`J`|`name`|--|Site that represents $j$ in the angle $i$-$j$-$k$, taken from the `name`d [`Select`]({{< ref "selectnode" >}}) node.|
|`K`|`name`|--|Site that represents $k$ in the angle $i$-$j$-$k$, taken from the `name`d [`Select`]({{< ref "selectnode" >}}) node.|
