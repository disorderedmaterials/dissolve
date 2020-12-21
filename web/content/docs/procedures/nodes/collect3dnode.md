---
title: Collect3D (Node)
linkTitle: Collect3D
description: Accumulate a 3D histogram
---

{{< htable >}}
| | |
|-|-|
|Context|Analysis|
|Name Required?|Yes|
|Branches|`SubCollect`|
{{< /htable >}}

## Overview

The `Collect3D` node is used to accumulate a histogram over three dimensions, using values extracted from three `Calculate`-style nodes.

## Description

Where the `Calculate*` nodes are responsible for calculating quantities, the `Collect*` nodes are responsible for accumulating averages, typically in histogram format.

`Collect3D` accumulates a histogram over three dimensions using target values taken from two `Calculate*` nodes, over a specified range and binwidth for each dimension. The values are treated as a triplet giving the {x,y,z} coordinate to bin in the histogram. No post-processing (e.g. normalisation) is performed by `Collect3D` - see the [`Process3D`]({{< ref "Process3dnode" >}}) for processing options.

## Branching

The `Collect*` nodes all have a branch (accessed through the hidden `SubCollect` keyword) which is executed if the value is successfully binned (i.e. is within all ranges specified).

## Configuration

### Control Keywords

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`QuantityX`|`name`<br/>`[index]`|--|{{< required-label >}} The calculated observable to use for the x axis. The `name`d node must be one of the `Calculate*` nodes. The `index` specifies which value to take from multi-dimensional observables, starting from zero for the first. If not specified, index` defaults to zero.|
|`QuantityY`|`name`<br/>`[index]`|--|{{< required-label >}} The calculated observable to use for the y axis. The `name`d node must be one of the `Calculate*` nodes. The `index` specifies which value to take from multi-dimensional observables, starting from zero for the first. If not specified, index` defaults to zero.|
|`QuantityZ`|`name`<br/>`[index]`|--|{{< required-label >}} The calculated observable to use for the z axis. The `name`d node must be one of the `Calculate*` nodes. The `index` specifies which value to take from multi-dimensional observables, starting from zero for the first. If not specified, index` defaults to zero.|
|`RangeX `|`min`<br/>`max`<br/>`delta`|--|Defines the range and binwidth of the x-axis of the histogram.|
|`RangeY `|`min`<br/>`max`<br/>`delta`|--|Defines the range and binwidth of the y-axis of the histogram.|
|`RangeZ `|`min`<br/>`max`<br/>`delta`|--|Defines the range and binwidth of the z-axis of the histogram.|
