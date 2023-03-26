---
title: CalculateDistance (Node)
linkTitle: CalculateDistance
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

The `CalculateDistance` node calculates the distance, in Angstroms, between two provided sites $i$ and $j$, which are referenced from {{< node "Select" >}} nodes.

## Description

`CalculateDistance` is one of several calculation nodes that determine and store a numerical result, ready for retrieval and use by other nodes in a procedure.

`CalculateDistance` determines the distance $i$-$j$ between the selected sites $i$ and $j$, obeying the minimum image convention if the encompassing box is periodic. The resulting value is measured in Angstroms.

## Options

### Sites

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`I`|{{< node "Select" >}}|--|Site that represents $i$ in the distance $i$-$j$, taken from the named {{< node "Select" >}} node.|
|`J`|{{< node "Select" >}}|--|Site that represents $j$ in the distance $i$-$j$, taken from the named {{< node "Select" >}} node.|
