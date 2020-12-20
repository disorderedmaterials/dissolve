---
title: Collect1D (Node)
linkTitle: Collect1D
description: Accumulate a 1D histogram
---

{{< htable >}}
| | |
|-|-|
|Context|Analysis|
|Name Required?|Yes|
|Branches|`SubCollect`|
{{< /htable >}}

## Overview

The `Collect1D` node is used to accumulate a histogram of a one-dimensional quantity, extracted from a `Calculate`-style nodes.

## Description

Where the `Calculate*` nodes are responsible for calculating quantities, the `Collect*` nodes are responsible for accumulating averages, typically in histogram format.

`Collect1D` accumulates a histogram of a target value taken from a `Calculate*` node, over a specified range and binwidth. No post-processing (e.g. normalisation) is performed by `Collect1D` - see the [`Process1D`]({{< ref "Process1dnode" >}}) for processing options.

## Branching

The `Collect*` nodes all have a branch (accessed through the hidden `SubCollect` keyword) which is executed if the value is successfully binned (i.e. is within all ranges specified).

## Configuration

### Control Keywords

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`QuantityX`|`name`<br/>`[index]`|--|{{< required-label >}} The calculated observable to collect. The `name`d node must be one of the `Calculate*` nodes. The `index` specifies which value to take from multi-dimensional observables, starting from zero for the first. If not specified, index` defaults to zero.|
|`RangeX `|`min`<br/>`max`<br/>`delta`|--|Defines the range and binwidth of the histogram over which `QuantityX` will be accumulated.|
