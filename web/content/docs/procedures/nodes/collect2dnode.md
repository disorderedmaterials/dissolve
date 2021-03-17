---
title: Collect2D (Node)
linkTitle: Collect2D
description: Accumulate a 2D histogram
---

{{< htable >}}
| | |
|-|-|
|Context|Analysis|
|Name Required?|Yes|
|Branches|`SubCollect`|
{{< /htable >}}

## Overview

The `Collect2D` node is used to accumulate a histogram over two dimensions, using values extracted from two `Calculate`-style nodes.

## Description

Where the `Calculate*` nodes are responsible for calculating quantities, the `Collect*` nodes are responsible for accumulating averages, typically in histogram format.

`Collect2D` accumulates a histogram over two dimensions using target values taken from two `Calculate*` nodes, over a specified range and binwidth for each dimension. The values are treated as a pair giving the {x,y} coordinate to bin in the histogram. No post-processing (e.g. normalisation) is performed by `Collect2D` - see the [`Process2D`]({{< ref "Process2dnode" >}}) for processing options.

## Branching

The `Collect*` nodes all have a branch (accessed through the hidden `SubCollect` keyword) which is executed if the value is successfully binned (i.e. is within all ranges specified).

## Configuration

### Control Keywords

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`QuantityX`|`name`<br/>`[index]`|--|{{< required-label >}} The calculated observable to use for the x axis. The `name`d node must be one of the `Calculate*` nodes. The `index` specifies which value to take from multi-dimensional observables, starting from zero for the first. If not specified, index` defaults to zero.|
|`QuantityY`|`name`<br/>`[index]`|--|{{< required-label >}} The calculated observable to use for the y axis. The `name`d node must be one of the `Calculate*` nodes. The `index` specifies which value to take from multi-dimensional observables, starting from zero for the first. If not specified, index` defaults to zero.|
|`RangeX `|`min`<br/>`max`<br/>`delta`|--|Defines the range and binwidth of the x-axis of the histogram.|
|`RangeY `|`min`<br/>`max`<br/>`delta`|--|Defines the range and binwidth of the y-axis of the histogram.|
