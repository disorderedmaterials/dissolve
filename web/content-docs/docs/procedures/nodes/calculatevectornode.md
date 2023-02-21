---
title: CalculateVector (Node)
linkTitle: CalculateVector
description: Calculate a vector between two sites
---

{{< htable >}}
| | |
|-|-|
|Context|Analysis|
|Name Required?|Yes|
|Branches|--|
{{< /htable >}}

## Overview

The `CalculateVector` node calculates the vector between two provided sites $i$ and $j$, which are referenced from {{< gui-node "Select" >}} nodes.

## Description

`CalculateVector` is one of several calculation nodes that determine and store a numerical result, ready for retrieval and use by other nodes in a procedure.

`CalculateVector` determines the vector $v_{ij}$ in 3D space between the selected sites $i$ and $j$, obeying the minimum image convention if the encompassing box is periodic. The resulting value is measured in Angstroms.

## Options

### Sites

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`I`|{{< gui-node "Select" >}}|--|Site that represents $i$ in the distance $i$-$j$, taken from the named {{< gui-node "Select" >}} node.|
|`J`|{{< gui-node "Select" >}}|--|Site that represents $j$ in the distance $i$-$j$, taken from the named {{< gui-node "Select" >}} node.|

### Control

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`RotateIntoFrame`|`bool`|`false`|Whether to rotate the calculated vector into the local frame defined on $i$. If `true`, and the site $i$ has a set of axes defined, the vector is rotated into the frame of those axes.|
