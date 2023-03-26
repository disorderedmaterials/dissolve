---
title: Angle (Node)
linkTitle: Angle
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

The `Angle` node calculates the angle, in degrees, between three provided sites $i$, $j$, and $k$, which are referenced from {{< node "Select" >}} nodes.

## Description

`Angle` is one of several calculation nodes that determine and store a numerical result, ready for retrieval and use by other nodes in a procedure.

`Angle` determines the angle $i$-$j$-$k$ between the selected sites $i$, $j$, and $k$, obeying minimum image distances if the encompassing box is periodic. The resulting value is measured in degrees.

## Options

### Sites

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`I`|{{< node "Select" >}}|--|Site that represents $i$ in the angle $i$-$j$-$k$, taken from the named {{< node "Select" >}} node.|
|`J`|{{< node "Select" >}}|--|Site that represents $j$ in the angle $i$-$j$-$k$, taken from the named {{< node "Select" >}} node.|
|`K`|{{< node "Select" >}}|--|Site that represents $k$ in the angle $i$-$j$-$k$, taken from the named {{< node "Select" >}} node.|

### Control

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Symmetric`|`bool`|`false`|Whether to consider angles as symmetric about 90, mapping all angles to the range 0 - 90.|
