---
title: AxisAngle (Node)
linkTitle: AxisAngle
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

The `AxisAngle` node calculates the angle, in degrees, between specific axes on two sites $i$ and $j$, which are referenced from {{< node "Select" >}} nodes.

## Description

`AxisAngle` is one of several calculation nodes that determine and store a numerical result, ready for retrieval and use by other nodes in a procedure.

`AxisAngle` determines the angle formed between specific axes on two oriented sites $i$ and $j$, mapping the origins of both axes to {0,0,0}. The resulting value is measured in degrees.

## Options

### Sites

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`I`|{{< node "Select" >}}|--|Site that represents $i$ in the angle $i$-$j$-$k$, taken from the named {{< node "Select" >}} node.|
|`J`|{{< node "Select" >}}|--|Site that represents $j$ in the angle $i$-$j$-$k$, taken from the named {{< node "Select" >}} node.|
|`AxisI`|`X|Y|Z`|`X`|Axis to use from site I|
|`AxisJ`|`X|Y|Z`|`X`|Axis to use from site J|

### Control

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Symmetric`|`bool`|`false`|Whether to consider angles as symmetric about 90, mapping all angles to the range 0 - 90.|
